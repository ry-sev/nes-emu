#include "Bus.h"
#include <cstring>
#include <sstream>

Bus::Bus()
{
    memset(ram, 0x0000, RAM_SIZE);
}

void Bus::load_program_from_string(const char* program)
{
    std::stringstream stream;
    stream << program;
    u16 offset = 0x8000;
    while (!stream.eof()) {
        std::string b;
        stream >> b;
        ram[offset++] = (u8)std::stoul(b, nullptr, 16);
    }
}

u8 Bus::read(u16 address)
{
    if (address >= 0x0000 && address <= RAM_SIZE)
        return ram[address];
    else
        return 0x00;
}

void Bus::write(u16 address, u8 value)
{
    if (address >= 0x0000 && address <= RAM_SIZE)
        ram[address] = value;
    else
        return;
}
