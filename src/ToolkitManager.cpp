#include "ToolkitManager.h"

#ifdef _WIN32
#include <Windows.h>
#define SLEEP(x) Sleep(x)
#else
#include <unistd.h>
#define SLEEP(x) usleep(x)
#endif

#include <cstdlib>
#include <cstring>
#include <string>

#include "HidComm.h"

#define DUMP_SPI 1

// SPI Defines
#define PACKET_LENGTH   0x10    // A packet is 16b
#define MAX_PACKET_SIZE 0x1D    // 29b
#define SPI_MEMORY_SIZE 0x80000 // Memory is from 0x0 to 0x80000
#define MAX_ATTEMPTS    2000    // Maximum number of attempts to get a packet from the device

ToolkitManager::ToolkitManager()
{

}

void ToolkitManager::ListAvailableControllers()
{
    int nConnected = 0;
    while(m_Scanner.ScanForAnyController())
    {
        m_Devices[m_Scanner.GetHidDevice()] = m_Scanner.GetControllerType();
        nConnected++;
    }

    int n = 0;
    for(auto it = m_Devices.begin(); it != m_Devices.end(); it++, n++)
        printf("%d: Found a %s\n", n + 1, TypeToString(it->second));
    
    printf("Type wanted controller (number between 1 and %d): ", n);
    int selected = 0, choice = 0;
    scanf("%d", &selected);
    printf("Select what to do ?:\n  1.Dump SPI\n  2.Enable leds\nChoice: ");
    scanf("%d", &choice);

    switch (choice)
    {
        case DUMP_SPI:
        {
            DoSPIDump(GetDeviceFromSelection(selected), m_Devices[GetDeviceFromSelection(selected)]);
            break;
        }
    
    default:
        printf("Unknown choice. Please try again.\n");
        break;
    }
}

void ToolkitManager::DoSPIDump(hid_device* device, ControllerType type)
{
    printf("\nBegin of SPI Dump for %s :\n", TypeToString(type));

    HIDBuffer buffer = initBuffer();
    HIDBuffer commandArgs = initBuffer();
    commandArgs.BufferSize = 0x26;
    commandArgs.Buffer[0x4] = MAX_PACKET_SIZE;
    HidIO deviceIO = HidIO(device);

    // Open File
    std::string name = TypeToString(type); name += ".bin";
    FILE* file = fopen(name.c_str(), "wb");

    uint32_t *offset = (uint32_t*)commandArgs.Buffer;
    for(*offset = 0x0; *offset < SPI_MEMORY_SIZE; *offset += PACKET_LENGTH)
    {
        commandArgs.Buffer[0x4] = MAX_PACKET_SIZE;
        int attempts = 0;
        HIDBuffer ans = initBuffer();
        while(attempts <= MAX_ATTEMPTS)
        {
            attempts++;
            memcpy(buffer.Buffer, commandArgs.Buffer, 0x26);
            ans = deviceIO.SendSubCommandToDevice(commandArgs, 0x1, 0x10);

            if(ans.Buffer[0] == 0x21 && *(uint32_t*)&ans.Buffer[0xF] == *offset)
                break;
            
            SLEEP(5);
        }

        if(attempts > MAX_ATTEMPTS)
        {
            printf("Max attemps reached to retrieve data. Abord Dumping ....\n");
            return;
        }
        else
        {
            fwrite(ans.Buffer + 14 * sizeof(char), MAX_PACKET_SIZE, 1, file); // Reply data start on 14th byte

            if(*offset % 500 == 0) // Less spam
                printf("Dumped %f %%\n", ((float)*offset / (float)SPI_MEMORY_SIZE) * 100);
        }
    }

    fclose(file);
}

hid_device* ToolkitManager::GetDeviceFromSelection(int selection)
{
    int i = 0;
    hid_device* device = nullptr;
    for(auto it = m_Devices.begin(); it != m_Devices.end() && i < selection; it++, i++)
        device = it->first;

    return device;
}