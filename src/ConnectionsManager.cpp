#include "ConnectionsManager.h"

#include <unistd.h>

#define SCANNING_DELAY 2000 // in ms

ControllerHandler::ControllerHandler()
: m_Connected(0), m_scanner(ControllerType::Any) {}

void ControllerHandler::StartListening()
{
    /* Creating Listening Thread */
    std::thread listeningThread(&ControllerHandler::ListenToControllers, this);

    while(true)
    {
        /* Check for new controllers */
        if(m_scanner.ScanForAnyController())
        {
            hid_device* controllerHandler = m_scanner.GetHidDevice();
            Controller* controller = new Controller(controllerHandler);
            m_Connected++;

            if(controller->DoHandshake(m_Connected))
            {
                printf("New %s found !\n", TypeToString(m_scanner.GetControllerType()));
                m_ConnectedControllers.push_back(controller);
            }
            else
            {
                delete controller;
                m_Connected--;
                printf("Warning: Handshake failed for %s\n", TypeToString(m_scanner.GetControllerType()));
            }
        }

        usleep(2000);
    }
}

void ControllerHandler::ListenToControllers()
{
    while(true)
    {
        for(unsigned int iController = 0; iController < m_ConnectedControllers.size(); iController++)
        {
            m_ConnectedControllers[iController]->DoControllerRoutine();

            if(!m_ConnectedControllers[iController]->IsConnected())
            {
                delete m_ConnectedControllers[iController];
                m_ConnectedControllers.erase(m_ConnectedControllers.begin() + iController);
                m_Connected--;
            }
        }

        usleep(25);
    }
}

ControllerHandler::~ControllerHandler()
{
    printf("Waiting running threads, controllers will be disconnected ...\n");

    for(unsigned int iController = 0; iController < m_ConnectedControllers.size(); iController++)
    {
        delete m_ConnectedControllers[iController];
        m_ConnectedControllers.erase(m_ConnectedControllers.begin() + iController);
    }
}