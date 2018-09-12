#ifndef ANSWER_READER_H
#define ANSWER_READER_H

#include "HidComm.h"

struct ButtonsReport // Contain all buttons states
{
    bool X;
    bool Y;
    bool B;
    bool A;
    bool SR;
    bool SL;
    bool R;
    bool ZR;
    bool Minus;
    bool Plus;
    bool RStick;
    bool LStick;
    bool Home;
    bool Capture;
    bool Down;
    bool Up;
    bool Right;
    bool Left;
    bool L;
    bool ZL;

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