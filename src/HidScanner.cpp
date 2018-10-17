#include "HidScanner.h"

#include <cstdio>
#include <cstring>

static void devicePrint(hid_device_info* device)
{
    	printf("====Device====\n Vendor ID: %04hx\n Product ID: %04hx\n path: %s\n Serial number: %ls\n",
			device->vendor_id,
            device->product_id,
            device->path,
            device->serial_number);
		printf(" Manufacturer Name: %ls\n", device->manufacturer_string);
		printf(" Product Name:      %ls\n", device->product_string);
        printf("==============\n");
}

static const char* TypeToString(const unsigned short &type)
{
    switch(type)
    {
        case JOYCON_L_ID:
            return "JoyCon L";
        
        case JOYCON_R_ID:
            return "JoyCon R";
        
        case PRO_CONTROLLER_ID:
            return "Pro Controller";
        
        case ControllerType::Unknow:
            return "Unknow type";

        default:
            return "Unknow type";
    }
}

HidScanner::HidScanner(ControllerType wantedTyped)
: m_ControllerHandle(nullptr), m_Device(nullptr),
  m_Type(ControllerType::Unknow), m_WantedType(wantedTyped)
{}

bool HidScanner::ScanForControllers()
{
    hid_device_info *devs, *devIter;

    devs = hid_enumerate(NINTENDO_VENDORID, (unsigned short)m_WantedType);
    devIter = devs;

    while(devIter)
    {
        devicePrint(devIter);

        if(devIter->product_id == (unsigned short)m_WantedType
           && !m_ControllerHandle) // Check if the controller isn't already found
        {
            if(InitController(devIter))
                return true;
        }
        else
        {
            break;
        }
                
        devIter = devIter->next;
    }

    hid_free_enumeration(devs);
    return false;
}

bool HidScanner::ScanForAnyController()
{
    hid_device_info *devs, *devIter;

    devs = hid_enumerate(NINTENDO_VENDORID, 0);
    devIter = devs;

    while(devIter)
    {
        devicePrint(devIter);

        switch(devIter->product_id)
        {
            case JOYCON_L_ID:
                if(InitController(devIter))
                    return true;
            
            case JOYCON_R_ID:
                if(InitController(devIter))
                    return true;

            case PRO_CONTROLLER_ID:
                if(InitController(devIter))
                    return true;
        }
                
        devIter = devIter->next;
    }

    hid_free_enumeration(devs);
    return false;
}

bool HidScanner::InitController(hid_device_info* &iter)
{
    printf("A %s has been found", TypeToString(iter->product_id));

    if(!wcscmp(iter->serial_number, BT_SERIAL_NUMBER)) // USB isn't supported
    {
        printf("A USB controller has been found:\n Please use it over Bluetooth\n");
        return false;
    }
    
    m_ControllerHandle = iter;
    m_Device = hid_open_path(iter->path);

    return true;
}