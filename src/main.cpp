#include "Bus.h"
#include "CPU.h"
#include "WindowManager.h"

int main() 
{
    auto bus = Bus();
    auto cpu = CPU6502();

    bus.connect_to_cpu(&cpu);
    cpu.connect_to_bus(&bus);

    bus.load_program_from_string("A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA");

    cpu.reset();

    auto wm = WindowManager(&bus, &cpu);
    wm.init();
    wm.run();
    
    return 0;
}
