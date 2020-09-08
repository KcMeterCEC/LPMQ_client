#ifndef RB_H
#define RB_H
#include <cstdint>
#include <vector>

class Rb
{
public:
    Rb(std::uint32_t size = 1024);

    ~Rb();

    bool write(const char * wbuf, std::uint32_t size);
    bool read(char *rbuf, std::uint32_t size);
private:
    std::uint32_t write_index = 0;
    std::uint32_t read_index = 0;
    std::uint32_t empty_size = 0;
    std::vector<std::uint8_t> buf;
};
#endif
