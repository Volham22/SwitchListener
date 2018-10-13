#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "HidComm.h"

class Controller
{
public:
    Controller(hid_device* device);
    bool DoHandshake();
    bool SetVibration(bool active);
    bool EnableIMU(bool active);
    bool SetIMUSensitivity(const uint8_t &gyroSensi, const uint8_t &acellsensi, const bool &gyroPerf = true, const bool &accelAAFilter = true);
    void DoControllerRoutine();
    ~Controller();
private:
    hid_device* m_Device;
    bool m_IsInitialized;
    HidIO m_Com;

    bool Disconnect();
};

#endif