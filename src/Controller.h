#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "HidComm.h"

class SampleController
{
public:
    SampleController(hid_device* device);
    virtual bool DoHandshake(const uint8_t &controllerNumber) = 0;
    virtual bool SetVibration(bool active) = 0;
    virtual bool EnableIMU(bool active) = 0;
    virtual bool SetIMUSensitivity(const uint8_t &gyroSensi, const uint8_t &acellsensi, const bool &gyroPerf = true, const bool &accelAAFilter = true) = 0;
    virtual bool SwitchPlayerLedOn(const uint8_t &ledNumber) = 0;
    virtual void DoControllerRoutine() = 0;
    inline hid_device* GetHidDevice() const { return m_Device; };
    inline bool IsConnected() const { return m_Com.IsConnected(); };
    virtual ~SampleController() = 0;
protected:
    uint8_t m_ControllerPosition;
    hid_device* m_Device;
    bool m_IsInitialized;
    HidIO m_Com;

    virtual bool Disconnect() = 0;
};

class Controller : public SampleController
{
public:
    Controller(hid_device* device);
    bool DoHandshake(const uint8_t &controllerNumber);
    bool SetVibration(bool active);
    bool EnableIMU(bool active);
    bool SetIMUSensitivity(const uint8_t &gyroSensi, const uint8_t &acellsensi, const bool &gyroPerf = true, const bool &accelAAFilter = true);
    bool SwitchPlayerLedOn(const uint8_t &ledNumber);
    void DoControllerRoutine();
    ~Controller();
private:
    bool Disconnect();
};

#endif