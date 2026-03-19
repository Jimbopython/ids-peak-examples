#include <peak_icv/peak_icv.hpp>
#include <iostream>

int main()
{
    try
    {
        peak::icv::library::Init();

        peak::icv::library::Exit();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
    std::cout << "Success!\n";
    return 0;
}
