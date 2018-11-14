#include <cstring>
#include "ConnectionsManager.h"

int main(int argc, char* argv[])
{
    int res = hid_init();

    if(res)
    {
        printf("Failed to initialise HidAPI !\n");
        return EXIT_FAILURE;
    }

    bool mergingMode = false;
    
    /* Reading args temp */
    for(int i = 0; i<argc; i++)
    {
        if(!strcmp("merge", argv[i]))
        {
            printf("Merging joycons mode\n");
            mergingMode = true;
            break;
        }
    }
    ControllerHandler handler(mergingMode);
    handler.StartListening();

    return EXIT_SUCCESS;
}