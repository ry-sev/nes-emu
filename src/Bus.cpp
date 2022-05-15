#include "Bus.h"
#include "PPU.h"
#include "Cartridge.h"
#include <cstring>
#include <sstream>

Bus::Bus()
{
    cpu_ram.fill(0x00);
}

void Bus::load_program_from_string(const char* program)
{
    std::stringstream stream;
    stream << program;
    u16 offset = 0x8000;
    while (!stream.eof()) {
        std::string b;
        stream >> b;
        cpu_ram[offset++] = (u8)std::stoul(b, nullptr, 16);
    }
    cpu_write(0xFFFC, 0x00);
    cpu_write(0xFFFD, 0x80);
}

u8 Bus::cpu_read(u16 address)
{
    u8 data = 0x00;

    if (m_cartridge->cpu_read(address, data)) {}

    else if (address <= 0x1FFF)
        data = cpu_ram[address & 0x07FF];

    else if (address >= 0x2000 && address <= 0x3FFF)
        data = m_ppu->cpu_read(address & 0x0007);

    else if (address >= 0x4016 && address <= 0x4017) {
        data = (m_controller_state[address & 0x0001] & 0x80) > 0;
        m_controller_state[address & 0x0001] <<= 1;
    }

    return data;
}

void Bus::cpu_write(u16 address, u8 value)
{
    if (m_cartridge->cpu_write(address, value)) {

    }
    else if (address <= 0x1FFF)
        cpu_ram[address & 0x07FF] = value;

    else if (address >= 0x2000 && address <= 0x3FFF)
        m_ppu->cpu_write(address & 0x0007, value);

    else if (address == 0x4014) {
        m_dma_page = value;
        m_dma_address = 0x00;
        m_dma_transfer = true;
    }

    else if (address >= 0x4016 && address <= 0x4017)
        m_controller_state[address & 0x0001] = m_controllers[address & 0x0001];
}

void Bus::insert_cartridge(const std::shared_ptr<Cartridge> cartridge)
{
    m_cartridge = cartridge;
}

bool Bus::handle_dma_transfer(u32 clock_count)
{
    if (!m_dma_transfer) return false;

    if (m_dma_dummy && clock_count % 2 ==1) {
        m_dma_dummy = false;
    }
    else {
        if (clock_count % 2 == 0)
            m_dma_data = cpu_read(m_dma_page << 8 | m_dma_address);
        else {
            m_ppu->oam_ptr[m_dma_address] = m_dma_data;
            m_dma_address++;
            if (m_dma_address == 0x00) {
                m_dma_transfer = false;
                m_dma_dummy = true;
            }
        }
    }
    return true;
}

void Bus::write_controller_state(u32 controller_id, u8 data)
{
    m_controllers[controller_id] |= data;
}

void Bus::clear_controller_state(u32 controller_id){
    m_controllers[controller_id] = 0x00;
}

void Bus::reset()
{
    m_dma_page = 0x00;
    m_dma_address = 0x00;
    m_dma_dummy = true;
    m_dma_transfer = false;
}
