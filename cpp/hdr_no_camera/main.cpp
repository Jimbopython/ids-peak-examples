/*
 * Copyright(C) 2026, IDS Imaging Development Systems GmbH.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef _WIN32
#    define _CRT_SECURE_NO_WARNINGS
#    include <windows.h>
#else
#    include <unistd.h>
#endif

#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#include <peak_icv/peak_icv.hpp>

namespace
{
std::string GetDirectoryFromFilePath(const std::string& filePath);
std::string GetExecutablePath();
std::string GetCurrentExecutableDir();
std::string GetCurrentDateTime();
std::string GetImageFilePath();
std::string GetCalibrationImageFilePath();
std::string GetProcessingImageFilePath();
std::string GetToneMappedLdrImageFilePath();
std::string GetHdrImageFilePath();
std::vector<peak::icv::Image> ReadImagesFromDir(
    const std::string& directoryPath, const std::vector<double>& exposureTimes);
std::vector<peak::icv::Image> ReadCalibrationImagesFromDir(const std::string& directoryPath);
std::vector<peak::icv::Image> ReadProcessingImagesFromDir(const std::string& directoryPath);
} // namespace

int main()
{
    try
    {
        peak::icv::library::Init();

        const peak::icv::ImageWriter writer;

        std::cout << "Initialize HDR" << std::endl;
        peak::icv::experimental::HDR hdr;

        std::cout << "Loading calibration images from " << GetCalibrationImageFilePath() << std::endl;
        const auto calibrationImages = ReadCalibrationImagesFromDir(GetCalibrationImageFilePath());

        std::cout << "Estimate camera response curve" << std::endl;
        hdr.EstimateResponseCurve(calibrationImages);

        std::cout << "Loading processing images from " << GetProcessingImageFilePath() << std::endl;
        const auto processingImages = ReadProcessingImagesFromDir(GetProcessingImageFilePath());

        // It is also possible to skip the Calibrate method, then calibration is done on first call of Process method
        std::cout << "Process HDR image" << std::endl;
        auto hdrImage = hdr.Process(processingImages);

        writer.Write(GetHdrImageFilePath(), hdrImage);
        std::cout << "HDR image saved to " << GetHdrImageFilePath() << std::endl;

        std::cout << "Initialize tone mapping" << std::endl;
        peak::icv::experimental::ToneMapping toneMapping;

        std::cout << "Tone mapping of HDR image" << std::endl;
        auto ldrImage = toneMapping.Process(hdrImage);

        writer.Write(GetToneMappedLdrImageFilePath(), ldrImage);
        std::cout << "Tone mapped ldr image saved to " << GetToneMappedLdrImageFilePath() << std::endl;

        peak::icv::library::Exit();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

namespace
{
std::string GetDirectoryFromFilePath(const std::string& filePath)
{
    const auto lastSlashPos = filePath.find_last_of("\\/");
    return filePath.substr(0, lastSlashPos + 1);
}

std::string GetExecutablePath()
{
    using std::min;

    std::array<char, FILENAME_MAX> buffer{};
#ifdef _WIN32
    auto count = GetModuleFileName(NULL, buffer.data(), static_cast<DWORD>(buffer.size()));
#else
    auto count = readlink("/proc/self/exe", buffer.data(), buffer.size());
#endif
    count = min(count, static_cast<decltype(count)>(buffer.size() - 1));

    auto executablePath = std::string{ buffer.begin(), buffer.begin() + count };

#ifdef _WIN32
    std::replace(executablePath.begin(), executablePath.end(), '\\', '/');
#endif
    return executablePath;
}

std::string GetCurrentExecutableDir()
{
    const auto executablePath = GetExecutablePath();
    return GetDirectoryFromFilePath(executablePath);
}

std::string GetCurrentDateTime()
{
    const auto now = std::chrono::system_clock::now();
    const auto inTime = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&inTime), "%Y-%m-%d_%H-%M-%S");
    return ss.str();
}

std::string GetImageFilePath()
{
    return GetCurrentExecutableDir() + DATA_PATH "hdr";
}

std::string GetCalibrationImageFilePath()
{
    return GetImageFilePath() + "/" + "calibration";
}

std::string GetProcessingImageFilePath()
{
    return GetImageFilePath() + "/" + "processing";
}

std::string GetToneMappedLdrImageFilePath()
{
    return "tone_mapped_ldr_image_" + GetCurrentDateTime() + ".png";
}

std::string GetHdrImageFilePath()
{
    return "hdr_image_" + GetCurrentDateTime() + ".tiff";
}

std::string ExposureToString(const double exposure)
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << exposure;
    std::string s = ss.str();

    for (char& c : s)
    {
        if (c == '.')
        {
            c = '_';
        }
    }

    return s;
}

std::vector<peak::icv::Image> ReadImagesFromDir(
    const std::string& directoryPath, const std::vector<double>& exposureTimes)
{
    std::vector<peak::icv::Image> images;
    images.reserve(exposureTimes.size());

    for (double exposureTime : exposureTimes)
    {
        std::stringstream ss;
        ss << directoryPath << "/mono_exposure_" << ExposureToString(exposureTime / 1'000) << "_ms.png";

        peak::icv::Image image{ ss.str() };

        peak::common::Metadata metaData;
        metaData.SetValueByKey<peak::common::MetadataKey::DeviceExposureTime>(exposureTime);
        image.SetMetadata(metaData);

        images.emplace_back(image);
    }

    return images;
}

std::vector<peak::icv::Image> ReadCalibrationImagesFromDir(const std::string& directoryPath)
{
    return ReadImagesFromDir(directoryPath, { 100, 500, 2'000, 3'000, 8'000, 12'000 });
}

std::vector<peak::icv::Image> ReadProcessingImagesFromDir(const std::string& directoryPath)
{
    return ReadImagesFromDir(directoryPath, { 1'000, 4'000 });
}
} // namespace
