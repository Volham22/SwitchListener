#include <iostream>

#include "HidScanner.h"
#include "Controller.h"

int main()
{
    int res = hid_init();

    if(res)
    {
        std::cout << "Failed to initialize HidApi." << std::endl;
        return EXIT_FAILURE;
    }

    HidScanner scanner(ControllerType::Any);

    if(!scanner.ScanForAnyController())
    {
        std::cout << "Nohing found." << std::endl;
        return EXIT_FAILURE;
    }

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

    return EXIT_SUCCESS;
}