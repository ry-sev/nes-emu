#include "Cartridge.h"
#include "Mappers/Mapper_000.h"
#include <fstream>

Cartridge::Cartridge(const std::string& filename)
{
    std::ifstream file;
    file.open(filename, std::ios::binary);

    if (file.is_open()) 
    {
        file.read((char*)&m_header, sizeof(m_header));
        if (m_header.mapper_1 & 0x04)
            file.seekg(512, std::ios_base::cur);

        m_mirror = (m_header.mapper_1 & 0x01) ? Mirror::VERTICAL : Mirror::HORIZONTAL;
        m_mapper_id = ((m_header.mapper_2 >> 4) << 4) | (m_header.mapper_1 >> 4);

        m_program_banks = m_header.prg_rom;
        m_program_memory.resize(m_program_banks * 16384); // 16 KB
        file.read((char*)m_program_memory.data(), m_program_memory.size());

        m_character_banks = m_header.chr_rom;
        m_character_memory.resize(m_character_banks * 8192);
        file.read((char*)m_character_memory.data(), m_character_memory.size());

        m_mapper = new Mapper_000(m_program_banks, m_character_banks);

        m_valid_file = true;
        file.close();
    }
}

Cartridge::~Cartridge()
{
    delete m_mapper;
}

bool Cartridge::cpu_read(u16 address, u8& value)
{
    u32 mapped_address = 0;
    if (m_mapper->cpu_map_read(address, mapped_address)) {
        value = m_program_memory[mapped_address];
        return true;
    }
    return false;
}

bool Cartridge::cpu_write(u16 address, u8 value)
{
    u32 mapped_address = 0;
    if (m_mapper->cpu_map_write(address, mapped_address)) {
        m_program_memory[mapped_address] = value;
        return true;
    }
    return false;
}

bool Cartridge::ppu_read(u16 address, u8& value)
{
    u32 mapped_address = 0;
    if (m_mapper->ppu_map_read(address, mapped_address)) {
        value = m_character_memory[mapped_address];
        return true;
    }
    return false;
}

bool Cartridge::ppu_write(u16 address, u8 value)
{
    u32 mapped_address = 0;
    if (m_mapper->ppu_map_write(address, mapped_address)) {
        m_character_memory[mapped_address] = value;
        return true;
    }
    return false;
}
