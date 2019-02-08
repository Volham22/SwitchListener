#ifndef ANSWER_READER_H
#define ANSWER_READER_H

#include "HidComm.h"

enum Buttons {
     Y,
     X,
     B,
     A,
     SR,
     SL,
     R,
     ZR,
     Minus,
     Plus,
     RStick,
     LStick,
     Home,
     Capture,
     Down,
     Up,
     Right,
     Left,
     L  = 20,
     ZL = 21
};

struct StickReport
{
    uint16_t Horizontal;
    uint16_t Vertical;
};

struct SensorsReport
{
    uint16_t AccelX;
    uint16_t AccelY;
    uint16_t AccelZ;

    uint16_t GyroData[3];
};

struct ButtonsReport // Contain all buttons states
{
    bool ButtonsStates[22];
    int ControllerBattery;

    StickReport StickLeft;
    StickReport StickRight;

    SensorsReport Sensors;
};


enum BatteryLevel { Unknown = -1, Critical = 0, Low = 1, Medium = 2, High = 3 };

bool isButtonsPressed(const ButtonsReport &report);

class AnswerReader
{
public:
    AnswerReader();
    ButtonsReport ReadAnswer(const HIDBuffer &reply) const;
    // TODO: Reply for subcommands functions ...
    BatteryLevel ReadBatteryLevel(const HIDBuffer &reply) const;
    HIDBuffer GetSensorsData(const HIDBuffer &reply) const;
    HIDBuffer GetNFCData(const HIDBuffer &reply) const;
private:
    void AnalyzeInputType(const HIDBuffer &reply) const;
    bool IsStandardInput(const HIDBuffer &reply) const;
    SensorsReport DecodeSensors(const HIDBuffer &reply) const;
};

#endif