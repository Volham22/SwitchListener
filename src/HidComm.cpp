#include "HidComm.h"

#include <stdio.h>
#include <cstring>

static void PrintData(const HIDBuffer &data)
{
    for(int i = 0; i<data.BufferSize; i++)
        printf("%02x", data.Buffer[i]);

    printf("\n");
}

static HIDBuffer initBuffer()
{
    HIDBuffer buffer;

    memset(buffer.Buffer, 0, INPUT_BUFFER_SIZE);
    buffer.BufferSize = INPUT_BUFFER_SIZE;

    return buffer;
}

HidIO::HidIO(hid_device* device)
: m_Device(nullptr), m_isConnected(true)
{
    if(device)
        m_Device = device;
    else
        m_isConnected = false;

    //hid_set_nonblocking(m_Device, 1);
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

HIDBuffer HidIO::ReadOnDevice()
{   
    if(m_isConnected)
    {
        HIDBuffer data = initBuffer();

        int result = hid_read(m_Device, data.Buffer, data.BufferSize);
        if(result > 0)
            return data;
        else
            return SetDisconnected(); // If reading failed return HIDBuffer initialized with 0
    }
    else
    {
        return { 0, 0 }; // Device is disconnected return null struct too
    }
}

HIDBuffer HidIO::ExchangeOnDevice(const HIDBuffer &data)
{
    if(m_isConnected)
    {
        int res = hid_write(m_Device, data.Buffer, data.BufferSize);

        if(res < 0)
            return SetDisconnected();

        if(m_isConnected)
        {
            HIDBuffer buffer = initBuffer();

            int result = hid_read(m_Device, buffer.Buffer, buffer.BufferSize);

            if(!result > 0)
                return SetDisconnected();
            else
                return buffer;
        }
    }
    else
    {
        return { 0, 0 }; // Same here
    }
}