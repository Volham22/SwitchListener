#include "ConnectionsManager.h"

int main()
{
    int res = hid_init();

    if(res)
    {
        printf("Failed to initialise HidAPI !\n");
        return EXIT_FAILURE;
    }

    ControllerHandler handler;
    handler.StartListening();

    return EXIT_SUCCESS;
}