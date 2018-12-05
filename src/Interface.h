#ifndef INTERFACE_H
#define INTERFACE_H

#include "AnswerReader.h"

#define DEVICENAME_JOYCON_SIZE           18
#define DEVICENAME_PRO_CONTROLLER_SIZE   24
#define DEVICENAME_UNKNOWN_SIZE          28
#define JOYCON_L_DEVICENAME              "Nintendo Joycon L"
#define JOYCON_R_DEVICENAME              "Nintendo Joycon R"
#define PRO_CONTROLLER_DEVICENAME        "Nintendo Pro Controller"
#define UNKNOWN_DEVICENAME               "Nintendo unknown Controller"

enum InterfaceStatus {
    UDVEV_ERROR,
    UINPUT_ERROR,
    INTERFACE_INIT_SUCCESS,
    INTERFACE_INIT_FAILURE,
    UNKNOW
};


class InterfaceManager
{
public:
    InterfaceManager(const char* deviceName); // Must be a valid device name !
    bool CreateEvent(const ButtonsReport &report) const;
    inline InterfaceStatus GetInterfaceStatus() const { return m_Status; };

private:
    InterfaceStatus m_Status;
};

#endif