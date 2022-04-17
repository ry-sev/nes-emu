#include "Bus.h"
#include "CPU.h"
#include "WindowManager.h"
#include "MenuWidget.h"
#include "RamWidget.h"
#include "StatusWidget.h"

int main() 
{
    auto bus = Bus();
    auto cpu = CPU6502();
    bus.connect_to_cpu(&cpu);
    cpu.connect_to_bus(&bus);

    bus.load_program_from_string("A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA");

    cpu.reset();

    while (!(cpu.registers().a == 0x1e))
        cpu.clock();

    dbgln("3 X 10 = ", static_cast<unsigned>(cpu.registers().a));
    dbgln("Total CPU cycles: ", static_cast<unsigned>(cpu.cycles()));

    cpu.print_registers();

    auto wm = new WindowManager();
    wm->init();

    auto menu_widget = new MenuWidget();
    wm->add_widget(menu_widget);

    auto ram_widget = new RamWidget(&bus);
    wm->add_widget(ram_widget);

    auto status_widget = new StatusWidget(&cpu);
    wm->add_widget(status_widget);

    wm->run();
    
    return 0;
}
