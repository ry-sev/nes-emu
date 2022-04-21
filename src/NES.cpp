#include "NES.h"
#include "Cartridge.h"

NES::NES()
{
    m_bus = Bus();
    m_cpu = CPU6502();
    m_ppu = PPU();

    m_cpu.connect_to_bus(&m_bus);
}

void NES::insert_cartridge(Cartridge* cartridge)
{
    m_cartridge = cartridge;
    m_bus.insert_cartridge(cartridge);
    m_ppu.insert_cartridge(cartridge);
}

void NES::reset()
{
    m_cpu.reset();
    m_system_clock_count = 0;
}

void NES::clock()
{
    m_ppu.clock();

    if (m_system_clock_count % 3 == 0)
        m_cpu.clock();

    m_system_clock_count++;
}
