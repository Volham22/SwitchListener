#include "AnswerReader.h"

#include <stdio.h>
#include <cstring>

#ifdef DEBUG
static void PrintBytes(const uchar &byte)
{
    printf("Warning: Unknow value: ");
    printf("%02x\n", byte);
}
#endif

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

        /* Reading Sensors */
        report.Sensors = DecodeSensors(reply);

        /* Reporting Battery level */
        report.ControllerBattery = ReadBatteryLevel(reply);

        return report;
    }
    else
    {
        return { 0, 0, 0, 0, 0 };
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
                #ifdef DEBUG
                PrintBytes(batteryInfo);
                #endif
                return BatteryLevel::Unknow;
        }
    }
    else
    {
        #ifdef DEBUG
        printf("Battery:\n  ");
        PrintBytes(reply.Buffer[2] >> 4);
        #endif
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

SensorsReport AnswerReader::DecodeSensors(const HIDBuffer &reply) const
{
    SensorsReport report = { 0, 0, 0 };
    uint16_t* accel = nullptr;

    /* Accelerometer X byte 13, 14 */
    accel = (uint16_t*)reply.Buffer + 13;
    report.AccelX = *accel;

    /* Accelerometer Y byte 15, 16 */
    accel = (uint16_t*)reply.Buffer + 15;
    report.AccelY = *accel;

    /* Accelerometer Z byte 17, 18 */
    accel = (uint16_t*)reply.Buffer + 17;
    report.AccelZ = *accel;

    /* Gyroscope Data byte 19 to 24 */
    int cnt = 0;
    for(unsigned int i = 19; i<24; i+=2)
    {
        accel = (uint16_t*)reply.Buffer + i;
        report.GyroData[cnt] = *accel;
        cnt++;
    }

    return report;
}

bool AnswerReader::IsStandardInput(const HIDBuffer &reply) const
{
    switch(reply.Buffer[0])
    {
        case 0x30:
            return true;
        
        case 0x31:
            return true;

        case 0x32:
            return true;

        case 0x33:
            return true;

        case 0x21:
            return true;

        default:
            return false;
    }
}