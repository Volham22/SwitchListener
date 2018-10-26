#ifndef CONNECTIONSMANAGER_H
#define CONNECTIONSMANAGER_H

#include <thread>
#include <vector>

#include "HidScanner.h"
#include "Controller.h"

class ConnectedDevice
{
public:
    ConnectedDevice();
    virtual void DoControllerRoutine() = 0;
    virtual void KillConnection() = 0;
    inline bool IsConnected() const { return m_IsConnected; };
    ~ConnectedDevice();
private:
    Controller m_controller;
    bool m_IsConnected;
};

class ControllerHandler
{
public:
    ControllerHandler();
    void StartListening();
    inline int GetConnectedControllers() const { return m_Connected; };
    ~ControllerHandler();
private:
    void ListenToControllers();
    uint8_t m_Connected;
    HidScanner m_scanner;
    std::vector<Controller*> m_ConnectedControllers;
};

#endif