#pragma once
#include <vector>
#include <map>

enum AddressingModes {
    Absolute,
    Absolute_X,
    Absolute_Y,
    Accumalator,
    Immediate,
    Implied,
    Indirect,
    Indirect_X,
    Indirect_Y,
    Relative,
    Zeropage,
    Zeropage_X,
    Zeropage_Y,
};

enum Flags {
    C = (1 << 0), // Carry
    Z = (1 << 1), // Zero
    I = (1 << 2), // Interrupt
    D = (1 << 3), // Decimal
    B = (1 << 4), // Break
    U = (1 << 5), // Unused
    V = (1 << 6), // Overflow
    N = (1 << 7), // Negative
};

typedef struct Registers {
    u8 a   = 0x00;  // accumulator
    u8 x   = 0x00;  // index register
    u8 y   = 0x00;  // index register
    u16 pc = 0x00;  // program counter
    u8 s   = 0x00;  // stack pointer
    u8 p   = 0x00;  // status register
} Registers;

struct InstructionStrings {
    std::string address;
    std::string instruction;
    std::string mode;
};

class Bus;

class CPU6502 {

public:
    CPU6502();
    ~CPU6502() = default;

    struct Instruction {
        std::string name;
        void (CPU6502::*mode)(void) = nullptr;
        void (CPU6502::*instruction)(void) = nullptr;
    };
    
    Registers& registers() { return m_registers; }
    void connect_to_bus(Bus* bus) { m_bus = bus; };
    std::vector<Instruction> lookup_table() { return m_lookup_table; }
    std::map<u16, InstructionStrings> disassemble(u16 start_address, u16 end_address);
    u8 cycles() { return m_cycles; }
    
    void reset();
    u8 get_flag(Flags flag);
    u8 read(u16 address);
    void write(u16 address, u8 value);
    void clock();

private:

    Bus* m_bus;
    Registers m_registers;
    AddressingModes m_addressing_mode;
    std::vector<Instruction> m_lookup_table;

    u16 m_current_address;
    u16 m_offset;
    u8 m_current_value;
    u8 m_cycles;
    bool m_page_boundary_crossed;

    void set_flag(Flags flag, bool value);

    void AM_ABS();
    void AM_ABX();
    void AM_ABY();
    void AM_ACC();
    void AM_IMM();
    void AM_IMP();
    void AM_IND();
    void AM_INX();
    void AM_INY();
    void AM_REL();
    void AM_ZER();
    void AM_ZEX();
    void AM_ZEY();

    void ADC();
    void AND();
    void ASL();
    void BCC();
    void BCS();
    void BEQ();
    void BIT();
    void BMI();
    void BNE();
    void BPL();
    void BRK();
    void BVC();
    void BVS();
    void CLC();
    void CLD();
    void CLI();
    void CLV();
    void CMP();
    void CPX();
    void CPY();
    void DEC();
    void DEX();
    void DEY();
    void EOR();
    void INC();
    void INX();
    void INY();
    void JMP();
    void JSR();
    void LDA();
    void LDX();
    void LDY();
    void LSR();
    void NOP();
    void ORA();
    void PHA();
    void PHP();
    void PLA();
    void PLP();
    void ROL();
    void ROR();
    void RTI();
    void RTS();
    void SBC();
    void SEC();
    void SED();
    void SEI();
    void STA();
    void STX();
    void STY();
    void TAX();
    void TAY();
    void TSX();
    void TXA();
    void TXS();
    void TYA();
    void XXX();

    void IRQ();
    void NMI();
};
