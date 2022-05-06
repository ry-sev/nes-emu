#include "NES.h"

NES::NES()
{
    m_bus = std::make_shared<Bus>();
    m_cpu = std::make_shared<CPU6502>();
    m_ppu = std::make_shared<PPU>();

    m_cpu->connect_to_bus(m_bus);
    m_bus->connect_to_ppu(m_ppu);
}

void NES::insert_cartridge(std::shared_ptr<Cartridge> cartridge)
{
    m_bus->insert_cartridge(cartridge);
    m_ppu->insert_cartridge(cartridge);
}

void NES::reset()
{
    m_cpu->reset();
    m_system_clock_count = 0;
}

void NES::clock()
{
    m_ppu->clock();

    if (m_system_clock_count % 3 == 0)
        m_cpu->clock();

    m_system_clock_count++;
}
