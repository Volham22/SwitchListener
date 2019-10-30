#include "HidComm.h"

#include <stdio.h>
#include <cstring>

#ifdef DEBUG_VERBOSE
static void printData(const HIDBuffer &data)
{
    for(int i = 0; i<data.BufferSize; i++)
        printf("%02x ", data.Buffer[i]);

    printf("\n");
}
#endif

HIDBuffer initBuffer()
{
    HIDBuffer buffer;

    memset(buffer.Buffer, 0, INPUT_BUFFER_SIZE);
    buffer.BufferSize = INPUT_BUFFER_SIZE;

    return buffer;
}

HidIO::HidIO()
: m_Device(nullptr), m_isConnected(false), m_ExchangeCount(0) {}

HidIO::HidIO(hid_device* device)
: m_Device(nullptr), m_isConnected(true), m_ExchangeCount(0)
{
    if(device)
        m_Device = device;
    else
        m_isConnected = false;

    //hid_set_nonblocking(m_Device, 0);
}

HIDBuffer HidIO::SetDisconnected()
{
    m_isConnected = false;
    return { 0, 0 }; 
}

void HidIO::WriteOnDevice(const HIDBuffer &data)
{
    if(m_isConnected)
    {
        int result = hid_write(m_Device, data.Buffer, data.BufferSize);

        if(result < 0) // Couldn't write on device, mark it as disconnected
            m_isConnected = false;
    }
}

void HidIO::SetNonblocking(bool active) const
{
    if(active)
        hid_set_nonblocking(m_Device, 1);
    else
        hid_set_nonblocking(m_Device, 0);
}

HIDBuffer HidIO::ReadOnDevice()
{   
    if(m_isConnected)
    {
        HIDBuffer data = initBuffer();

        int result = hid_read(m_Device, data.Buffer, data.BufferSize);
        if(result > 0)
        {
            data.BufferSize = 24;
            
            #ifdef DEBUG_VERBOSE
            printf("Receive:\n");
            printData(data);
            #endif
            return data;
        }
        else
        {
            #ifdef DEBUG_VERBOSE
            printf("Failed to read on device\n");
            #endif
            return SetDisconnected(); // If reading failed return HIDBuffer initialized with 0
        }
    }
    else
    {
        return { 0, 0 }; // Device is disconnected return null struct too
    }
}

HIDBuffer HidIO::ExchangeOnDevice(HIDBuffer buffer)
{
    //#ifdef DEBUG_VERBOSE
    //printf("Send:\n");
    //printData(buffer);
    //#endif
    
    hid_write(m_Device, buffer.Buffer, buffer.BufferSize);

    int res = hid_read(m_Device, buffer.Buffer, INPUT_BUFFER_SIZE);

    if(res > 0)
    {
        //#ifdef DEBUG_VERBOSE
        //printf("Receive:\n");
        //printData(buffer);
        //#endif
        return buffer; // Return data sent by the controller
    }
    else
    {
        #ifdef DEBUG_VERBOSE
        printf("The controller returned nothing.\n");
        #endif
        return { 0, 0 }; // Return null
    }
}

HIDBuffer HidIO::SendCommandToDevice(HIDBuffer commandBuffer, uint8_t commandID)
{
    unsigned char buf[0x400];
    memset(buf, 0, 0x400);

    buf[0x0] = commandID;
    if(commandBuffer.Buffer != NULL && commandBuffer.BufferSize != 0)
        memcpy(buf + (0x1), commandBuffer.Buffer, commandBuffer.BufferSize);
    
    HIDBuffer buffer;
    buffer.BufferSize = commandBuffer.BufferSize;
    memcpy(buffer.Buffer, buf, INPUT_BUFFER_SIZE);

    return ExchangeOnDevice(buffer);
}

HIDBuffer HidIO::SendSubCommandToDevice(HIDBuffer commandBuffer, uint8_t commandID, uint8_t subCommandID)
{
    unsigned char buf[0x400];
    memset(buf, 0, 0x400);
    
    uint8_t rumble_base[9] = {((uint8_t)++m_ExchangeCount) & 0xF, 0x00, 0x01, 0x40, 0x40, 0x00, 0x01, 0x40, 0x40};
    memcpy(buf, rumble_base, 9);
    
    buf[9] = subCommandID;
    if(commandBuffer.Buffer && commandBuffer.BufferSize != 0)
        memcpy(buf + 10, commandBuffer.Buffer, commandBuffer.BufferSize);

    HIDBuffer send;
    send.BufferSize = commandBuffer.BufferSize + 10;
    memcpy(send.Buffer, buf, INPUT_BUFFER_SIZE);
        
    return SendCommandToDevice(send, commandID);
}