#pragma once

class Mapper {

public:
    Mapper(u8 program_banks, u8 character_banks);
    virtual ~Mapper() = default;

    virtual bool cpu_map_read(u16 address, u32 &mapped_address) = 0;
    virtual bool cpu_map_write(u16 address, u32 &mapped_address) = 0;
    virtual bool ppu_map_read(u16 address, u32 &mapped_address) = 0;
    virtual bool ppu_map_write(u16 address, u32 &mapped_address) = 0;

protected:
    u8 m_program_banks;
    u8 m_character_banks;
};
