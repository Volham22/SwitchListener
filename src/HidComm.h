#ifndef HID_COMM_H
#define HID_COMM_H

#include <hidapi/hidapi.h>

#define INPUT_BUFFER_SIZE 0x400
typedef unsigned char uchar;

struct HIDBuffer
{
    int BufferSize;
    uchar Buffer[INPUT_BUFFER_SIZE];
};

class HidIO
{
public:
    HidIO(hid_device* device);
    void WriteOnDevice(const HIDBuffer &data);
    HIDBuffer ReadOnDevice();
    HIDBuffer ExchangeOnDevice(const HIDBuffer &data);
    inline bool IsConnected() const { return m_isConnected; }
    inline hid_device* GetDevice() const { return m_Device; }
private:
    hid_device* m_Device;
    bool m_isConnected;

    inline HIDBuffer SetDisconnected();
};

#endif