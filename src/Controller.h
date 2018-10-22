#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "HidComm.h"

class Controller
{
public:
    Controller(hid_device* device);
    bool DoHandshake(const uint8_t &controllerNumber);
    bool SetVibration(bool active);
    bool EnableIMU(bool active);
    bool SetIMUSensitivity(const uint8_t &gyroSensi, const uint8_t &acellsensi, const bool &gyroPerf = true, const bool &accelAAFilter = true);
    bool SwitchPlayerLedOn(const uint8_t &ledNumber);
    void DoControllerRoutine();
    inline hid_device* GetHidDevice() const { return m_Device; };
    ~Controller();
private:
    uint8_t m_ControllerPosition;
    hid_device* m_Device;
    bool m_IsInitialized;
    HidIO m_Com;

    bool Disconnect();
};

#endif