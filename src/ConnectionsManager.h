#ifndef CONNECTIONSMANAGER_H
#define CONNECTIONSMANAGER_H

#ifdef _WIN32
#define _WIN32_WINNT 0x0501 // To use mingw threads on Windows xp and higher
#include "../libs/mingw.thread.h"
#else
#include <thread>
#endif

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