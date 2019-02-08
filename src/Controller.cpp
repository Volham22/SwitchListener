#include "Controller.h"

#include <cstring>
#include <cstdio>
#include <unistd.h>

#include "AnswerReader.h"


static void PrintBatteryLevel(const BatteryLevel &level)
{
    switch(level)
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

        case BatteryLevel::Unknown:
            printf("Battery: Unknownn or empty\n");
            break;
    }  
}

Controller::Controller(hid_device* device)
: m_ControllerPosition(0), m_Device(nullptr), m_IsInitialized(false),
  m_Com(HidIO(device)), m_Interface(InterfaceManager(PRO_CONTROLLER_DEVICENAME))
{
    if(device) // Check if device exists
        m_Device = device;
}

bool Controller::DoHandshake(const uint8_t &controllerNumber)
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
        usleep(50);
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
        m_Com.SendSubCommandToDevice(command, 0x1, 0x40); // Enable it
        usleep(50);
        SetIMUSensitivity(3, 0, true, true); // Set Sensors to the defaults parameters
        usleep(50);
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
        usleep(50);
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
        SwitchPlayerLedOn(controllerNumber);
        m_ControllerPosition = controllerNumber;
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    if(m_Com.IsConnected())
    {
        AnswerReader reader;
        HIDBuffer buff;
        BatteryLevel level;

        for(int i = 0; i<5; i++) // 5 retry for reading battery level
        {
            buff = m_Com.ReadOnDevice();
            level = reader.ReadBatteryLevel(buff);

            if(level != BatteryLevel::Unknown)
                break;
        } 

        PrintBatteryLevel(level);      
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

bool Controller::SetIMUSensitivity(const uint8_t &gyroSensi, const uint8_t &acellsensi, const bool &gyroPerf, const bool &accelAAFilter)
{
    /*
     * The gyroscope sensitivity must be an integer betwenn 0 and 3
     * The accelerometer sensitivity must be an integer between 0 and 3
     * Gyroscope performance : True, 208hz (default), False 833hz
     * Accelerometer Anti-aliasing filter bandwidth : True 100hz (default), False 200hz
     */

    HIDBuffer args = initBuffer();
    args.BufferSize = 3;

    if(m_Com.IsConnected())
    {
        args.Buffer[0] = gyroSensi;
        args.Buffer[1] = acellsensi;
        args.Buffer[2] = gyroPerf ? 1 : 0;
        args.Buffer[3] = accelAAFilter ? 1 : 0;
        
        m_Com.SendSubCommandToDevice(args, 0x1, 0x41);

        return true;
    }
    else
    {
        printf("Communication with Controller failed.\n Is the Controller connected?\n");
        return false;
    }
}

bool Controller::SwitchPlayerLedOn(const uint8_t &ledNumber)
{
    /*
     * ledNumber must be between 1 and 4
     * 1 : Switch the first left led on
     * 2 : Switch the second left led on
     * 3 : Switch the third left led on
     * 4 : Switch the fourth left led on
     */

    if(m_Com.IsConnected())
    {
        HIDBuffer args = initBuffer();
        args.BufferSize = 2;

        if(ledNumber >= 1 && ledNumber <= 4)
        {
            args.Buffer[0] = ledNumber | 0x00 | 0x00 | 0x00;
            args.Buffer[1] = 0x00 | 0x00 | 0x00 | 0x00;

            m_Com.SendSubCommandToDevice(args, 0x1, 0x30);
            printf("Led %i activated.\n", ledNumber);

            return true;
        }
        else
        {
            printf("Warning: Led value must be between 1 and 4 !\n");
            return false;
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

void Controller::DoControllerRoutine()
{
    AnswerReader reader;

    if(m_Com.IsConnected())
    {
        HIDBuffer reportBuffer = m_Com.ReadOnDevice();
        ButtonsReport report = reader.ReadAnswer(reportBuffer);
        
        if(m_Interface.GetInterfaceStatus() == InterfaceStatus::INTERFACE_INIT_SUCCESS)
            m_Interface.CreateEvent(report);
        #ifdef DEBUG
        else
            printf("Error : Controller interface has not been initialized correctly\n Unable to register controller events\n");
        #endif
        
        #ifdef DEBUG
        if(isButtonsPressed(report))
        {
            // Print result in console for test purpose
            printf("======Button Report for Controller %i ======\n", m_ControllerPosition);
            printf("Button Y: %i\n", report.ButtonsStates[0] ? 1 : 0);
            printf("Button X: %i\n", report.ButtonsStates[1] ? 1 : 0);
            printf("Button B: %i\n", report.ButtonsStates[2] ? 1 : 0);
            printf("Button A: %i\n", report.ButtonsStates[3] ? 1 : 0);
            printf("Button SR: %i\n", report.ButtonsStates[4] ? 1 : 0);
            printf("Button SL: %i\n", report.ButtonsStates[5] ? 1 : 0);
            printf("Button R: %i\n", report.ButtonsStates[6] ? 1 : 0);
            printf("Button ZR: %i\n", report.ButtonsStates[7] ? 1 : 0);
            printf("Button Minus: %i\n", report.ButtonsStates[8] ? 1 : 0);
            printf("Button Plus: %i\n", report.ButtonsStates[9] ? 1 : 0);
            printf("Button RStick: %i\n", report.ButtonsStates[10] ? 1 : 0);
            printf("Button LStick: %i\n", report.ButtonsStates[11] ? 1 : 0);
            printf("Button Home: %i\n", report.ButtonsStates[12] ? 1 : 0);
            printf("Button Capture: %i\n", report.ButtonsStates[13] ? 1 : 0);
            printf("Button Down: %i\n", report.ButtonsStates[14] ? 1 : 0);
            printf("Button Up: %i\n", report.ButtonsStates[15] ? 1 : 0);
            printf("Button Right: %i\n", report.ButtonsStates[16] ? 1 : 0);
            printf("Button Left: %i\n", report.ButtonsStates[17] ? 1 : 0);
            printf("Button L: %i\n", report.ButtonsStates[20] ? 1 : 0);
            printf("Button ZL: %i\n", report.ButtonsStates[21] ? 1 : 0);
            printf("Right Stick y: %i\n", report.StickRight.Vertical);
            printf("Right Stick x: %i\n", report.StickRight.Horizontal);
            printf("Left Stick y: %i\n", report.StickLeft.Vertical);
            printf("Left Stick x: %i\n", report.StickLeft.Horizontal);
            printf("Accelerometer x: %i\n", report.Sensors.AccelX);
            printf("Accelerometer y: %i\n", report.Sensors.AccelY);
            printf("Accelerometer z: %i\n", report.Sensors.AccelZ);
            
            for(int i = 0; i<3; i++)
                printf("Gyroscope%i: %i\n", i, report.Sensors.GyroData[i]);

            PrintBatteryLevel((BatteryLevel)report.ControllerBattery);
            printf("=================================\n");
        }
        #endif
    }
}

Controller::~Controller()
{
    if(m_IsInitialized && m_Com.IsConnected())
        Disconnect();
}

JoyconController::JoyconController(hid_device* joyconR, hid_device* joyconL)
: m_JoyconLCom(HidIO(joyconL)), m_JoyConL(nullptr), m_ControllerPosition(0),
  m_Device(nullptr), m_IsInitialized(false), m_Com(HidIO(joyconR))
{
    if(joyconR && joyconL)
    {
        m_Device = joyconR;
        m_JoyConL = joyconL;
    }
}

bool JoyconController::DoHandshake(const uint8_t &controllerNumber)
{
    printf("====Beginning of Handshake====\n");

    if((!m_Com.IsConnected()) || (!m_JoyconLCom.IsConnected()))
        return false;

    HIDBuffer command = initBuffer(); // Setting new buffer for command args

    if(m_Com.IsConnected() && m_JoyconLCom.IsConnected())
    {
        // Enable Vibration (SubcommandID 0x48)
        command.Buffer[0] = 0x01; // Set to true
        command.BufferSize = 1; // still size of uint8_t
        m_Com.SendSubCommandToDevice(command, 0x1, 0x48);
        m_JoyconLCom.SendSubCommandToDevice(command, 0x1, 0x48);
        printf("Enabled Vibration\n");
        usleep(50);
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    if(m_Com.IsConnected() && m_JoyconLCom.IsConnected())
    {
        command = initBuffer(); // Reset buffer

        // Enable IMU (Controller sensors) (command 0x40)
        command.Buffer[0] = 0x01; // Set it to true
        command.BufferSize = 1;
        m_Com.SendSubCommandToDevice(command, 0x1, 0x40); // Enable it on right joycon
        m_JoyconLCom.SendSubCommandToDevice(command, 0x1, 0x40); // Enable it on left joycon
        usleep(50);
        SetIMUSensitivity(3, 0, true, true); // Set Sensors to the defaults parameters
        usleep(50);
        printf("Enabled IMU\n");
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    if(m_Com.IsConnected() && m_JoyconLCom.IsConnected())
    {
        command = initBuffer(); // Reset buffer

        // Increase Bluetooth Speed and packet size for NFC/IR (subcommand 0x3)
        command.Buffer[0] = 0x31; // Larger packet size
        command.BufferSize = 1;
        m_Com.SendSubCommandToDevice(command, 0x1, 0x3);
        m_JoyconLCom.SendSubCommandToDevice(command, 0x1, 0x3);
        usleep(50);
        printf("Initialized Bluetooth for NFC/IR\n");
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    // Enable Player leds
    if(m_Com.IsConnected() && m_JoyconLCom.IsConnected())
    {
        SwitchPlayerLedOn(controllerNumber);
        m_ControllerPosition = controllerNumber;
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    if(m_Com.IsConnected())
    {
        AnswerReader reader;
        HIDBuffer rBuffer;
        HIDBuffer lBuffer;
        BatteryLevel rLevel;
        BatteryLevel lLevel;

        for(int i = 0; i<5; i++) // 5 retry for reading battery level
        {
            rBuffer = m_Com.ReadOnDevice();
            lBuffer = m_JoyconLCom.ReadOnDevice();
            rLevel = reader.ReadBatteryLevel(rBuffer);
            lLevel = reader.ReadBatteryLevel(lBuffer);

            if(rLevel != BatteryLevel::Unknown && lLevel != BatteryLevel::Unknown)
                break;
        } 

        if((int8_t)rLevel < (int8_t)lLevel)
            PrintBatteryLevel(rLevel);
        else
            PrintBatteryLevel(lLevel);
    }
    else
    {
        printf("=======End of Handshake=======\n");
        return false;
    }

    printf("=======End of Handshake=======\n\n");

    if((!m_Com.IsConnected()) && (!m_JoyconLCom.IsConnected()))
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

bool JoyconController::SetVibration(bool active)
{
    if(m_Com.IsConnected() && m_JoyconLCom.IsConnected())
    {
        HIDBuffer buffer = initBuffer();
        buffer.BufferSize = 1;

        if(active)
        {
            buffer.Buffer[0] = 0x1;
            m_Com.SendSubCommandToDevice(buffer, 0x1, 0x48);
            m_JoyconLCom.SendSubCommandToDevice(buffer, 0x1, 0x48);

            printf("Enabled vibrations\n");
            return true;
        }
        else
        {
            buffer.Buffer[0] = 0x0;
            m_Com.SendSubCommandToDevice(buffer, 0x1, 0x48);
            m_JoyconLCom.SendSubCommandToDevice(buffer, 0x1, 0x48);

            printf("Disabled vibrations\n");
            return true;
        }
    }
    else
    {
        printf("Communication with on of the Joycon failed.\n Is the both joycons connected?\n");
        return false;
    }
}

bool JoyconController::EnableIMU(bool active)
{
    if(m_Com.IsConnected() && m_JoyconLCom.IsConnected())
    {
        HIDBuffer buffer = initBuffer();
        buffer.BufferSize = 1;

        if(active)
        {
            buffer.Buffer[0] = 0x1;
            m_Com.SendSubCommandToDevice(buffer, 0x1, 0x40);
            m_JoyconLCom.SendSubCommandToDevice(buffer, 0x1, 0x40);

            printf("Enabled IMU\n");
            return true;
        }
        else
        {
            buffer.Buffer[0] = 0x0;
            m_Com.SendSubCommandToDevice(buffer, 0x1, 0x40);
            m_JoyconLCom.SendSubCommandToDevice(buffer, 0x1, 0x40);

            printf("Disabled IMU\n");
            return true;   
        }
    }
    else
    {
        printf("Communication with on of the Joycon failed.\n Is the both joycons connected?\n");
        return false;
    }
}

bool JoyconController::SetIMUSensitivity(const uint8_t &gyroSensi, const uint8_t &acellsensi, const bool &gyroPerf, const bool &accelAAFilter)
{
    HIDBuffer args = initBuffer();
    args.BufferSize = 3;

    if(m_Com.IsConnected() && m_JoyconLCom.IsConnected())
    {
        args.Buffer[0] = gyroSensi;
        args.Buffer[1] = acellsensi;
        args.Buffer[2] = gyroPerf ? 1 : 0;
        args.Buffer[3] = accelAAFilter ? 1 : 0;
        
        m_Com.SendSubCommandToDevice(args, 0x1, 0x41);
        m_JoyconLCom.SendSubCommandToDevice(args, 0x1, 0x41);

        return true;
    }
    else
    {
        printf("Communication with on of the Joycon failed.\n Is the both joycons connected?\n");
        return false;
    }
}

bool JoyconController::SwitchPlayerLedOn(const uint8_t &ledNumber)
{
    if(m_Com.IsConnected() && m_JoyconLCom.IsConnected())
    {
        HIDBuffer args = initBuffer();
        args.BufferSize = 2;

        if(ledNumber >= 1 && ledNumber <= 4)
        {
            args.Buffer[0] = ledNumber | 0x00 | 0x00 | 0x00;
            args.Buffer[1] = 0x00 | 0x00 | 0x00 | 0x00;

            m_Com.SendSubCommandToDevice(args, 0x1, 0x30);
            m_JoyconLCom.SendSubCommandToDevice(args, 0x1, 0x30);
            printf("Led %i activated.\n", ledNumber);

            return true;
        }
        else
        {
            printf("Warning: Led value must be between 1 and 4 !\n");
            return false;
        }
    }
    else
    {
        printf("Communication with on of the Joycon failed.\n Is the both joycons connected?\n");
        return false;
    }
}

bool JoyconController::Disconnect()
{
    HIDBuffer buffer = initBuffer();
    buffer.BufferSize = 1;
    buffer.Buffer[0] = 0x0;

    if(m_Com.IsConnected())
    {
        m_Com.SendSubCommandToDevice(buffer, 0x1, 0x06);

        printf("Right Joycon Disconnected\n");
        return true;
    }
    else
    {
        printf("Right Joycon already disconnected\n");
        return false;
    }

    if(m_JoyconLCom.IsConnected())
    {
        m_JoyconLCom.SendSubCommandToDevice(buffer, 0x1, 0x06);

        printf("Left Joycon Disconnected\n");
        return true;
    }
    else
    {
        printf("Left Joycon already disconnected\n");
        return false;
    }
}

void JoyconController::DoControllerRoutine()
{
    AnswerReader reader;

    if(m_Com.IsConnected() && m_JoyconLCom.IsConnected())
    {
        /* Reading informations from both joycons */
        HIDBuffer RReportBuffer = m_Com.ReadOnDevice();
        HIDBuffer LReportBuffer = m_JoyconLCom.ReadOnDevice();

        ButtonsReport RReport = reader.ReadAnswer(RReportBuffer);
        ButtonsReport LReport = reader.ReadAnswer(LReportBuffer);
        
        #ifdef DEBUG
        if(isButtonsPressed(RReport) || isButtonsPressed(LReport))
        {
            // Print result in console for test purpose
            printf("======Button Report for Controller %i ======\n", m_ControllerPosition);
            printf("Button Y: %i\n", RReport.ButtonsStates[0] ? 1 : 0);
            printf("Button X: %i\n", RReport.ButtonsStates[1] ? 1 : 0);
            printf("Button B: %i\n", RReport.ButtonsStates[2] ? 1 : 0);
            printf("Button A: %i\n", RReport.ButtonsStates[3] ? 1 : 0);
            printf("Button R: %i\n", RReport.ButtonsStates[6] ? 1 : 0);
            printf("Button ZR: %i\n", RReport.ButtonsStates[7] ? 1 : 0);
            printf("Button Minus: %i\n", LReport.ButtonsStates[8] ? 1 : 0);
            printf("Button Plus: %i\n", RReport.ButtonsStates[9] ? 1 : 0);
            printf("Button RStick: %i\n", RReport.ButtonsStates[10] ? 1 : 0);
            printf("Button LStick: %i\n", LReport.ButtonsStates[11] ? 1 : 0);
            printf("Button Home: %i\n", RReport.ButtonsStates[12] ? 1 : 0);
            printf("Button Capture: %i\n", LReport.ButtonsStates[13] ? 1 : 0);
            printf("Button Down: %i\n", LReport.ButtonsStates[14] ? 1 : 0);
            printf("Button Up: %i\n", LReport.ButtonsStates[15] ? 1 : 0);
            printf("Button Right: %i\n", LReport.ButtonsStates[16] ? 1 : 0);
            printf("Button Left: %i\n", LReport.ButtonsStates[17] ? 1 : 0);
            printf("Button L: %i\n", LReport.ButtonsStates[20] ? 1 : 0);
            printf("Button ZL: %i\n", LReport.ButtonsStates[21] ? 1 : 0);
            printf("Right Stick y: %i\n", RReport.StickRight.Vertical);
            printf("Right Stick x: %i\n", RReport.StickRight.Horizontal);
            printf("Left Stick y: %i\n", LReport.StickLeft.Vertical);
            printf("Left Stick x: %i\n", LReport.StickLeft.Horizontal);
            printf("Accelerometer x: %i\n", RReport.Sensors.AccelX);
            printf("Accelerometer y: %i\n", RReport.Sensors.AccelY);
            printf("Accelerometer z: %i\n", RReport.Sensors.AccelZ);
            
            for(int i = 0; i<3; i++)
                printf("Gyroscope%i: %i\n", i, RReport.Sensors.GyroData[i]);

            printf("Right Joycon:");
            PrintBatteryLevel((BatteryLevel)RReport.ControllerBattery);
            printf("Left Joycon:");
            PrintBatteryLevel((BatteryLevel)LReport.ControllerBattery);
            printf("=================================\n");
        }
        #endif
    }
}

JoyconController::~JoyconController()
{
    if(m_IsInitialized)
        Disconnect();
}