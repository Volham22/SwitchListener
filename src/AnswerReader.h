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
     L,
     ZL
};

struct ButtonsReport // Contain all buttons states
{
    bool ButtonsStates[22];
    int ControllerBattery;
};

enum BatteryLevel { Unknow = -1, Critical = 0, Low = 1, Medium = 2, High = 3 };

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
};

#endif