#ifndef HID_COMM_H
#define HID_COMM_H

#ifdef _WIN32
#include "../libs/hidapi.h"
#else
#include <hidapi/hidapi.h>
#endif

#include <cstdint>

#define INPUT_BUFFER_SIZE (0x400)
typedef unsigned char uchar;

struct HIDBuffer {
    int BufferSize;
    uchar Buffer[INPUT_BUFFER_SIZE];
};

HIDBuffer initBuffer();

class HidIO
{
public:
    HidIO();
    HidIO(hid_device* device);
    void WriteOnDevice(const HIDBuffer &data);
    void SetNonblocking(bool state) const;
    HIDBuffer SendCommandToDevice(HIDBuffer commandBuffer, uint8_t commandID);
    HIDBuffer SendSubCommandToDevice(HIDBuffer commandBuffer, uint8_t commandID, uint8_t subCommandID);
    HIDBuffer ReadOnDevice();
    HIDBuffer ExchangeOnDevice(HIDBuffer buffer);
    inline bool IsConnected() const { return m_isConnected; }
    inline hid_device* GetDevice() const { return m_Device; }
private:
    hid_device* m_Device;
    bool m_isConnected;
    uint8_t m_ExchangeCount;

    inline HIDBuffer SetDisconnected();
};

#endif