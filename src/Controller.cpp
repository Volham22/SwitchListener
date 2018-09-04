#include "Controller.h"

#include <cstring>
#include <cstdio>

Controller::Controller(hid_device* device)
: m_Com(device), m_Device(nullptr), m_IsInitialized(false)
{
    if(device)
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

    ControllerCommand command;
    memset(command.Data, 0, INPUT_BUFFER_SIZE); // 1 is sizeof(uint8_t)

    if(m_Com.IsConnected())
    {
        // Enable Vibration (SubcommandID 0x48)
        command.Data[0] = 0x01; // Set to true
        command.CommandID = 0x01;
        command.DataSize = 1; // still size of uint8_t
        m_Com.SendSubCommandToDevice(command, 0x48);
        printf("Enabled Vibration\n");
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    if(m_Com.IsConnected())
    {
        // Enable IMU (Controller sensors)
        command.Data[0] = 0x01; // Set it to true
        command.CommandID = 0x01;
        command.DataSize = 1;
        m_Com.SendSubCommandToDevice(command, 0x40);
        printf("Enabled IMU\n");
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    if(m_Com.IsConnected())
    {
        // Increase Bluetooth Speed and packet size for NFC/IR
        command.Data[0] = 0x31; // Larger packet size
        command.CommandID = 0x01;
        command.DataSize = 1;
        m_Com.SendSubCommandToDevice(command, 0x3);
        printf("Initialized Bluetooth for NFC/IR\n");
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