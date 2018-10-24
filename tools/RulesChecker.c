/*
 *  This program will check if udev rules are set for
 *  the differents Nintendo Controllers.
 *   If not it will add the missing ones.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SL_UDEV_RULES_PATH "/etc/udev/rules.d/switch_listener.rules"
#define BT_PROCONTROLLER_RULE "KERNEL==\"hidraw*\", KERNELS==\"*057E:2009*\", MODE=\"0666\"\n" // 0x2009 ID
#define BT_LJOYCON_RULE "KERNEL==\"hidraw*\", KERNELS==\"*057E:2006*\", MODE=\"0666\"\n" // 0x2007 ID
#define BT_RJOYCON_RULE "KERNEL==\"hidraw*\", KERNELS==\"*057E:2007*\", MODE=\"0666\"\n" // 0x2006 ID

void createRuleFile()
{
        FILE* file = fopen(SL_UDEV_RULES_PATH, "a+");

        /* Merging strings */
        char* fileString[strlen(BT_PROCONTROLLER_RULE) * 3]; // Create string buffer
        strcpy((char*)fileString, BT_PROCONTROLLER_RULE);
        strcat((char*)fileString, BT_LJOYCON_RULE);
        strcat((char*)fileString, BT_RJOYCON_RULE);

        fputs((char*)fileString, file);
        fclose(file);

        printf("Done.\n");
}

void rewriteRuleFile()
{
        FILE* file = fopen(SL_UDEV_RULES_PATH, "w");

        /* Merging strings */
        char* fileString[strlen(BT_PROCONTROLLER_RULE) * 3]; // Create string buffer
        strcpy((char*)fileString, BT_PROCONTROLLER_RULE);
        strcat((char*)fileString, BT_LJOYCON_RULE);
        strcat((char*)fileString, BT_RJOYCON_RULE);

        fputs((char*)fileString, file);
        fclose(file);

        printf("Done.\n");
}

int main(int argc, char* argv[])
{
    printf("=== Rules Checker for Switchlistener ===\n");

    if(getuid() != 0)
    {
        printf("  Please run Rules checker as root !\n");
        return 1;
    }

    if(argc > 1)
    {
        if(strstr(argv[1], "rewrite") != NULL)
        {
            rewriteRuleFile();

            return 0;
        }
    }

    if(access(SL_UDEV_RULES_PATH, F_OK) == -1)
    {
        printf("  Rule file does not exist, creating new ...\n");

        createRuleFile();

        return 0;
    }
    else
    {
        printf("File already exist add \"rewrite\" in arguments to write a new one.\n");
        
        return 0;
    }
}