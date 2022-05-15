#include "CPU.h"
#include "Bus.h"
#include <iostream>
#include <cstdio>

CPU6502::CPU6502()
{
    m_lookup_table = {
        {"BRK", &CPU6502::AM_IMP, &CPU6502::BRK},
        {"ORA", &CPU6502::AM_INX, &CPU6502::ORA},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"ORA", &CPU6502::AM_ZER, &CPU6502::ORA},
        {"ASL", &CPU6502::AM_ZER, &CPU6502::ASL},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"PHP", &CPU6502::AM_IMP, &CPU6502::PHP},
        {"ORA", &CPU6502::AM_IMM, &CPU6502::ORA},
        {"ASL", &CPU6502::AM_ACC, &CPU6502::ASL},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"ORA", &CPU6502::AM_ABS, &CPU6502::ORA},
        {"ASL", &CPU6502::AM_ABS, &CPU6502::ASL},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"BPL", &CPU6502::AM_REL, &CPU6502::BPL},
        {"ORA", &CPU6502::AM_INY, &CPU6502::ORA},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"ORA", &CPU6502::AM_ZEX, &CPU6502::ORA},
        {"ASL", &CPU6502::AM_ZEX, &CPU6502::ASL},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CLC", &CPU6502::AM_IMP, &CPU6502::CLC},
        {"ORA", &CPU6502::AM_ABY, &CPU6502::ORA},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"ORA", &CPU6502::AM_ABX, &CPU6502::ORA},
        {"ASL", &CPU6502::AM_ABX, &CPU6502::ASL},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"JSR", &CPU6502::AM_ABS, &CPU6502::JSR},
        {"AND", &CPU6502::AM_INX, &CPU6502::AND},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"BIT", &CPU6502::AM_ZER, &CPU6502::BIT},
        {"AND", &CPU6502::AM_ZER, &CPU6502::AND},
        {"ROL", &CPU6502::AM_ZER, &CPU6502::ROL},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"PLP", &CPU6502::AM_IMP, &CPU6502::PLP},
        {"AND", &CPU6502::AM_IMM, &CPU6502::AND},
        {"ROL", &CPU6502::AM_ACC, &CPU6502::ROL},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"BIT", &CPU6502::AM_ABS, &CPU6502::BIT},
        {"AND", &CPU6502::AM_ABS, &CPU6502::AND},
        {"ROL", &CPU6502::AM_ABS, &CPU6502::ROL},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"BMI", &CPU6502::AM_REL, &CPU6502::BMI},
        {"AND", &CPU6502::AM_INY, &CPU6502::AND},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"AND", &CPU6502::AM_ZEX, &CPU6502::AND},
        {"ROL", &CPU6502::AM_ZEX, &CPU6502::ROL},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"SEC", &CPU6502::AM_IMP, &CPU6502::SEC},
        {"AND", &CPU6502::AM_ABY, &CPU6502::AND},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"AND", &CPU6502::AM_ABX, &CPU6502::AND},
        {"ROL", &CPU6502::AM_ABX, &CPU6502::ROL},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"RTI", &CPU6502::AM_IMP, &CPU6502::RTI},
        {"EOR", &CPU6502::AM_INX, &CPU6502::EOR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"EOR", &CPU6502::AM_ZER, &CPU6502::EOR},
        {"LSR", &CPU6502::AM_ZER, &CPU6502::LSR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"PHA", &CPU6502::AM_IMP, &CPU6502::PHA},
        {"EOR", &CPU6502::AM_IMM, &CPU6502::EOR},
        {"LSR", &CPU6502::AM_ACC, &CPU6502::LSR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"JMP", &CPU6502::AM_ABS, &CPU6502::JMP},
        {"EOR", &CPU6502::AM_ABS, &CPU6502::EOR},
        {"LSR", &CPU6502::AM_ABS, &CPU6502::LSR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"BVC", &CPU6502::AM_REL, &CPU6502::BVC},
        {"EOR", &CPU6502::AM_INY, &CPU6502::EOR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"EOR", &CPU6502::AM_ZEX, &CPU6502::EOR},
        {"LSR", &CPU6502::AM_ZEX, &CPU6502::LSR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CLI", &CPU6502::AM_IMP, &CPU6502::CLI},
        {"EOR", &CPU6502::AM_ABY, &CPU6502::EOR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"EOR", &CPU6502::AM_ABX, &CPU6502::EOR},
        {"LSR", &CPU6502::AM_ABX, &CPU6502::LSR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"RTS", &CPU6502::AM_IMP, &CPU6502::RTS},
        {"ADC", &CPU6502::AM_INX, &CPU6502::ADC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"ADC", &CPU6502::AM_ZER, &CPU6502::ADC},
        {"ROR", &CPU6502::AM_ZER, &CPU6502::ROR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"PLA", &CPU6502::AM_IMP, &CPU6502::PLA},
        {"ADC", &CPU6502::AM_IMM, &CPU6502::ADC},
        {"ROR", &CPU6502::AM_ACC, &CPU6502::ROR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"JMP", &CPU6502::AM_IND, &CPU6502::JMP},
        {"ADC", &CPU6502::AM_ABS, &CPU6502::ADC},
        {"ROR", &CPU6502::AM_ABS, &CPU6502::ROR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"BVS", &CPU6502::AM_REL, &CPU6502::BVS},
        {"ADC", &CPU6502::AM_INY, &CPU6502::ADC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"ADC", &CPU6502::AM_ZEX, &CPU6502::ADC},
        {"ROR", &CPU6502::AM_ZEX, &CPU6502::ROR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"SEI", &CPU6502::AM_IMP, &CPU6502::SEI},
        {"ADC", &CPU6502::AM_ABY, &CPU6502::ADC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"ADC", &CPU6502::AM_ABX, &CPU6502::ADC},
        {"ROR", &CPU6502::AM_ABX, &CPU6502::ROR},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"STA", &CPU6502::AM_INX, &CPU6502::STA},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"STY", &CPU6502::AM_ZER, &CPU6502::STY},
        {"STA", &CPU6502::AM_ZER, &CPU6502::STA},
        {"STX", &CPU6502::AM_ZER, &CPU6502::STX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"DEY", &CPU6502::AM_IMP, &CPU6502::DEY},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"TXA", &CPU6502::AM_IMP, &CPU6502::TXA},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"STY", &CPU6502::AM_ABS, &CPU6502::STY},
        {"STA", &CPU6502::AM_ABS, &CPU6502::STA},
        {"STX", &CPU6502::AM_ABS, &CPU6502::STX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"BCC", &CPU6502::AM_REL, &CPU6502::BCC},
        {"STA", &CPU6502::AM_INY, &CPU6502::STA},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"STY", &CPU6502::AM_ZEX, &CPU6502::STY},
        {"STA", &CPU6502::AM_ZEX, &CPU6502::STA},
        {"STX", &CPU6502::AM_ZEY, &CPU6502::STX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"TYA", &CPU6502::AM_IMP, &CPU6502::TYA},
        {"STA", &CPU6502::AM_ABY, &CPU6502::STA},
        {"TXS", &CPU6502::AM_IMP, &CPU6502::TXS},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"STA", &CPU6502::AM_ABX, &CPU6502::STA},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"LDY", &CPU6502::AM_IMM, &CPU6502::LDY},
        {"LDA", &CPU6502::AM_INX, &CPU6502::LDA},
        {"LDX", &CPU6502::AM_IMM, &CPU6502::LDX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"LDY", &CPU6502::AM_ZER, &CPU6502::LDY},
        {"LDA", &CPU6502::AM_ZER, &CPU6502::LDA},
        {"LDX", &CPU6502::AM_ZER, &CPU6502::LDX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"TAY", &CPU6502::AM_IMP, &CPU6502::TAY},
        {"LDA", &CPU6502::AM_IMM, &CPU6502::LDA},
        {"TAX", &CPU6502::AM_IMP, &CPU6502::TAX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"LDY", &CPU6502::AM_ABS, &CPU6502::LDY},
        {"LDA", &CPU6502::AM_ABS, &CPU6502::LDA},
        {"LDX", &CPU6502::AM_ABS, &CPU6502::LDX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"BCS", &CPU6502::AM_REL, &CPU6502::BCS},
        {"LDA", &CPU6502::AM_INY, &CPU6502::LDA},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"LDY", &CPU6502::AM_ZEX, &CPU6502::LDY},
        {"LDA", &CPU6502::AM_ZEX, &CPU6502::LDA},
        {"LDX", &CPU6502::AM_ZEY, &CPU6502::LDX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CLV", &CPU6502::AM_IMP, &CPU6502::CLV},
        {"LDA", &CPU6502::AM_ABY, &CPU6502::LDA},
        {"TSX", &CPU6502::AM_IMP, &CPU6502::TSX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"LDY", &CPU6502::AM_ABX, &CPU6502::LDY},
        {"LDA", &CPU6502::AM_ABX, &CPU6502::LDA},
        {"LDX", &CPU6502::AM_ABY, &CPU6502::LDX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CPY", &CPU6502::AM_IMM, &CPU6502::CPY},
        {"CMP", &CPU6502::AM_INX, &CPU6502::CMP},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CPY", &CPU6502::AM_ZER, &CPU6502::CPY},
        {"CMP", &CPU6502::AM_ZER, &CPU6502::CMP},
        {"DEC", &CPU6502::AM_ZER, &CPU6502::DEC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"INY", &CPU6502::AM_IMP, &CPU6502::INY},
        {"CMP", &CPU6502::AM_IMM, &CPU6502::CMP},
        {"DEX", &CPU6502::AM_IMP, &CPU6502::DEX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CPY", &CPU6502::AM_ABS, &CPU6502::CPY},
        {"CMP", &CPU6502::AM_ABS, &CPU6502::CMP},
        {"DEC", &CPU6502::AM_ABS, &CPU6502::DEC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"BNE", &CPU6502::AM_REL, &CPU6502::BNE},
        {"CMP", &CPU6502::AM_INY, &CPU6502::CMP},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CMP", &CPU6502::AM_ZEX, &CPU6502::CMP},
        {"DEC", &CPU6502::AM_ZEX, &CPU6502::DEC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CLD", &CPU6502::AM_IMP, &CPU6502::CLD},
        {"CMP", &CPU6502::AM_ABY, &CPU6502::CMP},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CMP", &CPU6502::AM_ABX, &CPU6502::CMP},
        {"DEC", &CPU6502::AM_ABX, &CPU6502::DEC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CPX", &CPU6502::AM_IMM, &CPU6502::CPX},
        {"SBC", &CPU6502::AM_INX, &CPU6502::SBC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CPX", &CPU6502::AM_ZER, &CPU6502::CPX},
        {"SBC", &CPU6502::AM_ZER, &CPU6502::SBC},
        {"INC", &CPU6502::AM_ZER, &CPU6502::INC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"INX", &CPU6502::AM_IMP, &CPU6502::INX},
        {"SBC", &CPU6502::AM_IMM, &CPU6502::SBC},
        {"NOP", &CPU6502::AM_IMP, &CPU6502::NOP},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"CPX", &CPU6502::AM_ABS, &CPU6502::CPX},
        {"SBC", &CPU6502::AM_ABS, &CPU6502::SBC},
        {"INC", &CPU6502::AM_ABS, &CPU6502::INC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"BEQ", &CPU6502::AM_REL, &CPU6502::BEQ},
        {"SBC", &CPU6502::AM_INY, &CPU6502::SBC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"SBC", &CPU6502::AM_ZEX, &CPU6502::SBC},
        {"INC", &CPU6502::AM_ZEX, &CPU6502::INC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"SED", &CPU6502::AM_IMP, &CPU6502::SED},
        {"SBC", &CPU6502::AM_ABY, &CPU6502::SBC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX},
        {"SBC", &CPU6502::AM_ABX, &CPU6502::SBC},
        {"INC", &CPU6502::AM_ABX, &CPU6502::INC},
        {"???", &CPU6502::AM_IMP, &CPU6502::XXX}
    };
}

void CPU6502::reset()
{
    m_registers = {};
    m_current_address = 0xFFFC;
    u16 low_byte = read(m_current_address);
    u16 high_byte = read(m_current_address + 1);

    m_registers.pc = (high_byte << 8) | low_byte;
    //m_registers.pc = 0xC000; // NESTEST

    set_flag(U, 1);
    m_registers.s = 0xFD;
    //m_registers.p = 0x00 | U;
    m_registers.p = 0x24;

    m_current_address = 0x0000;
    m_offset = 0x0000;
    m_current_value = 0x00;

    m_cycles = 7;
}

u8 CPU6502::get_flag(Flags flag)
{
    return ((m_registers.p & flag) > 0) ? 1 : 0;
}

void CPU6502::set_flag(Flags flag, bool value)
{
    value ? m_registers.p |= flag : m_registers.p &= ~flag;
}

// Addressing modes

void CPU6502::AM_ABS()
{
    u16 low_byte = read(m_registers.pc++);
    u16 high_byte = read(m_registers.pc++);
    m_current_address = (high_byte << 8) | low_byte;

    m_addressing_mode = Absolute;
}

void CPU6502::AM_ABX()
{
    u16 low_byte = read(m_registers.pc++);
    u16 high_byte = read(m_registers.pc++);
    m_current_address = (high_byte << 8) | low_byte;
    m_current_address += m_registers.x;

    if ((m_current_address & 0xFF00) != (high_byte << 8))
        m_page_boundary_crossed = true;

    m_addressing_mode = Absolute_X;
}

void CPU6502::AM_ABY()
{
    u16 low_byte = read(m_registers.pc++);
    u16 high_byte = read(m_registers.pc++);
    m_current_address = (high_byte << 8) | low_byte;
    m_current_address += m_registers.y;

    if ((m_current_address & 0xFF00) != (high_byte << 8))
        m_page_boundary_crossed = true;

    m_addressing_mode = Absolute_Y;
}

void CPU6502::AM_ACC()
{
    m_current_value = m_registers.a;
    m_addressing_mode = Accumalator;
}

void CPU6502::AM_IMM()
{
    m_current_address = m_registers.pc++;
    m_addressing_mode = Immediate;
}

void CPU6502::AM_IMP()
{
    m_current_value = m_registers.a;
    m_addressing_mode = Implied;
}

void CPU6502::AM_IND()
{
    u16 low_byte = read(m_registers.pc++);
    u16 high_byte = read(m_registers.pc++);
    u16 ptr = (high_byte << 8) | low_byte;

    if (low_byte == 0x00FF)
        m_current_address = (read( ptr & 0xFF00) << 8) | read(ptr + 0);
    else
        m_current_address = (read(ptr + 1) << 8) | read(ptr + 0);

    m_addressing_mode = Indirect;
}

void CPU6502::AM_INX()
{
    u16 value = read(m_registers.pc++);
    u16 low_byte = read((value + m_registers.x) & 0x00FF);
    u16 high_byte = read((value + m_registers.x + 1) & 0x00FF);

    m_current_address = (high_byte << 8) | low_byte;

    m_addressing_mode = Indirect_X;
}

void CPU6502::AM_INY()
{
    u16 value = read(m_registers.pc++);
    u16 low_byte = read(value & 0x00FF);
    u16 high_byte = read((value + 1) & 0x00FF);

    m_current_address = (high_byte << 8) | low_byte;
    m_current_address += m_registers.y;

    if ((m_current_address & 0xFF00) != (high_byte << 8))
        m_page_boundary_crossed = true;

    m_addressing_mode = Indirect_Y;
}

void CPU6502::AM_REL()
{
    m_offset = read(m_registers.pc++);
    if (m_offset & 0x80)
        m_offset |= 0xFF00;

    m_addressing_mode = Relative;
}

void CPU6502::AM_ZER()
{
    m_current_address = read(m_registers.pc++);
    m_current_address &= 0x00FF;
    m_addressing_mode = Zeropage;
}

void CPU6502::AM_ZEX()
{
    m_current_address = (read(m_registers.pc++) + m_registers.x);
    m_current_address &= 0x00FF;
    m_addressing_mode = Zeropage_X;
}

void CPU6502::AM_ZEY()
{
    m_current_address = (read(m_registers.pc++) + m_registers.y);
    m_current_address &= 0x00FF;
    m_addressing_mode = Zeropage_Y;
}

// Instructions

void CPU6502::ADC()
{
    m_current_value = read(m_current_address);
    auto value = (u16)m_registers.a + (u16)m_current_value + (u16)get_flag(C);

    set_flag(C, value > 255);
    set_flag(Z, (value & 0x00FF) == 0x00);
    set_flag(V, (~((u16)m_registers.a ^ (u16)m_current_value) & ((u16)m_registers.a ^ (u16)value)) & 0x0080);
    set_flag(N, value & 0x80);

    m_registers.a = value & 0x00FF;

    switch (m_addressing_mode) {
        case Immediate: m_cycles += 2; break; 
        case Zeropage: m_cycles += 3; break;
        case Zeropage_X: 
        case Absolute: m_cycles += 4; break;
        case Absolute_X: 
        case Absolute_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 4;
            break;
        }
        case Indirect_X: m_cycles += 6; break;
        case Indirect_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 5;
            break;
        }
        default: break;
    }
}

void CPU6502::AND()
{
    m_current_value = read(m_current_address);
    m_registers.a &= m_current_value;

    set_flag(Z, m_registers.a == 0x00);
    set_flag(N, m_registers.a & 0x80);

    switch (m_addressing_mode) {
        case Immediate:
            m_cycles += 2; break;
        case Zeropage:
            m_cycles += 3; break;
        case Zeropage_X:
        case Absolute:
            m_cycles += 4; break;
        case Absolute_X:
        case Absolute_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 4;
            break;
        }
        case Indirect_X: m_cycles += 6; break;
        case Indirect_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 5;
            break;
        }
        default: break;
    }
}

void CPU6502::ASL()
{
    m_current_value = read(m_current_address);
    auto value = (u16)m_current_value << 1;

    set_flag(Z, (value & 0x00FF) == 0x00);
    set_flag(C, (value & 0xFF00) > 0);
    set_flag(N, (value & 0x80));

    if (m_addressing_mode == Accumalator)
        m_registers.a = value & 0x00FF;
    else
        write(m_current_address, value & 0x00FF);

    switch(m_addressing_mode) {
        case Accumalator: m_cycles += 2; break;
        case Zeropage: m_cycles +=5; break;
        case Absolute: 
        case Zeropage_X: m_cycles += 6; break;
        case Absolute_X: m_cycles += 7; break;
        default: break;
    }
}

void CPU6502::BCC()
{
    if (get_flag(C) == 0) {
        m_cycles++;
        m_current_address = m_registers.pc + m_offset;
        if ((m_current_address & 0xFF00) != (m_registers.pc & 0xFF00))
            m_cycles ++;
        
        m_registers.pc = m_current_address;
    }
    m_cycles += 2;
}

void CPU6502::BCS()
{
    if (get_flag(C) == 1) {
        m_cycles++;
        m_current_address = m_registers.pc + m_offset;
        if ((m_current_address & 0xFF00) != (m_registers.pc & 0xFF00))
            m_cycles ++;

        m_registers.pc = m_current_address;
    }
    m_cycles += 2;
}

void CPU6502::BEQ()
{
    if (get_flag(Z) == 1) {
        m_cycles++;
        m_current_address = m_registers.pc + m_offset;
        if ((m_current_address & 0xFF00) != (m_registers.pc & 0xFF00))
            m_cycles ++;

        m_registers.pc = m_current_address;
    }
    m_cycles += 2;
}

void CPU6502::BIT()
{
    m_current_value = read(m_current_address);
    auto result = m_registers.a & m_current_value;

    set_flag(Z, (result & 0x00FF) == 0x00);
    set_flag(V, m_current_value & (1 << 6));
    set_flag(N, m_current_value & (1 << 7));

    switch (m_addressing_mode) {
        case Zeropage: m_cycles += 3; break;
        case Absolute: m_cycles += 4; break;
        default: break;
    }
}

void CPU6502::BMI()
{
    if (get_flag(N) == 1) {
        m_cycles++;
        m_current_address = m_registers.pc + m_offset;
        if ((m_current_address & 0xFF00) != (m_registers.pc & 0xFF00))
            m_cycles ++;

        m_registers.pc = m_current_address;
    }
    m_cycles += 2;
}

void CPU6502::BNE()
{
    if (get_flag(Z) == 0) {
        m_cycles++;
        m_current_address = m_registers.pc + m_offset;
        if ((m_current_address & 0xFF00) != (m_registers.pc & 0xFF00))
            m_cycles ++;

        m_registers.pc = m_current_address;
    }
    m_cycles += 2;
}

void CPU6502::BPL()
{
    if (get_flag(N) == 0) {
        m_cycles++;
        m_current_address = m_registers.pc + m_offset;
        if ((m_current_address & 0xFF00) != (m_registers.pc & 0xFF00))
            m_cycles ++;

        m_registers.pc = m_current_address;
    }
    m_cycles += 2;
}

void CPU6502::BRK()
{
    m_registers.pc++;
    set_flag(I, 1);
    
    write(0x0100 + m_registers.s, (m_registers.pc >> 8) & 0x00FF);
    m_registers.s--;

    write(0x0100 + m_registers.s, m_registers.pc & 0x00FF);
    m_registers.s--;

    set_flag(B, 1);

    write(0x0100 + m_registers.s, m_registers.p);
    m_registers.s--;
    set_flag(B, 0);

    m_registers.pc = (u16)read(0xFFFE) | ((u16)read(0xFFFF) << 8);

    m_cycles += 7;
}

void CPU6502::BVC()
{
    if (get_flag(V) == 0) {
        m_cycles++;
        m_current_address = m_registers.pc + m_offset;
        if ((m_current_address & 0xFF00) != (m_registers.pc & 0xFF00))
            m_cycles ++;

        m_registers.pc = m_current_address;
    }
    m_cycles += 2;
}

void CPU6502::BVS()
{
    if (get_flag(V) == 1) {
        m_cycles++;
        m_current_address = m_registers.pc + m_offset;
        if ((m_current_address & 0xFF00) != (m_registers.pc & 0xFF00))
            m_cycles ++;

        m_registers.pc = m_current_address;
    }
    m_cycles += 2;
}

void CPU6502::CLC()
{
    set_flag(C, 0);
    m_cycles += 2;
}

void CPU6502::CLD()
{
    set_flag(D, 0);
    m_cycles += 2;
}

void CPU6502::CLI()
{
    set_flag(I, 0);
    m_cycles += 2;
}

void CPU6502::CLV()
{
    set_flag(V, 0);
    m_cycles += 2;
}

void CPU6502::CMP()
{
    m_current_value = read(m_current_address);
    auto value = (u16)m_registers.a - (u16)m_current_value;

    set_flag(C, m_registers.a >= m_current_value);
    set_flag(Z, (value & 0x00FF) == 0x0000);
    set_flag(N, value & 0x80);

    switch (m_addressing_mode) {
        case Immediate: m_cycles += 2; break;
        case Zeropage: m_cycles += 3; break;
        case Zeropage_X:
        case Absolute: m_cycles += 4; break;
        case Absolute_X:
        case Absolute_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 4;
            break;
        }
        case Indirect_X: m_cycles += 6; break;
        case Indirect_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 5;
            break;
        }
        default: break;
    }
}

void CPU6502::CPX()
{
    m_current_value = read(m_current_address);
    auto value = (u16)m_registers.x - (u16)m_current_value;

    set_flag(C, m_registers.x >= m_current_value);
    set_flag(Z, (value & 0x00FF) == 0x0000);
    set_flag(N, value & 0x0080);

    switch (m_addressing_mode) {
        case Immediate: m_cycles += 2; break;
        case Zeropage: m_cycles += 3; break;
        case Absolute: m_cycles += 4; break;
        default: break;
    }
}

void CPU6502::CPY()
{
    m_current_value = read(m_current_address);
    auto value = (u16)m_registers.y - (u16)m_current_value;

    set_flag(C, m_registers.y >= m_current_value);
    set_flag(Z, (value & 0x00FF) == 0x0000);
    set_flag(N, value & 0x0080);

    switch (m_addressing_mode) {
        case Immediate: m_cycles += 2; break;
        case Zeropage: m_cycles += 3; break;
        case Absolute: m_cycles += 4; break;
        default: break;
    }
}

void CPU6502::DEC()
{
    m_current_value = read(m_current_address);
    auto value = m_current_value - 0x01;

    write(m_current_address, value & 0x00FF);

    set_flag(Z, (value & 0x00FF) == 0x0000);
    set_flag(N, value & 0x0080);

    switch (m_addressing_mode) {
        case Zeropage: m_cycles += 5; break;
        case Zeropage_X:
        case Absolute: m_cycles += 6; break;
        case Absolute_X: m_cycles += 7; break;
        default: break;
    }
}

void CPU6502::DEX()
{
    m_registers.x--;

    set_flag(Z, m_registers.x == 0x00);
    set_flag(N, m_registers.x & 0x80);

    m_cycles += 2;
}

void CPU6502::DEY()
{
    m_registers.y--;

    set_flag(Z, m_registers.y == 0x00);
    set_flag(N, m_registers.y & 0x80);

    m_cycles += 2;
}

void CPU6502::EOR()
{
    m_current_value = read(m_current_address);
    m_registers.a ^= m_current_value;

    set_flag(Z, m_registers.a == 0x00);
    set_flag(N, m_registers.a & 0x80);

    switch (m_addressing_mode) {
        case Immediate: m_cycles += 2; break;
        case Zeropage: m_cycles += 3; break;
        case Zeropage_X:
        case Absolute: m_cycles += 4; break;
        case Absolute_X:
        case Absolute_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 4;
            break;
        }
        case Indirect_X: m_cycles += 6; break;
        case Indirect_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 5;
            break;
        }
        default: break;
    }
}

void CPU6502::INC()
{
    m_current_value = read(m_current_address);
    auto value = m_current_value + 0x01;

    write(m_current_address, value & 0x00FF);

    set_flag(Z, (value & 0x00FF) == 0x0000);
    set_flag(N, value & 0x0080);

    switch (m_addressing_mode) {
        case Zeropage: m_cycles += 5; break;
        case Zeropage_X:
        case Absolute: m_cycles += 6; break;
        case Absolute_X: m_cycles += 7; break;
        default: break;
    }
}

void CPU6502::INX()
{
    m_registers.x++;

    set_flag(Z, m_registers.x == 0x00);
    set_flag(N, m_registers.x & 0x80);

    m_cycles += 2;
}

void CPU6502::INY()
{
    m_registers.y++;

    set_flag(Z, m_registers.y == 0x00);
    set_flag(N, m_registers.y & 0x80);

    m_cycles += 2;
}

void CPU6502::JMP()
{
    m_registers.pc = m_current_address;

    switch (m_addressing_mode) {
        case Absolute: m_cycles += 3; break;
        case Indirect: m_cycles += 5; break;
        default: break;
    }
}

void CPU6502::JSR()
{
    m_registers.pc--;

    write(0x0100 + m_registers.s , (m_registers.pc >> 8) & 0x00FF);
    m_registers.s--;

    write(0x0100 + m_registers.s , m_registers.pc & 0x00FF);
    m_registers.s--;

    m_registers.pc = m_current_address;

    m_cycles += 6;
}

void CPU6502::LDA()
{
    m_current_value = read(m_current_address);
    m_registers.a = m_current_value;

    set_flag(Z, m_registers.a == 0x00);
    set_flag(N, m_registers.a & 0x80);

    switch (m_addressing_mode) {
        case Immediate: m_cycles += 2; break;
        case Zeropage: m_cycles += 3; break;
        case Zeropage_X: 
        case Absolute: m_cycles += 4; break;
        case Absolute_X:
        case Absolute_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 4;
            break;
        } 
        case Indirect_X: m_cycles += 6; break;
        case Indirect_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 5;
            break;
        } 
        default: break;
    }
}

void CPU6502::LDX()
{
    m_current_value = read(m_current_address);
    m_registers.x = m_current_value;

    set_flag(Z, m_registers.x == 0x00);
    set_flag(N, m_registers.x & 0x80);

    switch (m_addressing_mode) {
        case Immediate: m_cycles += 2; break;
        case Zeropage: m_cycles += 3; break;
        case Zeropage_Y:
        case Absolute: m_cycles += 4; break;
        case Absolute_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 4;
            break;
        }
        default: break;
    }
}

void CPU6502::LDY()
{
    m_current_value = read(m_current_address);
    m_registers.y = m_current_value;

    set_flag(Z, m_registers.y == 0x00);
    set_flag(N, m_registers.y & 0x80);

    switch (m_addressing_mode) {
        case Immediate: m_cycles += 2; break;
        case Zeropage: m_cycles += 3; break;
        case Zeropage_X:
        case Absolute: m_cycles += 4; break;
        case Absolute_X: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 4;
            break;
        }
        default: break;
    }
}

void CPU6502::LSR()
{
    m_current_value = read(m_current_address);
    auto value = m_current_value >> 1;

    set_flag(C, m_current_value & 0x0001);
    set_flag(Z, (value & 0x00FF) == 0x0000);
    set_flag(N, (value & 0x0080));

    if (m_addressing_mode == Accumalator)
        m_registers.a = value & 0x00FF;
    else
        write(m_current_address, value & 0x00FF);

    switch (m_addressing_mode) {
        case Accumalator: m_cycles += 2; break;
        case Zeropage: m_cycles += 5; break;
        case Zeropage_X:
        case Absolute: m_cycles += 6; break;
        case Absolute_X: m_cycles += 7; break;
        default: break;
    }

}

void CPU6502::NOP()
{
    m_cycles += 2;
}

void CPU6502::ORA()
{
    m_current_value = read(m_current_address);
    m_registers.a |= m_current_value;

    set_flag(Z, m_registers.a == 0x00);
    set_flag(N, m_registers.a & 0x80);

    switch (m_addressing_mode) {
        case Immediate: m_cycles += 2; break;
        case Zeropage: m_cycles += 3; break;
        case Zeropage_X:
        case Absolute: m_cycles += 4; break;
        case Absolute_X:
        case Absolute_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 4;
            break;
        }
        case Indirect_X: m_cycles += 6; break;
        case Indirect_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 5;
            break;
        }
        default: break;
    }
}

void CPU6502::PHA()
{
    write(0x0100 + m_registers.s, m_registers.a);
    m_registers.s--;
    m_cycles += 3;
}

void CPU6502::PHP()
{
    write(0x0100 + m_registers.s, m_registers.p | B | U);
	set_flag(B, 0);
	set_flag(U, 0);
    m_registers.s--;
    m_cycles += 3;
}

void CPU6502::PLA()
{
    m_registers.s++;
    m_registers.a = read(0x0100 + m_registers.s);

    set_flag(Z, m_registers.a == 0x00);
    set_flag(N, m_registers.a & 0x80);

    m_cycles += 4;
}

void CPU6502::PLP()
{
    m_registers.s++;
    m_registers.p = ((read(0x100 + m_registers.s) & 0xef) | (0xab & 0x10)) | 0x20;
    set_flag(U, 1);

    m_cycles += 4;
}

void CPU6502::ROL()
{
    m_current_value = read(m_current_address);
    auto value = (u16)(m_current_value << 1) | get_flag(C);

    set_flag(C, value & 0xFF00);
    set_flag(Z, (value & 0x00FF) == 0x0000);
    set_flag(N, value & 0x0080);

    if (m_addressing_mode == Accumalator)
        m_registers.a = value & 0x00FF;
    else
        write(m_current_address, value & 0x00FF);

    switch (m_addressing_mode) {
        case Accumalator: m_cycles += 2; break;
        case Zeropage: m_cycles += 5; break;
        case Zeropage_X:
        case Absolute: m_cycles += 6; break;
        case Absolute_X: m_cycles += 7; break;
        default: break;
    }
}

void CPU6502::ROR()
{
    m_current_value = read(m_current_address);
    auto value = (u16)(get_flag(C) << 7) | (m_current_value >> 1);

    set_flag(C, m_current_value & 0x01);
    set_flag(Z, (value & 0x00FF) == 0x00);
    set_flag(N, value & 0x0080);

    if (m_addressing_mode == Accumalator)
        m_registers.a = value & 0x00FF;
    else
        write(m_current_address, value & 0x00FF);

    switch (m_addressing_mode) {
        case Accumalator: m_cycles += 2; break;
        case Zeropage: m_cycles += 5; break;
        case Zeropage_X:
        case Absolute: m_cycles += 6; break;
        case Absolute_X: m_cycles += 7; break;
        default: break;
    }
}

void CPU6502::RTI()
{
    m_registers.s++;
    
    m_registers.p = read(0x0100 + m_registers.s);
    m_registers.p &= ~B;
    m_registers.p &= ~U;

    m_registers.s++;
    m_registers.pc = (u16)read(0x0100 + m_registers.s);
    m_registers.s++;

    m_registers.pc |= (u16)read(0x0100 + m_registers.s) << 8;

    m_cycles += 6;
}

void CPU6502::RTS()
{
    m_registers.s++;
    m_registers.pc = (u16)read(0x0100 + m_registers.s);
    m_registers.s++;
    m_registers.pc |= (u16)read(0x0100 + m_registers.s) << 8;

    m_registers.pc++;

    m_cycles += 6;
}

void CPU6502::SBC()
{
    m_current_value = read(m_current_address);

    auto inverted_value = (u16)m_current_value ^ 0x00FF;
    auto value = (u16)m_registers.a + inverted_value + (u16)get_flag(C);

    set_flag(C, value & 0xFF00);
    set_flag(Z, (value & 0x00FF) == 0x00);
    set_flag(V, (value ^ (u16)m_registers.a) & (value ^ inverted_value) & 0x80);
    set_flag(N, value & 0x0080);

    m_registers.a = value & 0x00FF;

    switch (m_addressing_mode) {
        case Immediate: m_cycles += 2; break; 
        case Zeropage: m_cycles += 3; break;
        case Zeropage_X: 
        case Absolute: m_cycles += 4; break;
        case Absolute_X: 
        case Absolute_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 4;
            break;
        }
        case Indirect_X: m_cycles += 6; break;
        case Indirect_Y: {
            if (m_page_boundary_crossed)
                m_cycles++;
            m_cycles += 5;
            break;
        }
        default: break;
    }
}

void CPU6502::SEC()
{
    set_flag(C, 1);
    m_cycles +=2;
}

void CPU6502::SED()
{
    set_flag(D, 1);
    m_cycles +=2;
}

void CPU6502::SEI()
{
    set_flag(I, 1);
    m_cycles +=2;
}

void CPU6502::STA()
{
    write(m_current_address, m_registers.a);
    
    switch (m_addressing_mode) {
        case Zeropage: m_cycles += 3; break;
        case Zeropage_X: 
        case Absolute: m_cycles += 4; break;
        case Absolute_X: 
        case Absolute_Y: m_cycles += 5; break;
        case Indirect_X: 
        case Indirect_Y: m_cycles += 6; break;
        default: break;
    }
}

void CPU6502::STX()
{
    write(m_current_address, m_registers.x);
    
    switch (m_addressing_mode) {
        case Zeropage: m_cycles += 3; break;
        case Zeropage_Y:
        case Absolute: m_cycles += 4; break;
        default: break;
    }
}

void CPU6502::STY()
{
    write(m_current_address, m_registers.y);
    
    switch (m_addressing_mode) {
        case Zeropage: m_cycles += 3; break;
        case Zeropage_X:
        case Absolute: m_cycles += 4; break;
        default: break;
    }
}

void CPU6502::TAX()
{
    m_registers.x = m_registers.a;
    set_flag(Z, m_registers.x == 0x00);
    set_flag(N, m_registers.x & 0x80);
    m_cycles += 2;
}

void CPU6502::TAY()
{
    m_registers.y = m_registers.a;
    set_flag(Z, m_registers.y == 0x00);
    set_flag(N, m_registers.y & 0x80);
    m_cycles += 2;
}

void CPU6502::TSX()
{
    m_registers.x = m_registers.s;
    set_flag(Z, m_registers.x == 0x00);
    set_flag(N, m_registers.x & 0x80);
    m_cycles += 2;
}

void CPU6502::TXA()
{
    m_registers.a = m_registers.x;
    set_flag(Z, m_registers.a == 0x00);
    set_flag(N, m_registers.a & 0x80);
    m_cycles += 2;
}

void CPU6502::TXS()
{
    m_registers.s = m_registers.x;
    m_cycles += 2;
}

void CPU6502::TYA()
{
    m_registers.a = m_registers.y;
    set_flag(Z, m_registers.a == 0x00);
    set_flag(N, m_registers.a & 0x80);
    m_cycles += 2;
}

void CPU6502::XXX()
{
    return;
}

void CPU6502::irq()
{
    if (get_flag(I) == 0) {
        write(0x0100 + m_registers.s, (m_registers.pc >> 8) & 0x00FF);
        m_registers.s--;
        write(0x0100 + m_registers.s, m_registers.pc & 0x00FF);
        m_registers.s--;

        set_flag(B, 0);
        set_flag(U, 1);
        set_flag(I, 1);
        write(0x0100 + m_registers.s, m_registers.p);
        m_registers.s--;

        m_current_address = 0xFFFE;
        auto low_byte = read(m_current_address + 0);
        auto high_byte = read(m_current_address + 1);
        m_registers.pc = (high_byte << 8) | low_byte;

        m_cycles = 7;
    }
}

void CPU6502::nmi()
{
        write(0x0100 + m_registers.s, (m_registers.pc >> 8) & 0x00FF);
        m_registers.s--;
        write(0x0100 + m_registers.s, m_registers.pc & 0x00FF);
        m_registers.s--;

        set_flag(B, 0);
        set_flag(U, 1);
        set_flag(I, 1);
        write(0x0100 + m_registers.s, m_registers.p);
        m_registers.s--;

        m_current_address = 0xFFFA;
        auto low_byte = read(m_current_address + 0);
        auto high_byte = read(m_current_address + 1);
        m_registers.pc = (high_byte << 8) | low_byte;

        m_cycles = 8;
}

u8 CPU6502::read(u16 address)
{
    return m_bus->cpu_read(address);
}

void CPU6502::write(u16 address, u8 value)
{
    m_bus->cpu_write(address, value);
}

void CPU6502::clock()
{
    log_cpu();

    set_flag(U, 1);
    auto opcode = read(m_registers.pc++);
    (this->*m_lookup_table[opcode].mode)();
    (this->*m_lookup_table[opcode].instruction)();
    set_flag(U, 1);
    m_page_boundary_crossed = false;
}

void CPU6502::log_cpu()
{
    freopen("nestest.log", "a", stdout);

    auto instr = m_disassembly[m_registers.pc];

    std::cout
    << hex(m_registers.pc, 4)
    << " "
    << instr.opcodes
    << " "
    << instr.instruction
    << " "
    << "A:"
    << hex(m_registers.a, 2)
    << " X:"
    << hex(m_registers.x, 2)
    << " Y:"
    << hex(m_registers.y, 2)
    << " P:"
    << hex(m_registers.p, 2)
    << " SP:"
    << hex(m_registers.s, 2)
    << " CYC:"
    << std::to_string(m_cycles)
    << "\n";
}

std::map<u16, InstructionStrings> CPU6502::disassemble(u16 start_address, u16 end_address)
{
    std::map<u16, InstructionStrings> instruction_map;

    auto current_address = static_cast<u32>(start_address);

    while (current_address <= static_cast<u32>(end_address)) {

        u8 value = 0x00;
        u8 low_byte = 0x00;
        u8 high_byte = 0x00;

        auto id = current_address;
        std::string address = hex(current_address, 4);

        u8 opcode = read(current_address++);

        std::string mode_str = "";
        std::string opcodes = hex(opcode, 2);
        std::string instruction = m_lookup_table[opcode].name;

        auto mode = m_lookup_table[opcode].mode;

        if (mode == &CPU6502::AM_ACC) {
            mode_str = "ACC";
        }
        else if (mode == &CPU6502::AM_ABS) {
            low_byte = read(current_address++);
            high_byte = read(current_address++);
            opcodes += " " + hex(low_byte, 2) + " " + hex(high_byte, 2);
            instruction += " $" + hex((high_byte << 8) | low_byte, 4);
            mode_str = "ABS";
        }
        else if (mode == &CPU6502::AM_ABX) {
            low_byte = read(current_address++);
            high_byte = read(current_address++);
            opcodes += " " + hex(low_byte, 2) + " " + hex(high_byte, 2);
            instruction += " $" + hex((high_byte << 8) | low_byte, 4) + ",X";
            mode_str = "ABX";
        }
        else if (mode == &CPU6502::AM_ABY) {
            low_byte = read(current_address++);
            high_byte = read(current_address++);
            opcodes += " " + hex(low_byte, 2) + " " + hex(high_byte, 2);
            instruction += " $" + hex((high_byte << 8) | low_byte, 4) + ",Y";
            mode_str = "ABY";
        }
        else if (mode == &CPU6502::AM_IMM) {
            value = read(current_address++);
            opcodes += " " + hex(value, 2);
            instruction += " #$" + hex(value, 2);
            mode_str = "IMM";
        }
        else if (mode == &CPU6502::AM_IMP) {
            opcodes = hex(opcode, 2);
            mode_str = "IMP";
        }
        else if (mode == &CPU6502::AM_IND) {
            low_byte = read(current_address++);
            high_byte = read(current_address++);
            opcodes += " " + hex(low_byte, 2) + " " + hex(high_byte, 2);
            instruction += " ($" + hex((high_byte << 8) | low_byte, 4) + ")";
            mode_str = "IND";
        }
        else if (mode == &CPU6502::AM_INX) {
            low_byte = read(current_address++);
            opcodes += " " + hex(low_byte, 2);
            instruction += " ($" + hex(low_byte, 2) + ",X)";
            mode_str = "INX";
        }
        else if (mode == &CPU6502::AM_INY) {
            low_byte = read(current_address++);
            opcodes += " " + hex(low_byte, 2);
            instruction += " ($" + hex(low_byte, 2) + ",Y)";
            mode_str= "INY";
        }
        else if (mode == &CPU6502::AM_REL) {
            value = read(current_address++);
            opcodes = hex(opcode, 2)  + " " + hex(value, 2);
            instruction += " $" + hex(current_address + (i8)value, 4);
            mode_str = "REL";
        }
        else if (mode == &CPU6502::AM_ZER) {
            low_byte = read(current_address++);
            opcodes += " " + hex(low_byte, 2);
            instruction += " $" + hex(low_byte, 2);
            mode_str = "ZER";
        }
        else if (mode == &CPU6502::AM_ZEX) {
            low_byte = read(current_address++);
            //opcodes = hex(opcode, 2);
            instruction += " $" + hex(low_byte, 2) + ",X";
            mode_str = "ZEX";
        }
        else if (mode == &CPU6502::AM_ZEY) {
            low_byte = read(current_address++);
            opcodes += " " + hex(low_byte, 2);
            instruction += " $" + hex(low_byte, 2) + ",Y";
            mode_str = "ZEY";
        }
        instruction_map[id] = {address, opcodes, instruction, mode_str};
    }
    m_disassembly = instruction_map;
    return instruction_map;
}
