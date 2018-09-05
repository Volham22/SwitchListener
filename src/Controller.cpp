#include "Controller.h"

#include <cstring>
#include <cstdio>


Controller::Controller(hid_device* device)
: m_Com(device), m_Device(nullptr), m_IsInitialized(false)
{
    if(device) // Check if device exists
        m_Device = device;
}

bool Controller::DoHandshake()
{
    /* 
     * Setting up few things to make the controller working
     * All subcommands details are here :
     * https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/bluetooth_hid_subcommands_notes.md
     * Credit: dekuNukem
     */

    printf("====Beginning of Handshake====\n");

    if(!m_Com.IsConnected())
        return false;

    HIDBuffer command = initBuffer(); // Setting new buffer for command args

    if(m_Com.IsConnected())
    {
        // Enable Vibration (SubcommandID 0x48)
        command.Buffer[0] = 0x01; // Set to true
        command.BufferSize = 1; // still size of uint8_t
        m_Com.SendSubCommandToDevice(command, 0x1, 0x48);
        printf("Enabled Vibration\n");
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    if(m_Com.IsConnected())
    {
        command = initBuffer(); // Reset buffer

        // Enable IMU (Controller sensors) (command 0x40)
        command.Buffer[0] = 0x01; // Set it to true
        command.BufferSize = 1;
        m_Com.SendSubCommandToDevice(command, 0x1, 0x40);
        printf("Enabled IMU\n");
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    if(m_Com.IsConnected())
    {
        command = initBuffer(); // Reset buffer

        // Increase Bluetooth Speed and packet size for NFC/IR (subcommand 0x3)
        command.Buffer[0] = 0x31; // Larger packet size
        command.BufferSize = 1;
        m_Com.SendSubCommandToDevice(command, 0x1, 0x3);
        printf("Initialized Bluetooth for NFC/IR\n");
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    // Enable Player leds
    if(m_Com.IsConnected())
    {
        command = initBuffer(); // Reset buffer

        command.Buffer[0] = 0x80 | 0x40 | 0x2 | 0x1;
        command.Buffer[1] = 0x80 | 0x40 | 0x2 | 0x1;
        command.BufferSize = 2;

        m_Com.SendSubCommandToDevice(command, 0x1, 0x30);
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    printf("=======End of Handshake=======\n");

    if(!m_Com.IsConnected())
        return false;
    else
        return true;
}