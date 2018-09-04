#ifndef HID_COMM_H
#define HID_COMM_H

#include <hidapi/hidapi.h>
#include <cstdint>

#define INPUT_BUFFER_SIZE 0x400
typedef unsigned char uchar;

struct HIDBuffer
{
    int BufferSize;
    uchar Buffer[INPUT_BUFFER_SIZE];
};

struct ControllerCommand
{
    uint8_t* Data;
    int CommandID;
    int DataSize;
};

class HidIO
{
public:
    HidIO(hid_device* device);
    void WriteOnDevice(const HIDBuffer &data);
    HIDBuffer SendCommandToDevice(ControllerCommand &command);
    HIDBuffer SendSubCommandToDevice(ControllerCommand &command, int subCommand);
    HIDBuffer ReadOnDevice();
    HIDBuffer ExchangeOnDevice(const HIDBuffer &data);
    inline bool IsConnected() const { return m_isConnected; }
    inline hid_device* GetDevice() const { return m_Device; }
private:
    hid_device* m_Device;
    bool m_isConnected;
    uint8_t m_ExchangeCount;

    inline HIDBuffer SetDisconnected();
};

#endif