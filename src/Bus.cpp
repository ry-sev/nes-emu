#include <cstring>
#include "Bus.h"

Bus::Bus()
{
    memset(ram, 0x0000, RAM_SIZE);
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
