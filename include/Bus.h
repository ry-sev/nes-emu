#pragma once

#define RAM_SIZE 0xFFFF

class CPU6502;

class Bus {

public:
    Bus();
    ~Bus() = default;

    u8 ram[RAM_SIZE];

    void connect_to_cpu(CPU6502* cpu) { m_cpu = cpu; };
    void load_program_from_string(const char* program);
    u8 read(u16 address);
    void write(u16 address, u8 value);

private:
    CPU6502* m_cpu;
};
