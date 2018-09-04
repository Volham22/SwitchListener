#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "HidComm.h"

class Controller
{
public:
    Controller(hid_device* device);
    bool DoHandshake();
private:
    hid_device* m_Device;
    bool m_IsInitialized;
    HidIO m_Com;
};

#endif