cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(hdr_no_camera LANGUAGES C CXX)

add_executable(${PROJECT_NAME} main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

find_package(ids_peak_icv 1.3 REQUIRED)

# These functions will add a post-build steps to your target
# in order to copy all needed files (e.g. DLL's) to the output directory.
ids_peak_icv_deploy(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE ids_peak_icv)

configure_file(data/calibration/mono_exposure_0_1_ms.png ${CMAKE_CURRENT_BINARY_DIR}/calibration/mono_exposure_0_1_ms.png COPYONLY)
configure_file(data/calibration/mono_exposure_0_5_ms.png ${CMAKE_CURRENT_BINARY_DIR}/calibration/mono_exposure_0_5_ms.png COPYONLY)
configure_file(data/calibration/mono_exposure_1_0_ms.png ${CMAKE_CURRENT_BINARY_DIR}/calibration/mono_exposure_1_0_ms.png COPYONLY)
configure_file(data/calibration/mono_exposure_2_0_ms.png ${CMAKE_CURRENT_BINARY_DIR}/calibration/mono_exposure_2_0_ms.png COPYONLY)
configure_file(data/calibration/mono_exposure_3_0_ms.png ${CMAKE_CURRENT_BINARY_DIR}/calibration/mono_exposure_3_0_ms.png COPYONLY)
configure_file(data/calibration/mono_exposure_4_0_ms.png ${CMAKE_CURRENT_BINARY_DIR}/calibration/mono_exposure_4_0_ms.png COPYONLY)
configure_file(data/calibration/mono_exposure_8_0_ms.png ${CMAKE_CURRENT_BINARY_DIR}/calibration/mono_exposure_8_0_ms.png COPYONLY)
configure_file(data/calibration/mono_exposure_12_0_ms.png ${CMAKE_CURRENT_BINARY_DIR}/calibration/mono_exposure_12_0_ms.png COPYONLY)

configure_file(data/processing/mono_exposure_1_0_ms.png ${CMAKE_CURRENT_BINARY_DIR}/processing/mono_exposure_1_0_ms.png COPYONLY)
configure_file(data/processing/mono_exposure_4_0_ms.png ${CMAKE_CURRENT_BINARY_DIR}/processing/mono_exposure_4_0_ms.png COPYONLY)