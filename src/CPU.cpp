#include "CPU.h"
#include "Bus.h"
#include <iostream>

CPU6502::CPU6502()
{
    //reset();
}

void CPU6502::reset()
{
    m_registers = {};
    m_current_address = 0xFFFC;
    u16 low_byte = read(m_current_address);
    u16 high_byte = read(m_current_address + 1);

    m_registers.pc = (high_byte << 8) | low_byte;

    set_flag(U, 1);
    m_registers.s = 0xFD;
    m_registers.p = 0x00 | U;

    m_current_address = 0x0000;
    m_offset = 0x0000;
    m_current_value = 0x00;

    m_cycles = 8;
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
    m_current_address = m_registers.pc + m_offset;

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
    auto value = m_registers.a + m_current_value + get_flag(C);

    set_flag(C, value > 255);
    set_flag(Z, (value & 0x00FF) == 0x00);
    set_flag(V, (~(m_registers.a ^ value) & (m_registers.a ^ value)) & 0x80);
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
        case Indirect_X: m_cycles++; break;
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
    auto value = m_current_value << 1;

    set_flag(Z, (value & 0x00FF) == 0x00);
    set_flag(C, (value & 0xFF00) > 0);
    set_flag(N, (value & 0x80));

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
    auto result = m_registers.a & m_current_address;

    set_flag(Z, result == 0x00);
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

    write(0x0100 + m_registers.s, m_registers.s);
    m_registers.s--;

    m_registers.pc = read(0xFFFE) | (read(0xFFFF) << 8);

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

    set_flag(C, m_registers.a >= m_current_value);
    set_flag(Z, m_registers.a == m_current_value);
    set_flag(N, m_current_value & 0x80);

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

    set_flag(C, m_registers.x >= m_current_value);
    set_flag(Z, m_registers.x == m_current_value);
    set_flag(N, m_registers.x & 0x80);

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

    set_flag(C, m_registers.y >= m_current_value);
    set_flag(Z, m_registers.y == m_current_value);
    set_flag(N, m_registers.y & 0x80);

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
    auto value = m_current_address - 0x01;

    write(m_current_address, value & 0x00FF);

    set_flag(C, value == 0x00);
    set_flag(N, value & 0x80);

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
    auto value = m_current_address + 0x01;

    write(m_current_address, value & 0x00FF);

    set_flag(Z, value == 0x00);
    set_flag(N, value & 0x80);
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
    set_flag(Z, (value & 0x00FF) == 0x00);
    set_flag(N, (value & 0x80));

    if (m_addressing_mode == Implied)
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
    write(0x0100 + m_registers.s, m_registers.p);
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
    m_registers.p = read(0x0100 + m_registers.s);

    m_cycles += 4;
}

void CPU6502::ROL()
{
    m_current_value = read(m_current_address);
    auto value = (m_current_value << 1) | get_flag(C);

    set_flag(C, value & 0xFF00);
    set_flag(Z, (value & 0x00FF) == 0x00);
    set_flag(N, value & 0x80);

    if (m_addressing_mode == Implied)
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
    auto value = (get_flag(C) << 7) | (m_current_value >> 1);

    set_flag(C, value & 0xFF00);
    set_flag(Z, (value & 0x00FF) == 0x00);
    set_flag(N, value & 0x80);

    if (m_addressing_mode == Implied)
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
    m_registers.pc = read(0x0100 + m_registers.s);
    m_registers.s++;

    m_registers.pc |= read(0x0100 + m_registers.s) << 8;

    m_cycles += 6;
}

void CPU6502::RTS()
{
    m_registers.s++;
    m_registers.pc = read(0x0100 + m_registers.s);
    m_registers.s++;
    m_registers.pc |= read(0x0100 + m_registers.s) << 8;

    m_registers.pc++;

    m_cycles += 6;
}

void CPU6502::SBC()
{
    m_current_value = read(m_current_address);

    auto inverted_value = m_current_value ^ 0x00FF;
    auto value = m_registers.a + inverted_value + get_flag(C);

    set_flag(C, value & 0xFF00);
    set_flag(Z, (value & 0x00FF) == 0x00);
    set_flag(V, (value ^ m_registers.a) & (value ^ inverted_value) & 0x80);
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

u8 CPU6502::read(u16 address)
{
    return m_bus->read(address);
}

void CPU6502::write(u16 address, u8 value)
{
    m_bus->write(address, value);
}

void  CPU6502::clock()
{
    u8 opcode = read(m_registers.pc++);

    switch (opcode) {
        case 0x00: { AM_IMM(); return BRK(); }
        case 0x01: { AM_INX(); return ORA(); }
        case 0x05: { AM_ZER(); return ORA(); }
        case 0x06: { AM_ZER(); return ASL(); }
        case 0x08: { AM_IMP(); return PHP(); }
        case 0x09: { AM_IMM(); return ORA(); }
        case 0x0A: { AM_ACC(); return ASL(); }
        case 0x0D: { AM_ABS(); return ORA(); }
        case 0x0E: { AM_ABS(); return ASL(); }
        case 0x10: { AM_REL(); return BPL(); }
        case 0x11: { AM_INY(); return ORA(); }
        case 0x15: { AM_ZEX(); return ASL(); }
        case 0x16: { AM_ZEX(); return ASL(); }
        case 0x18: { AM_IMP(); return CLC(); }
        case 0x19: { AM_ABY(); return ORA(); }
        case 0x1D: { AM_ABX(); return ORA(); }
        case 0x1E: { AM_ABX(); return ASL(); }
        case 0x20: { AM_ABS(); return JSR(); }
        case 0x21: { AM_INX(); return AND(); }
        case 0x24: { AM_ZER(); return BIT(); }
        case 0x25: { AM_ZER(); return AND(); }
        case 0x26: { AM_ZER(); return ROL(); }
        case 0x28: { AM_IMP(); return PLP(); }
        case 0x29: { AM_IMM(); return AND(); }
        case 0x2A: { AM_ACC(); return ROL(); }
        case 0x2C: { AM_ABS(); return BIT(); }
        case 0x2D: { AM_ABS(); return AND(); }
        case 0x2E: { AM_ABS(); return ROL(); }
        case 0x30: { AM_REL(); return BMI(); }
        case 0x31: { AM_INY(); return AND(); }
        case 0x35: { AM_ZEX(); return AND(); }
        case 0x36: { AM_ZEX(); return ROL(); }
        case 0x38: { AM_IMP(); return SEC(); }
        case 0x39: { AM_ABY(); return AND(); }
        case 0x3D: { AM_ABX(); return AND(); }
        case 0x3E: { AM_ABX(); return ROL(); }
        case 0x40: { AM_IMP(); return RTI(); }
        case 0x41: { AM_INX(); return EOR(); }
        case 0x45: { AM_ZER(); return EOR(); }
        case 0x46: { AM_ZER(); return LSR(); }
        case 0x48: { AM_IMP(); return PHA(); }
        case 0x49: { AM_IMM(); return EOR(); }
        case 0x4A: { AM_ACC(); return LSR(); }
        case 0x4C: { AM_ABS(); return JMP(); }
        case 0x4D: { AM_ABS(); return EOR(); }
        case 0x4E: { AM_ABS(); return LSR(); }
        case 0x50: { AM_REL(); return BVC(); }
        case 0x51: { AM_INY(); return EOR(); }
        case 0x55: { AM_ZEX(); return EOR(); }
        case 0x56: { AM_ZEX(); return LSR(); }
        case 0x58: { AM_IMP(); return CLI(); }
        case 0x59: { AM_ABY(); return EOR(); }
        case 0x5D: { AM_ABX(); return EOR(); }
        case 0x5E: { AM_ABX(); return LSR(); }
        case 0x60: { AM_IMP(); return RTS(); }
        case 0x61: { AM_INX(); return ADC(); }
        case 0x65: { AM_ZER(); return ADC(); }
        case 0x66: { AM_ZER(); return ROR(); }
        case 0x68: { AM_IMP(); return PLA(); }
        case 0x69: { AM_IMM(); return ADC(); }
        case 0x6A: { AM_ACC(); return ROR(); }
        case 0x6C: { AM_IND(); return JMP(); }
        case 0x6D: { AM_ABS(); return ADC(); }
        case 0x6E: { AM_ABS(); return ROR(); }
        case 0x70: { AM_REL(); return BVS(); }
        case 0x71: { AM_INY(); return ADC(); }
        case 0x75: { AM_ZEX(); return ADC(); }
        case 0x76: { AM_ZEX(); return ROR(); }
        case 0x78: { AM_IMP(); return SEI(); }
        case 0x79: { AM_ABY(); return ADC(); }
        case 0x7D: { AM_ABX(); return ADC(); }
        case 0x7E: { AM_ABX(); return ROR(); }
        case 0x81: { AM_INX(); return STA(); }
        case 0x84: { AM_ZER(); return STY(); }
        case 0x85: { AM_ZER(); return STA(); }
        case 0x86: { AM_ZER(); return STX(); }
        case 0x88: { AM_IMP(); return DEY(); }
        case 0x8A: { AM_IMP(); return TXA(); }
        case 0x8C: { AM_ABS(); return STY(); }
        case 0x8D: { AM_ABS(); return STA(); }
        case 0x8E: { AM_ABS(); return STX(); }
        case 0x90: { AM_REL(); return BCC(); }
        case 0x91: { AM_INY(); return STA(); }
        case 0x94: { AM_ZEX(); return STY(); }
        case 0x95: { AM_ZEX(); return STA(); }
        case 0x96: { AM_ZEY(); return STX(); }
        case 0x98: { AM_IMP(); return TYA(); }
        case 0x99: { AM_ABY(); return STA(); }
        case 0x9A: { AM_IMM(); return TXS(); }
        case 0x9D: { AM_ABX(); return STA(); }
        case 0xA0: { AM_IMM(); return LDY(); }
        case 0xA1: { AM_INX(); return LDA(); }
        case 0xA2: { AM_IMM(); return LDX(); }
        case 0xA4: { AM_ZER(); return LDY(); }
        case 0xA5: { AM_ZER(); return LDA(); }
        case 0xA6: { AM_ZER(); return LDX(); }
        case 0xA8: { AM_IMP(); return TAY(); }
        case 0xA9: { AM_IMM(); return LDA(); }
        case 0xAA: { AM_IMM(); return TAX(); }
        case 0xAC: { AM_ABS(); return LDY(); }
        case 0xAD: { AM_ABS(); return LDA(); }
        case 0xAE: { AM_ABS(); return LDX(); }
        case 0xB0: { AM_REL(); return BCS(); }
        case 0xB1: { AM_INY(); return LDA(); }
        case 0xB4: { AM_ZEX(); return LDY(); }
        case 0xB5: { AM_ZEX(); return LDA(); }
        case 0xB6: { AM_ZEY(); return LDX(); }
        case 0xB8: { AM_IMP(); return CLV(); }
        case 0xB9: { AM_ABY(); return LDA(); }
        case 0xBA: { AM_IMM(); return TSX(); }
        case 0xBC: { AM_ABX(); return LDY(); }
        case 0xBD: { AM_ABX(); return LDA(); }
        case 0xBE: { AM_ABY(); return LDX(); }
        case 0xC0: { AM_IMM(); return CPY(); }
        case 0xC1: { AM_INX(); return CMP(); }
        case 0xC4: { AM_ZER(); return CPY(); }
        case 0xC5: { AM_ZER(); return CMP(); }
        case 0xC6: { AM_ZER(); return DEC(); }
        case 0xC8: { AM_IMP(); return INY(); }
        case 0xC9: { AM_IMM(); return CMP(); }
        case 0xCA: { AM_IMM(); return DEX(); }
        case 0xCC: { AM_ABS(); return CPY(); }
        case 0xCD: { AM_ABS(); return CMP(); }
        case 0xCE: { AM_ABS(); return DEC(); }
        case 0xD0: { AM_REL(); return BNE(); }
        case 0xD1: { AM_INY(); return CMP(); }
        case 0xD5: { AM_ZEX(); return CMP(); }
        case 0xD6: { AM_ZEX(); return DEC(); }
        case 0xD8: { AM_IMP(); return CLD(); }
        case 0xD9: { AM_ABY(); return CMP(); }
        case 0xDD: { AM_ABX(); return CMP(); }
        case 0xDE: { AM_ABX(); return DEC(); }
        case 0xE0: { AM_IMM(); return CPX(); }
        case 0xE1: { AM_INX(); return SBC(); }
        case 0xE4: { AM_ZER(); return CPX(); }
        case 0xE5: { AM_ZER(); return SBC(); }
        case 0xE6: { AM_ZER(); return INC(); }
        case 0xE8: { AM_IMP(); return INX(); }
        case 0xE9: { AM_IMM(); return SBC(); }
        case 0xEA: { AM_IMP(); return NOP(); }
        case 0xEC: { AM_ABS(); return CPX(); }
        case 0xED: { AM_ABS(); return SBC(); }
        case 0xEE: { AM_ABS(); return INC(); }
        case 0xF0: { AM_REL(); return BEQ(); }
        case 0xF1: { AM_INY(); return SBC(); }
        case 0xF5: { AM_ZEX(); return SBC(); }
        case 0xF6: { AM_ZEX(); return INC(); }
        case 0xF8: { AM_IMP(); return SED(); }
        case 0xF9: { AM_ABY(); return SBC(); }
        case 0xFD: { AM_ABX(); return SBC(); }
        case 0xFE: { AM_ABX(); return INC(); }
        default: break;
    }
}

void CPU6502::print_registers()
{
    dbgln("---------------------------------------------------");

    printf("$A:\t0x%x\n",  static_cast<unsigned>(m_registers.a));
    printf("$X:\t0x%x\n",  static_cast<unsigned>(m_registers.x));
    printf("$Y:\t0x%x\n",  static_cast<unsigned>(m_registers.y));
    printf("\n");
    printf("$Program Counter: 0x%x\n",  static_cast<unsigned>(m_registers.pc));
    printf("$Stack Pointer:\t  0x%x\n",  static_cast<unsigned>(m_registers.s));
    printf("$Status:\t  0x%x\n",  static_cast<unsigned>(m_registers.p));

    printf("\nC=%x", static_cast<unsigned>(get_flag(C)));
    printf(" Z=%x", static_cast<unsigned>(get_flag(Z)));
    printf(" I=%x", static_cast<unsigned>(get_flag(I)));
    printf(" D=%x", static_cast<unsigned>(get_flag(D)));
    printf(" B=%x", static_cast<unsigned>(get_flag(B)));
    printf(" O=%x", static_cast<unsigned>(get_flag(V)));
    printf(" N=%x\n", static_cast<unsigned>(get_flag(N)));

    dbgln("---------------------------------------------------");
}
