#include "Bus.h"
#include "CPU.h"
#include <sstream>

int main() 
{
    auto bus = Bus();
    auto cpu = CPU6502();
    bus.connect_to_cpu(&cpu);
    cpu.connect_to_bus(&bus);

    std::stringstream ss;
    ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
    u16 offset = 0x8000;
    while (!ss.eof()) {
        std::string b;
        ss >> b;
        bus.ram[offset++] = (u8)std::stoul(b, nullptr, 16);
    }

    bus.write(0xFFFC, 0x00);
    bus.write(0xFFFD, 0x80);

    cpu.reset();

    while (!(cpu.registers().a == 0x1e)) {
        cpu.clock();
    }

    dbgln("3 X 10 = ", unsigned(cpu.registers().a));
    dbgln("Total CPU cycles: ", unsigned(cpu.cycles()));
    
    return 0;
}
