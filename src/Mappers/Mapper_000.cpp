#include "Mappers/Mapper_000.h"

Mapper_000::Mapper_000(u8 program_banks, u8 character_banks)
    : Mapper(program_banks, character_banks)
{
  
}

bool Mapper_000::cpu_map_read(u16 address, u32 &mapped_address)
{
    if (address >= 0x8000 && address <= 0xFFFF) {
        mapped_address = address & (m_program_banks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}


bool Mapper_000::cpu_map_write(u16 address, u32 &mapped_address)
{
    if (address >= 0x8000 && address <= 0xFFFF) {
        mapped_address = address & (m_program_banks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}


bool Mapper_000::ppu_map_read(u16 address, u32 &mapped_address)
{
    if (address >= 0x0000 && address <= 0x1FFF) {
        mapped_address = address;
        return true;
    }
    return false;
}


bool Mapper_000::ppu_map_write(u16 address, u32 &mapped_address)
{
    return false;
}
