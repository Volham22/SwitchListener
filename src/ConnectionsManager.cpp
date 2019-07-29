#include "ConnectionsManager.h"

#ifdef _WIN32
#include <Windows.h>
#define SLEEP(x) Sleep(x)
#else
#include <unistd.h>
#define SLEEP(x) usleep(x)
#endif

#include <cwchar>

#define SCANNING_DELAY 2000 // in ms

inline static bool isJoycon(ControllerType type)
{
    return (type == ControllerType::JoyConLeft || type == ControllerType::JoyConRight);
}

ControllerHandler::ControllerHandler(bool mergeJoycons)
: m_Connected(0), m_scanner(ControllerType::Any), m_MergeJoycons(mergeJoycons),
  m_WaitingJoycon(false), m_WaitingJoyconDevice(nullptr), m_WaitingJoyconType(ControllerType::Unknow) {}

void ControllerHandler::StartListening()
{
    /* Creating Listening Thread */
    #ifdef _WIN32
    mingw_stdthread::thread listeningThread(&ControllerHandler::ListenToControllers, this);
    #else
    std::thread listeningThread(&ControllerHandler::ListenToControllers, this);
    #endif

    while(true)
    {
        /* Check for new controllers */
        if(m_scanner.ScanForAnyController())
        {
            hid_device* controllerHandler = m_scanner.GetHidDevice();
            Controller* controller = new Controller(controllerHandler);

            if((!m_WaitingJoycon) && isJoycon(m_scanner.GetControllerType()) && m_MergeJoycons)
            {
                printf("Joycon found waiting a second one\n");

                m_WaitingJoycon = true;
                m_WaitingJoyconType = m_scanner.GetControllerType();
                m_WaitingJoyconDevice = controllerHandler;

                delete controller;
            }
            else if(m_WaitingJoycon && isJoycon(m_scanner.GetControllerType())
                    && m_scanner.GetControllerType() != m_WaitingJoyconType && m_MergeJoycons)
            {
                printf("Two differents Joycons connected reconized as one controller\n");

                if(m_WaitingJoyconType == ControllerType::JoyConRight)
                {
                    m_Connected++;

                    JoyconController* controller = new JoyconController(m_WaitingJoyconDevice, controllerHandler);

                    if(controller->DoHandshake(m_Connected))
                    {
                        printf("Merged Joycons connected\n");
                        m_ConnectedControllers.push_back(controller);
                    }
                    else
                    {
                        m_Connected--;
                        delete controller;
                        printf("Error: Handshake failed for merged %s\n", TypeToString(m_scanner.GetControllerType()));
                    }
                }
                else
                {
                    JoyconController* controller = new JoyconController(controllerHandler, m_WaitingJoyconDevice);
                    
                    m_Connected++;

                    if(controller->DoHandshake(m_Connected))
                    {
                        printf("Merged Joycons connected\n");
                        m_ConnectedControllers.push_back(controller);
                    }
                    else
                    {
                        m_Connected--;
                        delete controller;
                        printf("Error: Handshake failed for merged %s\n", TypeToString(m_scanner.GetControllerType()));
                    }
                }
            }
                        
            if(!isJoycon(m_scanner.GetControllerType()) || (!m_MergeJoycons))
            {
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
                    printf("Error: Handshake failed for %s aborting listening ...\n", TypeToString(m_scanner.GetControllerType()));
                }
            }
        }

        SLEEP(2000);
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

        SLEEP(25);
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