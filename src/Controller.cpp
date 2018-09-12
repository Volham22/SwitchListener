#include "Controller.h"

#include <cstring>
#include <cstdio>

#include "AnswerReader.h"


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

        command.Buffer[0] = 0x01 | 0x00 | 0x00 | 0x00; // Enable
        command.Buffer[1] = 0x00 | 0x00 | 0x00 | 0x00;
        command.BufferSize = 2;

        m_Com.SendSubCommandToDevice(command, 0x1, 0x30);
        printf("Enabled some leds.\n");
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    if(m_Com.IsConnected())
    {
        AnswerReader reader;

        HIDBuffer buffer = m_Com.ReadOnDevice();
        BatteryLevel battery = reader.ReadBatteryLevel(buffer);

        switch(battery)
        {
            case BatteryLevel::High:
                printf("Battery: High\n");
                break;

            case BatteryLevel::Medium:
                printf("Battery: Medium\n");
                break;

            case BatteryLevel::Low:
                printf("Battery: Low\n");
                break;

            case BatteryLevel::Critical:
                printf("Battery: Critical\n");
                break;

            case BatteryLevel::Unknow:
                printf("Battery: Unknow state\n");
                break;
        }
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    printf("=======End of Handshake=======\n\n");

    if(!m_Com.IsConnected())
    {
        m_IsInitialized = false;
        return false;
    }
    else
    {
        m_IsInitialized = true;
        return true;
    }
}

bool Controller::SetVibration(bool active)
{
    if(m_Com.IsConnected())
    {
        HIDBuffer buffer = initBuffer();
        buffer.BufferSize = 1;

        if(active)
        {
            buffer.Buffer[0] = 0x1;
            m_Com.SendSubCommandToDevice(buffer, 0x1, 0x48);

            printf("Enabled vibrations\n");
            return true;
        }
        else
        {
            buffer.Buffer[0] = 0x0;
            m_Com.SendSubCommandToDevice(buffer, 0x1, 0x48);

            printf("Disabled vibrations\n");
            return true;
        }
    }
    else
    {
        printf("Communication with Controller failed.\n Is the Controller connected?\n");
        return false;
    }
}

bool Controller::EnableIMU(bool active)
{
    if(m_Com.IsConnected())
    {
        HIDBuffer buffer = initBuffer();
        buffer.BufferSize = 1;

        if(active)
        {
            buffer.Buffer[0] = 0x1;
            m_Com.SendSubCommandToDevice(buffer, 0x1, 0x40);

            printf("Enabled IMU\n");
            return true;
        }
        else
        {
            buffer.Buffer[0] = 0x0;
            m_Com.SendSubCommandToDevice(buffer, 0x1, 0x40);

            printf("Disabled IMU\n");
            return true;   
        }
    }
    else
    {
        printf("Communication with Controller failed.\n Is the Controller connected?\n");
        return false;
    }
}

bool Controller::Disconnect()
{
    if(m_Com.IsConnected())
    {
        HIDBuffer buffer = initBuffer();
        buffer.BufferSize = 1;
        buffer.Buffer[0] = 0x0;

        m_Com.SendSubCommandToDevice(buffer, 0x1, 0x06);

        printf("Device Disconnected\n");
        return true;
    }
    else
    {
        printf("Device already disconnected\n");
        return false;
    }
}

Controller::~Controller()
{
    if(m_IsInitialized && m_Com.IsConnected())
        Disconnect();
}