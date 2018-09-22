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
        int position = 0;

        for(int iByte = 3; iByte<=5; iByte++)
        {
            for(unsigned int iBit = 0; iBit<8; iBit++)
            {
                report.ButtonsStates[position] = reply.Buffer[iByte] & (1 << iBit);
                position++;

                if(iByte == 4 && iBit == 5) // No need to scan 2 last bits in the fith byte
                    break;
            }
        }

        /* Left Stick */
        report.StickLeft.Horizontal = reply.Buffer[6] | ((reply.Buffer[7] & 0xF) << 8);
        report.StickLeft.Vertical = (reply.Buffer[7] >> 4) | (reply.Buffer[8] << 4);

        /* Right Stick */
        report.StickRight.Horizontal = reply.Buffer[9] | ((reply.Buffer[10] & 0xF) << 8);
        report.StickRight.Vertical = (reply.Buffer[10] >> 4) | (reply.Buffer[11] << 4);

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