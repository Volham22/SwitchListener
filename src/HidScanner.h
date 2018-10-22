#ifndef HID_SCANNER_H
#define HID_SCANNER_H

#include <hidapi/hidapi.h>
#include <vector>

#define JOYCON_L_ID (0x2006)
#define JOYCON_R_ID (0x2007)
#define PRO_CONTROLLER_ID (0x2009)
#define NINTENDO_VENDORID (0x057E)
#define BT_SERIAL_NUMBER L"000000000001"

enum ControllerType { ProController = PRO_CONTROLLER_ID,
                      JoyConRight   = JOYCON_R_ID,
                      JoyConLeft    = JOYCON_L_ID,
                      Any           = 1,
                      Unknow        = 0 };

const char* TypeToString(const unsigned short &type);

class HidScanner
{
public:
    HidScanner(ControllerType wantedTyped = ControllerType::Any);
    bool ScanForControllers(); // Return true if a controller is connected and has a valid hid interface
    bool ScanForAnyController();
    inline hid_device_info* GetControllerHandle() const { return m_ControllerHandle; };
    inline hid_device* GetHidDevice() const { return m_Device; }
    inline ControllerType GetControllerType() const { return m_Type; }
private:
    hid_device_info* m_ControllerHandle;
    hid_device* m_Device;
    ControllerType m_Type;
    ControllerType m_WantedType;
    std::vector<wchar_t*> m_OpenedDevices;

    bool InitController(hid_device_info* &iter);
};

#endif