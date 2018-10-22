#include "ConnectionsManager.h"

#include <unistd.h>

#define SCANNING_DELAY 2000 // in ms

ControllerHandler::ControllerHandler()
: m_Connected(false), m_scanner(ControllerType::Any) {}

void ControllerHandler::StartListening()
{
    while(true)
    {
        /* Check for new controllers */
        if(m_scanner.ScanForAnyController())
        {
            hid_device* controllerHandler = m_scanner.GetHidDevice();

            Controller* controller = new Controller(controllerHandler);

            if(controller->DoHandshake())
            {
                m_ConnectedControllers.push_back(controller);
                std::thread* controllerThread = new std::thread(&Controller::DoControllerRoutine, controller);
                m_ThreadList.push_back(controllerThread);
            }
            else
            {
                delete controller;
                printf("Warning: Handshake failed for %s\n", TypeToString(m_scanner.GetControllerType()));
            }
        }

        for(unsigned int iThread = 0; iThread < m_ThreadList.size(); iThread++)
        {
            if(!m_ThreadList[iThread]->joinable())
            {
                /* If thread is stopped, free the memory */
                delete m_ThreadList[iThread];
                m_ThreadList.erase(m_ThreadList.begin() + iThread);
                delete m_ConnectedControllers[iThread];
                m_ConnectedControllers.erase(m_ConnectedControllers.begin() + iThread);
            }
        }

        usleep(2000);
    }
}

ControllerHandler::~ControllerHandler()
{
    printf("Waiting running threads, controllers will be disconnected ...\n");

    for(unsigned int iThread = 0; iThread < m_ThreadList.size(); iThread++)
    {
        delete m_ConnectedControllers[iThread];
        m_ThreadList[iThread]->join();
        delete m_ThreadList[iThread];
    }
}