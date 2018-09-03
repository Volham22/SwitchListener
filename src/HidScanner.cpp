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

HidScanner::HidScanner(ControllerType wantedTyped)
: m_ControllerHandle(nullptr), m_Device(nullptr),
  m_Type(ControllerType::Unknow), m_WantedType(wantedTyped)
{
}

bool HidScanner::ScanForControllers()
{
    hid_device_info *devs, *devIter;

    devs = hid_enumerate(NINTENDO_VENDORID, (unsigned short)m_WantedType);
    devIter = devs;

    while(devIter)
    {
        devicePrint(devIter);

        if(!devIter->product_id != (unsigned short)m_WantedType
           || !m_ControllerHandle) // Check if the controller isn't already found
        {
            m_ControllerHandle = devIter;
            m_Device = hid_open_path(devIter->path);

            hid_free_enumeration(devs);
            return true;
        }
        else
        {
            break;
        }
                
        if(!wcscmp(devIter->serial_number, BT_SERIAL_NUMBER)) // USB isn't supported
            printf("A USB controller has been found:\n Please use it over Bluetooth");

        devIter->next;
    }

    hid_free_enumeration(devs);
    return false;
}