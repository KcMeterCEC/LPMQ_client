#include "rb.h"
#include <QDebug>

Rb::Rb(std::uint32_t size)
: write_index(0), read_index(0), empty_size(size)
{
    buf.resize(size);
}

Rb::~Rb()
{

}

bool Rb::write(const char *wbuf, std::uint32_t size)
{
    if((wbuf == nullptr) || (!size))
    {
        return false;
    }

    if(empty_size < size)
    {
        qDebug() << "ring buffer hasn't enough space!";

        return false;
    }

    empty_size -= size;

    uint32_t first_cp = size;
    uint32_t second_cp = 0;
    uint32_t start_index = write_index;

    if((write_index + size) >= buf.size())
    {
        first_cp = buf.size() - write_index;
        second_cp = size - first_cp;

        write_index = second_cp;
    }
    else
    {
        write_index += size;
    }

    for(uint32_t i = 0; i < first_cp; ++i)
    {
        buf[start_index + i] = wbuf[i];
    }

    for(uint32_t i = 0; i < second_cp; ++i)
    {
        buf[i] = wbuf[first_cp + i];
    }

    return true;
}
bool Rb::read(char *rbuf, std::uint32_t size)
{
    if(rbuf == nullptr)
    {
        return false;
    }
    if(!size)
    {
        return true;
    }

    uint32_t full_size = buf.size() - empty_size;

    if(full_size < size)
    {
        qDebug() << "ring buffer hasn't enough data!";
        return false;
    }

    empty_size += size;

    uint32_t first_cp = size;
    uint32_t second_cp = 0;
    uint32_t start_index = read_index;

    if((read_index + size) >= buf.size())
    {
        first_cp = buf.size() - read_index;
        second_cp = size - first_cp;
        read_index = second_cp;
    }
    else
    {
        read_index += size;
    }
    for(uint32_t i = 0; i < first_cp; ++i)
    {
        rbuf[i] = buf[start_index + i];
    }

    for(uint32_t i = 0; i < second_cp; ++i)
    {
        rbuf[first_cp + i] = buf[i];
    }

    return true;
}
