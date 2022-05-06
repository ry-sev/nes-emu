#pragma once
#include <vector>

struct iNESHeader {
    char magic[4];
    u8 prg_rom;
    u8 chr_rom;
    u8 mapper_1;
    u8 mapper_2;
    u8 prg_ram_size;
    u8 tv_system_1;
    u8 tv_system_2;
    char unused[5];
};

enum class Mirror {
    HORIZONTAL,
    VERTICAL
};

class Mapper;

class Cartridge {

public:
    Cartridge(const std::string& filename);
    ~Cartridge();

    iNESHeader header() const { return m_header; }
    Mirror mirror() const { return m_mirror; }
    u8 mapper_id() const { return m_mapper_id; }
    bool is_valid() { return m_valid_file; }
    const std::vector<u8>& program_rom() const { return m_program_memory; }

    bool cpu_read(u16 address, u8& value);
    bool cpu_write(u16 address, u8 value);
    bool ppu_read(u16 address, u8& value);
    bool ppu_write(u16 address, u8 value);

private:
    Mirror m_mirror;
    Mapper* m_mapper = nullptr;
    iNESHeader m_header;
    u8 m_program_banks;
    u8 m_character_banks;
    u8 m_mapper_id;
    std::vector<u8> m_program_memory;
    std::vector<u8> m_character_memory;
    bool m_valid_file = false;
};
