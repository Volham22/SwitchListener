#include "ConnectionsManager.h"

int main()
{
    int res = hid_init();

    if(res)
    {
        printf("Failed to initialise HidAPI !\n");
        return EXIT_FAILURE;
    }

    ControllerHandler handler(true);
    handler.StartListening();

    return EXIT_SUCCESS;
}