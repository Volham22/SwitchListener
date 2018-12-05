#include "Interface.h"

#include <linux/input.h>
#include <linux/uinput.h>
#include <libudev.h>

InterfaceManager::InterfaceManager(const char* deviceName)
: m_Status(InterfaceStatus::UNKNOWN)
{
    // TODO: Initilisation
}

bool InterfaceManager::CreateEvent(const ButtonsReport &report) const
{
    return false;
}