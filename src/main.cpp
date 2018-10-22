#include <iostream>

#include "ConnectionsManager.h"

int main()
{
    int res = hid_init();

    if(res)
    {
        std::cout << "Failed to initialize HidApi." << std::endl;
        return EXIT_FAILURE;
    }

    ControllerHandler handler;
    handler.StartListening();

    return EXIT_SUCCESS;
}