#ifndef CONNECTIONSMANAGER_H
#define CONNECTIONSMANAGER_H

#include <thread>
#include <vector>

#include "HidScanner.h"
#include "Controller.h"


class ControllerHandler
{
public:
    ControllerHandler(bool m_MergeJoycons = false);
    void StartListening();
    inline int GetConnectedControllers() const { return m_Connected; };
    ~ControllerHandler();
private:
    void ListenToControllers();

    uint8_t m_Connected;
    HidScanner m_scanner;
    std::vector<SampleController*> m_ConnectedControllers;
    bool m_MergeJoycons;
    bool m_WaitingJoycon;
    hid_device* m_WaitingJoyconDevice;
    ControllerType m_WaitingJoyconType;
};

#endif