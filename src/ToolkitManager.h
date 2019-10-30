#ifndef TOOLKIT_MANAGER_H
#define TOOLKIT_MANAGER_H

#include <map>

#include "HidScanner.h"

class ToolkitManager
{
public:
    ToolkitManager();
    void ListAvailableControllers();
    void DoSPIDump(hid_device* device, ControllerType type);
    void PrintAnswer();
private:
    HidScanner m_Scanner;
    std::map<hid_device*, ControllerType> m_Devices;

    hid_device* GetDeviceFromSelection(int selection);
};

#endif