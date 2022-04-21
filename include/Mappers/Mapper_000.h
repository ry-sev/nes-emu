#pragma once
#include "Mapper.h"

class Mapper_000 : public Mapper {

public:
    Mapper_000(u8 program_banks, u8 character_banks);
    ~Mapper_000() = default;

    bool cpu_map_read(u16 address, u32 &mapped_address) override;
    bool cpu_map_write(u16 address, u32 &mapped_address) override;
    bool ppu_map_read(u16 address, u32 &mapped_address) override;
    bool ppu_map_write(u16 address, u32 &mapped_address) override;

private:

};
