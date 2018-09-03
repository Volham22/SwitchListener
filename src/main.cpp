#include <iostream>

#include "HidScanner.h"

int main()
{
    int res = hid_init();

    if(res)
    {
        std::cout << "Failed to initialize HidApi." << std::endl;
        return -1;
    }

    HidScanner scanner(ControllerType::ProController);

    if(scanner.ScanForControllers())
        std::cout << "A Pro Controller has been found !" << std::endl;
    else
        std::cout << "Nohing found." << std::endl;

    return 0;
}