#include <cstring>
#include "ConnectionsManager.h"

#ifdef _WIN32
#define SL_PLATFORM "SwitchListener - Windows\n"
#else
#define SL_PLATFORM "SwitchListener - GNU Linux\n"
#endif

int main(int argc, char* argv[])
{
    printf(SL_PLATFORM);

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
            #ifdef DEBUG_VERBOSE
            printf("Merging joycons mode\n");
            #endif
            mergingMode = true;
            break;
        }
    }
    ControllerHandler handler(mergingMode);
    handler.StartListening();

    hid_exit();
    return EXIT_SUCCESS;
}