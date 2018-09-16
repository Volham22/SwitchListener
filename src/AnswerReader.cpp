#include "AnswerReader.h"

#include <stdio.h>
#include <cstring>

static void PrintBytes(const uchar &byte)
{
    printf("Warning: Unknow value: ");
    printf("%02x\n", byte);
}

AnswerReader::AnswerReader() {}

ButtonsReport AnswerReader::ReadAnswer(const HIDBuffer &reply) const
{
    if(IsStandardInput(reply))
    {
        ButtonsReport report = { false, 0 };
        /* 
         * Reading the fourth byte
         * Y, X, B, A, SR, SL, R, ZR
         */
        for(int i = 0; i<8; i++)
        {
            if(reply.Buffer[3] & (1 << i))
                report.ButtonsStates[i] = true;
        }

        /* 
         * Reading the fifth byte
         * Minus, Plus, RStick, LStick, Home, Capture, unknow, unknow
         */
        for(int i = 0; i<6; i++)
        {
            if(reply.Buffer[4] & (1 << i))
                report.ButtonsStates[8 + i] = true;

        }

        /* 
         * Reading the sixth byte
         * Down, Up, Right, Left, SR, SL, L, ZL
         */
        for(int i = 0; i<8; i++)
        {
            if(reply.Buffer[5] & (1 << i))
                report.ButtonsStates[14 + i] = true;

        }

        /* Reporting Battery level */
        report.ControllerBattery = ReadBatteryLevel(reply);

        return report;
    }
}

BatteryLevel AnswerReader::ReadBatteryLevel(const HIDBuffer &reply) const
{
    if(IsStandardInput(reply))
    {
        uint8_t batteryInfo = reply.Buffer[2];
        batteryInfo = batteryInfo >> 4;

        switch(batteryInfo)
        {
            case 8:
                return BatteryLevel::High;

            case 6:
                return BatteryLevel::Medium;

            case 4:
                return BatteryLevel::Low;

            case 2:
                return BatteryLevel::Critical;

            default:
                printf("Battery:\n  ");
                PrintBytes(batteryInfo);
                return BatteryLevel::Unknow;
        }
    }
    else
    {
        printf("Battery:\n  ");
        PrintBytes(reply.Buffer[2] >> 4);
        return BatteryLevel::Unknow;
    }
}

//HIDBuffer AnswerReader::GetNFCData(const HIDBuffer &reply) const
//{
    /* Command ID 0x31 */
    // TODO:
//}

//HIDBuffer AnswerReader::GetSensorsData(const HIDBuffer &reply) const
//{
    /* Command ID 0x30 0x31 0x32 0x33 */
    // TODO:
//}

bool AnswerReader::IsStandardInput(const HIDBuffer &reply) const
{
    if(reply.Buffer[0] == 0x30 || reply.Buffer[0] == 0x31
    || reply.Buffer[0] == 0x32 || reply.Buffer[0] == 0x33
    || reply.Buffer[0] == 0x21)
        return true;
    else
        return false;
}