#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "HidComm.h"

class SampleController
{
public:
    virtual bool DoHandshake(const uint8_t &controllerNumber) = 0;
    virtual bool SetVibration(bool active) = 0;
    virtual bool EnableIMU(bool active) = 0;
    virtual bool SetIMUSensitivity(const uint8_t &gyroSensi, const uint8_t &acellsensi, const bool &gyroPerf = true, const bool &accelAAFilter = true) = 0;
    virtual bool SwitchPlayerLedOn(const uint8_t &ledNumber) = 0;
    virtual void DoControllerRoutine() = 0;
    virtual inline hid_device* GetHidDevice() const = 0;
    virtual inline bool IsConnected() const { return false; };
    virtual ~SampleController() {};
protected:
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
    inline hid_device* GetHidDevice() const { return m_Device; };
    inline bool IsConnected() const override { return m_Com.IsConnected(); };
    ~Controller() final;
private:
    uint8_t m_ControllerPosition;
    hid_device* m_Device;
    bool m_IsInitialized;
    HidIO m_Com;
    bool Disconnect();
};

class JoyconController : public SampleController
{
public:
    JoyconController(hid_device* joyconR, hid_device* joyconL);
    bool DoHandshake(const uint8_t &controllerNumber);
    bool SetVibration(bool active);
    bool EnableIMU(bool active);
    bool SetIMUSensitivity(const uint8_t &gyroSensi, const uint8_t &acellsensi, const bool &gyroPerf = true, const bool &accelAAFilter = true);
    bool SwitchPlayerLedOn(const uint8_t &ledNumber);
    void DoControllerRoutine();
    inline hid_device* GetHidDevice() const override { return nullptr; };
    inline hid_device* GetRJoyconHidDevice() const { return m_Device; };
    inline hid_device* GetLJoyconHidDevice() const { return m_JoyConL; };
    inline bool IsConnected() const override { return (m_JoyconLCom.IsConnected() && m_Com.IsConnected()); };
    ~JoyconController() final;
private:
    HidIO m_JoyconLCom;
    hid_device* m_JoyConL;
    uint8_t m_ControllerPosition;
    hid_device* m_Device;
    bool m_IsInitialized;
    HidIO m_Com;
    bool Disconnect();
};

#endif