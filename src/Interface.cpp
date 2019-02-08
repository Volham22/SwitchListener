#include "Interface.h"

#include <cstdio>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>

#define JOYCON_L_ID (0x2006)
#define JOYCON_R_ID (0x2007)
#define PRO_CONTROLLER_ID (0x2009)
#define NINTENDO_VENDORID (0x057E)

static uint16_t getNitendoProductID(const char* productName)
{
    if(strcmp(PRO_CONTROLLER_DEVICENAME, productName))
        return PRO_CONTROLLER_ID;
    else if (strcmp(JOYCON_L_DEVICENAME, productName))
        return JOYCON_L_ID;
    else if(strcmp(JOYCON_R_DEVICENAME, productName))
        return JOYCON_R_ID;
    else return 0x1;
}

static int getButtonId(Buttons button)
{
    switch(button)
    {
        case Buttons::Left:
            return BTN_DPAD_LEFT;
        
        case Buttons::Right:
            return BTN_DPAD_RIGHT;
        
        case Buttons::Up:
            return BTN_DPAD_UP;
        
        case Buttons::Down:
            return BTN_DPAD_DOWN;
        
        case Buttons::L:
            return BTN_TL;
        
        case Buttons::ZL:
            return BTN_TL2;

        case Buttons::R:
            return BTN_TR;
        
        case Buttons::ZR:
            return BTN_TR2;
        
        case Buttons::X:
            return BTN_WEST;

        case Buttons::B:
            return BTN_SOUTH;
        
        case Buttons::Y:
            return BTN_NORTH;
        
        case Buttons::A:
            return BTN_EAST;

        case Buttons::Plus:
            return BTN_START;
        
        case Buttons::Minus:
            return BTN_SELECT;
        
        case Buttons::LStick:
            return BTN_THUMBL;

        case Buttons::RStick:
            return BTN_THUMBR;
        
        case Buttons::Home:
            return BTN_MODE;
        
        default:
            return -1; // Error
    }
}

InterfaceManager::InterfaceManager(const char* deviceName)
: m_Status(InterfaceStatus::UNKNOWN), m_uDev(nullptr), m_uInput(nullptr), m_fd(0)
{
    // Create device interface
    m_uDev = udev_new();

    if(m_uDev == NULL)
    {
        printf("Unable to init udev for %s\n", deviceName);
        m_Status = InterfaceStatus::UDVEV_ERROR;
        return;
    }

    m_uInput = udev_device_new_from_subsystem_sysname(m_uDev, "misc", "uinput");

    if(m_uInput == NULL)
    {
        printf("Unable to create udev device for %s\n", deviceName);
        m_Status = InterfaceStatus::UINPUT_ERROR;
        return;
    }

    // Get device path
    const char* uinputPath = udev_device_get_devnode(m_uInput);

    if(uinputPath == NULL)
    {
        printf("Unable to get uinput Path for %s\n", deviceName);
        m_Status = InterfaceStatus::INTERFACE_INIT_FAILURE;
        return;
    }

    m_fd = open(uinputPath, O_WRONLY | O_NONBLOCK);

    // ioctl buttons
    ioctl(m_fd, UI_SET_KEYBIT, EV_KEY);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_NORTH);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_SOUTH);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_EAST);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_WEST);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_START);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_SELECT);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_MODE);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_Z);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_DPAD_UP);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_DPAD_DOWN);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_DPAD_LEFT);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_DPAD_RIGHT);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_THUMBL);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_THUMBR);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_TL);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_TL2);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_TR);
    ioctl(m_fd, UI_SET_KEYBIT, BTN_TR2);

    // ioctl sticks
    ioctl(m_fd, UI_SET_EVBIT, EV_ABS);
    ioctl(m_fd, UI_SET_ABSBIT, ABS_X);
    ioctl(m_fd, UI_SET_ABSBIT, ABS_Y);
    ioctl(m_fd, UI_SET_ABSBIT, ABS_RX);
    ioctl(m_fd, UI_SET_ABSBIT, ABS_RY);

    // Set name and fake informations
    memset(&m_uDevice, 0, sizeof(m_uDevice));
    snprintf(m_uDevice.name, UINPUT_MAX_NAME_SIZE, deviceName);
    m_uDevice.id.bustype = BUS_USB;
    m_uDevice.id.vendor = NINTENDO_VENDORID;
    m_uDevice.id.product = getNitendoProductID(deviceName);

    // TODO:
    for(int i = 0; i<= 2; i++)
    {
        ioctl(m_fd, UI_SET_ABSBIT, i);
        m_uDevice.absmin[i] = 32;
        m_uDevice.absmax[i] = 255 - 32;
    }

    write(m_fd, &m_uDevice, sizeof(m_uDevice));
    ioctl(m_fd, UI_DEV_CREATE);
    
    m_Status = InterfaceStatus::INTERFACE_INIT_SUCCESS;
}

bool InterfaceManager::CreateEvent(const ButtonsReport &report) const
{
    input_event event;

    // Writing buttons
    for(int i = 0; i < 22; i++)
    {
        if(report.ButtonsStates[i])
        {
            event.type  = EV_KEY;
            event.code  = getButtonId((Buttons) i);
            event.value = 1;

            #ifdef DEBUG
            printf("Writing Event for Key %d\n", i);
            #endif

            // Writing Event
            if(write(m_fd, &event, sizeof(input_event)) == -1)
                return false;
        }
    }

    // Writing Left Pad X axis
    memset(&event, 0, sizeof(event));
    event.type  = EV_ABS;
    event.code  = ABS_X;
    event.value = report.StickLeft.Horizontal;

    #ifdef DEBUG
        printf("Writing Left Pad X axis with value : %d\n", report.StickLeft.Horizontal);
    #endif

    if(write(m_fd, &event, sizeof(input_event)) == -1)
        return false;

    // Writing Left Pad Y axis
    memset(&event, 0, sizeof(input_event));
    event.type  = EV_ABS;
    event.type  = ABS_Y;
    event.value = report.StickLeft.Vertical;

    #ifdef DEBUG
        printf("Writing Left Pad Y axis with value : %d\n", report.StickLeft.Vertical);
    #endif

    if(write(m_fd, &event, sizeof(input_event)) == -1)
        return false;

    // Writing Right Pad X axis
    memset(&event, 0, sizeof(input_event));
    event.type  = EV_ABS;
    event.code  = ABS_RX;
    event.value = report.StickRight.Horizontal;

    #ifdef DEBUG
        printf("Writing Right Pad X axis with value : %d\n", report.StickRight.Horizontal);
    #endif

    if(write(m_fd, &event, sizeof(input_event)) == -1)
        return false;
    
    // Writing Right Pad Y axis
    memset(&event, 0, sizeof(input_event));
    event.type = EV_ABS;
    event.code = ABS_RY;
    event.value = report.StickRight.Vertical;

    #ifdef DEBUG
        printf("Writing Right Pad Y axis with value : %d\n", report.StickRight.Vertical);
    #endif

    if(write(m_fd, &event, sizeof(input_event)) == -1)
        return false;

    return true;
}