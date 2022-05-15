#include "NES.h"

NES::NES()
{
    m_bus = std::make_shared<Bus>();
    m_cpu = std::make_shared<CPU6502>();
    m_ppu = std::make_shared<PPU>();

    m_cpu->connect_to_bus(m_bus);
    m_bus->connect_to_ppu(m_ppu);
}

void NES::insert_cartridge(const std::shared_ptr<Cartridge> cartridge)
{
    m_bus->insert_cartridge(cartridge);
    m_ppu->insert_cartridge(cartridge);
}

void NES::reset()
{
    m_cpu->reset();
    m_ppu->reset();
    m_bus->reset();
    m_system_clock_count = 0;
}

void NES::clock()
{
    m_ppu->clock();

    if (m_system_clock_count % 3 == 0) {
        if (m_bus->handle_dma_transfer(m_system_clock_count)) {}
        else
            m_cpu->clock();
    }

    if (m_ppu->nmi()) {
        m_ppu->set_nmi(false);
        m_cpu->nmi();
    }

    m_system_clock_count++;
}

void NES::write_controller_state(u32 controller_id, u8 data)
{
    if (controller_id > NUMBER_OF_CONTROLLERS) return;
    m_bus->write_controller_state(controller_id, data);
}

void NES::clear_controller_state(u32 controller_id){
    if (controller_id > NUMBER_OF_CONTROLLERS) return;
    m_bus->clear_controller_state(controller_id);
}
