#include <iostream>

#include "HidScanner.h"
#include "Controller.h"

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

    hid_device* controllerHandler = scanner.GetHidDevice();

    if(controllerHandler)
    {
        Controller controller(controllerHandler);

        if(controller.DoHandshake())
            std::cout << "Sucessfully Init" << std::endl;
        else
            std::cout << "Error in HandShake !" << std::endl;

        controller.DoControllerRoutine();
        std::cin.get();
    }
    else
    {
        wprintf(L"Error: %s", hid_error(controllerHandler));
        return EXIT_FAILURE;
    }

    return 0;
}