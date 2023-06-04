use super::Bus;

#[derive(Default, Clone)]
pub struct Flags {
	pub carry: bool,     // Carry
	pub zero: bool,      // Zero
	pub interrupt: bool, // Interrupt
	pub decimal: bool,   // Decimal
	pub _break: bool,    // Break
	pub unused: bool,    // Unused
	pub overflow: bool,  // Overflow
	pub negative: bool,  // Negative
}

impl Flags {
	pub fn set_zn(&mut self, value: u8) {
		self.zero = value == 0;
		self.negative = value & 0x80 != 0;
	}

	pub fn byte(&self) -> u8 {
		(self.carry as u8)
			| (self.zero as u8) << 1
			| (self.interrupt as u8) << 2
			| (self.decimal as u8) << 3
			| (self._break as u8) << 4
			| (self.unused as u8) << 5
			| (self.overflow as u8) << 6
			| (self.negative as u8) << 7
	}

	pub fn set_byte(&mut self, byte: u8) {
		self.carry = ((byte) & 1) != 0;
		self.zero = ((byte >> 1) & 1) != 0;
		self.interrupt = ((byte >> 2) & 1) != 0;
		self.decimal = ((byte >> 3) & 1) != 0;
		self.overflow = ((byte >> 6) & 1) != 0;
		self.negative = ((byte >> 7) & 1) != 0;
	}
}

#[derive(PartialEq, Eq)]
#[allow(clippy::upper_case_acronyms)]
pub enum AddressingMode {
	ABS,
	ABX,
	ABY,
	ACC,
	IMM,
	IMP,
	IND,
	INX,
	INY,
	REL,
	ZPG,
	ZPX,
	ZPY,
}

#[derive(PartialEq, Eq)]
#[allow(clippy::upper_case_acronyms)]
pub enum Opcode {
	ADC,
	AND,
	ASL,
	BCC,
	BCS,
	BEQ,
	BIT,
	BMI,
	BNE,
	BPL,
	BRK,
	BVC,
	BVS,
	CLC,
	CLD,
	CLI,
	CLV,
	CMP,
	CPX,
	CPY,
	DEC,
	DEX,
	DEY,
	EOR,
	ERR,
	INC,
	INX,
	INY,
	JMP,
	JSR,
	LDA,
	LDX,
	LDY,
	LSR,
	NOP,
	ORA,
	PHA,
	PHP,
	PLA,
	PLP,
	ROL,
	ROR,
	RTI,
	RTS,
	SBC,
	SEC,
	SED,
	SEI,
	STA,
	STX,
	STY,
	TAX,
	TAY,
	TSX,
	TXA,
	TXS,
	TYA,
}

pub struct Instruction {
	pub opcode: Opcode,
	pub mode: AddressingMode,
	pub cycles: u8,
}

#[rustfmt::skip]
pub const INSTRUCTION_TABLE: [Instruction; 256] = [
	// 0
	Instruction { opcode: Opcode::BRK, mode: AddressingMode::IMP, cycles: 7 },
	Instruction { opcode: Opcode::ORA, mode: AddressingMode::INX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 3 },
	Instruction { opcode: Opcode::ORA, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::ASL, mode: AddressingMode::ZPG, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 5 },
	Instruction { opcode: Opcode::PHP, mode: AddressingMode::IMP, cycles: 3 },
	Instruction { opcode: Opcode::ORA, mode: AddressingMode::IMM, cycles: 2 },
	Instruction { opcode: Opcode::ASL, mode: AddressingMode::ACC, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::ORA, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::ASL, mode: AddressingMode::ABS, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	// 1
	Instruction { opcode: Opcode::BPL, mode: AddressingMode::REL, cycles: 2 },
	Instruction { opcode: Opcode::ORA, mode: AddressingMode::INY, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::ORA, mode: AddressingMode::ZPX, cycles: 4 },
	Instruction { opcode: Opcode::ASL, mode: AddressingMode::ZPX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	Instruction { opcode: Opcode::CLC, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ORA, mode: AddressingMode::ABY, cycles: 4 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::ORA, mode: AddressingMode::ABX, cycles: 4 },
	Instruction { opcode: Opcode::ASL, mode: AddressingMode::ABX, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
	// 2
	Instruction { opcode: Opcode::JSR, mode: AddressingMode::ABS, cycles: 6 },
	Instruction { opcode: Opcode::AND, mode: AddressingMode::INX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::BIT, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::AND, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::ROL, mode: AddressingMode::ZPG, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 5 },
	Instruction { opcode: Opcode::PLP, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::AND, mode: AddressingMode::IMM, cycles: 2 },
	Instruction { opcode: Opcode::ROL, mode: AddressingMode::ACC, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::BIT, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::AND, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::ROL, mode: AddressingMode::ABS, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	// 3
	Instruction { opcode: Opcode::BMI, mode: AddressingMode::REL, cycles: 2 },
	Instruction { opcode: Opcode::AND, mode: AddressingMode::INY, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::AND, mode: AddressingMode::ZPX, cycles: 4 },
	Instruction { opcode: Opcode::ROL, mode: AddressingMode::ZPX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	Instruction { opcode: Opcode::SEC, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::AND, mode: AddressingMode::ABY, cycles: 4 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::AND, mode: AddressingMode::ABX, cycles: 4 },
	Instruction { opcode: Opcode::ROL, mode: AddressingMode::ABX, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
	// 4
	Instruction { opcode: Opcode::RTI, mode: AddressingMode::IMP, cycles: 6 },
	Instruction { opcode: Opcode::EOR, mode: AddressingMode::INX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 3 },
	Instruction { opcode: Opcode::EOR, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::LSR, mode: AddressingMode::ZPG, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 5 },
	Instruction { opcode: Opcode::PHA, mode: AddressingMode::IMP, cycles: 3 },
	Instruction { opcode: Opcode::EOR, mode: AddressingMode::IMM, cycles: 2 },
	Instruction { opcode: Opcode::LSR, mode: AddressingMode::ACC, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::JMP, mode: AddressingMode::ABS, cycles: 3 },
	Instruction { opcode: Opcode::EOR, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::LSR, mode: AddressingMode::ABS, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	// 5
	Instruction { opcode: Opcode::BVC, mode: AddressingMode::REL, cycles: 2 },
	Instruction { opcode: Opcode::EOR, mode: AddressingMode::INY, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::EOR, mode: AddressingMode::ZPX, cycles: 4 },
	Instruction { opcode: Opcode::LSR, mode: AddressingMode::ZPX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	Instruction { opcode: Opcode::CLI, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::EOR, mode: AddressingMode::ABY, cycles: 4 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::EOR, mode: AddressingMode::ABX, cycles: 4 },
	Instruction { opcode: Opcode::LSR, mode: AddressingMode::ABX, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
	// 6
	Instruction { opcode: Opcode::RTS, mode: AddressingMode::IMP, cycles: 6 },
	Instruction { opcode: Opcode::ADC, mode: AddressingMode::INX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 3 },
	Instruction { opcode: Opcode::ADC, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::ROR, mode: AddressingMode::ZPG, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 5 },
	Instruction { opcode: Opcode::PLA, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::ADC, mode: AddressingMode::IMM, cycles: 2 },
	Instruction { opcode: Opcode::ROR, mode: AddressingMode::ACC, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::JMP, mode: AddressingMode::IND, cycles: 5 },
	Instruction { opcode: Opcode::ADC, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::ROR, mode: AddressingMode::ABS, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	// 7
	Instruction { opcode: Opcode::BVS, mode: AddressingMode::REL, cycles: 2 },
	Instruction { opcode: Opcode::ADC, mode: AddressingMode::INY, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::ADC, mode: AddressingMode::ZPX, cycles: 4 },
	Instruction { opcode: Opcode::ROR, mode: AddressingMode::ZPX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	Instruction { opcode: Opcode::SEI, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ADC, mode: AddressingMode::ABY, cycles: 4 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::ADC, mode: AddressingMode::ABX, cycles: 4 },
	Instruction { opcode: Opcode::ROR, mode: AddressingMode::ABX, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
	// 8
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::STA, mode: AddressingMode::INX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	Instruction { opcode: Opcode::STY, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::STA, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::STX, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 3 },
	Instruction { opcode: Opcode::DEY, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::TXA, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::STY, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::STA, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::STX, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	// 9
	Instruction { opcode: Opcode::BCC, mode: AddressingMode::REL, cycles: 2 },
	Instruction { opcode: Opcode::STA, mode: AddressingMode::INY, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	Instruction { opcode: Opcode::STY, mode: AddressingMode::ZPX, cycles: 4 },
	Instruction { opcode: Opcode::STA, mode: AddressingMode::ZPX, cycles: 4 },
	Instruction { opcode: Opcode::STX, mode: AddressingMode::ZPY, cycles: 4 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::TYA, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::STA, mode: AddressingMode::ABY, cycles: 5 },
	Instruction { opcode: Opcode::TXS, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 5 },
	Instruction { opcode: Opcode::STA, mode: AddressingMode::ABX, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 5 },
	// A
	Instruction { opcode: Opcode::LDY, mode: AddressingMode::IMM, cycles: 2 },
	Instruction { opcode: Opcode::LDA, mode: AddressingMode::INX, cycles: 6 },
	Instruction { opcode: Opcode::LDX, mode: AddressingMode::IMM, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	Instruction { opcode: Opcode::LDY, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::LDA, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::LDX, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 3 },
	Instruction { opcode: Opcode::TAY, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::LDA, mode: AddressingMode::IMM, cycles: 2 },
	Instruction { opcode: Opcode::TAX, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::LDY, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::LDA, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::LDX, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	// B
	Instruction { opcode: Opcode::BCS, mode: AddressingMode::REL, cycles: 2 },
	Instruction { opcode: Opcode::LDA, mode: AddressingMode::INY, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 5 },
	Instruction { opcode: Opcode::LDY, mode: AddressingMode::ZPX, cycles: 4 },
	Instruction { opcode: Opcode::LDA, mode: AddressingMode::ZPX, cycles: 4 },
	Instruction { opcode: Opcode::LDX, mode: AddressingMode::ZPY, cycles: 4 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::CLV, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::LDA, mode: AddressingMode::ABY, cycles: 4 },
	Instruction { opcode: Opcode::TSX, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::LDY, mode: AddressingMode::ABX, cycles: 4 },
	Instruction { opcode: Opcode::LDA, mode: AddressingMode::ABX, cycles: 4 },
	Instruction { opcode: Opcode::LDX, mode: AddressingMode::ABY, cycles: 4 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	// C
	Instruction { opcode: Opcode::CPY, mode: AddressingMode::IMM, cycles: 2 },
	Instruction { opcode: Opcode::CMP, mode: AddressingMode::INX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::CPY, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::CMP, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::DEC, mode: AddressingMode::ZPG, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 5 },
	Instruction { opcode: Opcode::INY, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::CMP, mode: AddressingMode::IMM, cycles: 2 },
	Instruction { opcode: Opcode::DEX, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::CPY, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::CMP, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::DEC, mode: AddressingMode::ABS, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	// D
	Instruction { opcode: Opcode::BNE, mode: AddressingMode::REL, cycles: 2 },
	Instruction { opcode: Opcode::CMP, mode: AddressingMode::INY, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::CMP, mode: AddressingMode::ZPX, cycles: 4 },
	Instruction { opcode: Opcode::DEC, mode: AddressingMode::ZPX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	Instruction { opcode: Opcode::CLD, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::CMP, mode: AddressingMode::ABY, cycles: 4 },
	Instruction { opcode: Opcode::NOP, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::CMP, mode: AddressingMode::ABX, cycles: 4 },
	Instruction { opcode: Opcode::DEC, mode: AddressingMode::ABX, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
	// E
	Instruction { opcode: Opcode::CPX, mode: AddressingMode::IMM, cycles: 2 },
	Instruction { opcode: Opcode::SBC, mode: AddressingMode::INX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::CPX, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::SBC, mode: AddressingMode::ZPG, cycles: 3 },
	Instruction { opcode: Opcode::INC, mode: AddressingMode::ZPG, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 5 },
	Instruction { opcode: Opcode::INX, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::SBC, mode: AddressingMode::IMM, cycles: 2 },
	Instruction { opcode: Opcode::NOP, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::CPX, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::SBC, mode: AddressingMode::ABS, cycles: 4 },
	Instruction { opcode: Opcode::INC, mode: AddressingMode::ABS, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	// F
	Instruction { opcode: Opcode::BEQ, mode: AddressingMode::REL, cycles: 2 },
	Instruction { opcode: Opcode::SBC, mode: AddressingMode::INY, cycles: 5 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 8 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::SBC, mode: AddressingMode::ZPX, cycles: 4 },
	Instruction { opcode: Opcode::INC, mode: AddressingMode::ZPX, cycles: 6 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 6 },
	Instruction { opcode: Opcode::SED, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::SBC, mode: AddressingMode::ABY, cycles: 4 },
	Instruction { opcode: Opcode::NOP, mode: AddressingMode::IMP, cycles: 2 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 4 },
	Instruction { opcode: Opcode::SBC, mode: AddressingMode::ABX, cycles: 4 },
	Instruction { opcode: Opcode::INC, mode: AddressingMode::ABX, cycles: 7 },
	Instruction { opcode: Opcode::ERR, mode: AddressingMode::IMP, cycles: 7 },
];

#[derive(Default)]
pub struct Cpu {
	pub bus: Bus,
	flags: Flags,
	a: u8,
	x: u8,
	y: u8,
	pc: u16,
	s: u8,
	cycles: u8,
	total_cycles: usize,
}

impl Cpu {
	#[allow(dead_code)]
	pub fn new(bus: Bus) -> Self {
		Self {
			bus,
			..Default::default()
		}
	}

	pub fn reset(&mut self) {
		self.flags = Flags::default();
		self.flags.unused = true;
		self.flags.interrupt = true;
		self.a = 0;
		self.x = 0;
		self.y = 0;
		self.s = 0xfd;
		self.pc = self.read_u16(0xfffc);
		self.cycles = 7;
		self.total_cycles = 0;
	}

	pub fn nmi(&mut self) {
		self.push_u16(self.pc);

		self.flags._break = false;
		self.flags.unused = true;
		self.flags.interrupt = true;

		self.push_flags();

		self.pc = self.read_u16(0xfffa);

		self.cycles = 8;
	}

	pub fn clock(&mut self) {
		self.total_cycles += 1;

		if self.cycles > 0 {
			self.cycles -= 1;
			return;
		}

		let opcode = self.bus.read(self.pc);
		self.pc += 1;

		let instruction = &INSTRUCTION_TABLE[opcode as usize];

		self.cycles = instruction.cycles;

		let address = self.address_from_addressing_mode(instruction);

		match instruction.opcode {
			Opcode::ADC => {
				let value = self.bus.read(address) as u16;
				let temp = (self.a as u16) + value + (self.flags.carry as u16);
				self.flags.carry = temp > 0xff;
				self.flags.overflow = !((self.a as u16) ^ value)
					& ((self.a as u16) ^ temp)
					& 0x80 != 0;
				self.a = temp as u8;
				self.flags.set_zn(self.a);
			}

			Opcode::AND => {
				let value = self.bus.read(address);
				self.a &= value;
				self.flags.set_zn(self.a);
			}

			Opcode::ASL => {
				let value = if instruction.mode == AddressingMode::ACC {
					address
				} else {
					self.bus.read(address) as u16
				};

				let temp = (value << 1) as u8;

				self.flags.carry = (value & 0x80) != 0;
				self.flags.set_zn(temp);

				if instruction.mode == AddressingMode::ACC {
					self.a = temp;
				} else {
					self.bus.write(address, temp);
				}
			}

			Opcode::BCC => {
				if !self.flags.carry {
					self.branch(address);
				}
			}

			Opcode::BCS => {
				if self.flags.carry {
					self.branch(address);
				}
			}

			Opcode::BEQ => {
				if self.flags.zero {
					self.branch(address);
				}
			}

			Opcode::BIT => {
				let value = self.bus.read(address);
				let result = self.a & value;

				self.flags.zero = result == 0;
				self.flags.overflow = (value & (1 << 6)) != 0;
				self.flags.negative = (value & (1 << 7)) != 0;
			}

			Opcode::BMI => {
				if self.flags.negative {
					self.branch(address);
				}
			}

			Opcode::BNE => {
				if !self.flags.zero {
					self.branch(address);
				}
			}

			Opcode::BPL => {
				if !self.flags.negative {
					self.branch(address);
				}
			}

			Opcode::BRK => {
				self.pc += 1;
				self.flags.interrupt = true;
				self.push_u16(self.pc);
				self.push_flags();
				self.pc = self.read_u16(0xfffe);
			}

			Opcode::BVC => {
				if !self.flags.overflow {
					self.branch(address);
				}
			}

			Opcode::BVS => {
				if self.flags.overflow {
					self.branch(address);
				}
			}

			Opcode::CLC => {
				self.flags.carry = false;
			}

			Opcode::CLD => {
				self.flags.decimal = false;
			}

			Opcode::CLI => {
				self.flags.interrupt = false;
			}

			Opcode::CLV => {
				self.flags.overflow = false;
			}

			Opcode::CMP => {
				let value = self.bus.read(address);
				self.flags.carry = self.a >= value;
				self.flags.set_zn(self.a.wrapping_sub(value));
			}

			Opcode::CPX => {
				let value = self.bus.read(address);
				self.flags.carry = self.x >= value;
				self.flags.set_zn(self.x.wrapping_sub(value));
			}

			Opcode::CPY => {
				let value = self.bus.read(address);
				self.flags.carry = self.y >= value;
				self.flags.set_zn(self.y.wrapping_sub(value));
			}

			Opcode::DEC => {
				let mut value = self.bus.read(address);
				value = value.wrapping_sub(1);
				self.flags.set_zn(value);
				self.bus.write(address, value);
			}

			Opcode::DEX => {
				self.x = self.x.wrapping_sub(1);
				self.flags.set_zn(self.x);
			}

			Opcode::DEY => {
				self.y = self.y.wrapping_sub(1);
				self.flags.set_zn(self.y);
			}

			Opcode::EOR => {
				let value = self.bus.read(address);
				self.a ^= value;
				self.flags.set_zn(self.a);
			}

			Opcode::ERR => {}

			Opcode::INC => {
				let mut value = self.bus.read(address);
				value = value.wrapping_add(1);
				self.bus.write(address, value);
				self.flags.set_zn(value);
			}

			Opcode::INX => {
				self.x = self.x.wrapping_add(1);
				self.flags.set_zn(self.x);
			}

			Opcode::INY => {
				self.y = self.y.wrapping_add(1);
				self.flags.set_zn(self.y);
			}

			Opcode::JMP => {
				self.pc = address;
			}

			Opcode::JSR => {
				self.pc -= 1;
				self.push_u16(self.pc);
				self.pc = address;
			}

			Opcode::LDA => {
				let value = self.bus.read(address);
				self.a = value;
				self.flags.set_zn(self.a);
			}

			Opcode::LDX => {
				let value = self.bus.read(address);
				self.x = value;
				self.flags.set_zn(self.x);
			}

			Opcode::LDY => {
				let value = self.bus.read(address);
				self.y = value;
				self.flags.set_zn(self.y);
			}

			Opcode::LSR => {
				let value = if instruction.mode == AddressingMode::ACC {
					address
				} else {
					self.bus.read(address) as u16
				};

				let temp = (value >> 1) as u8;

				self.flags.carry = (value & 0x0001) != 0;
				self.flags.set_zn(temp);

				if instruction.mode == AddressingMode::ACC {
					self.a = temp;
				} else {
					self.bus.write(address, temp);
				}
			}

			Opcode::NOP => {}

			Opcode::ORA => {
				let value = self.bus.read(address);
				self.a |= value;
				self.flags.set_zn(self.a);
			}

			Opcode::PHA => {
				self.push(self.a);
			}

			Opcode::PHP => {
				self.push_flags();
			}

			Opcode::PLA => {
				self.a = self.pop();
				self.flags.set_zn(self.a);
			}

			Opcode::PLP => {
				let flag = self.pop();
				self.flags.set_byte(flag);
			}

			Opcode::ROL => {
				let value = if instruction.mode == AddressingMode::ACC {
					address
				} else {
					self.bus.read(address) as u16
				};

				let temp = (value << 1) as u8 | (self.flags.carry as u8);

				self.flags.carry = (value & 0x80) != 0;
				self.flags.set_zn(temp);

				if instruction.mode == AddressingMode::ACC {
					self.a = temp;
				} else {
					self.bus.write(address, temp);
				}
			}

			Opcode::ROR => {
				let value = if instruction.mode == AddressingMode::ACC {
					address
				} else {
					self.bus.read(address) as u16
				};

				let temp = ((self.flags.carry as u8) << 7) | (value >> 1) as u8;

				self.flags.carry = (value & 0x0001) != 0;
				self.flags.set_zn(temp);

				if instruction.mode == AddressingMode::ACC {
					self.a = temp;
				} else {
					self.bus.write(address, temp);
				}
			}

			Opcode::RTI => {
				let flag = self.pop();
				self.flags.set_byte(flag);
				self.pc = self.pop_u16();
			}

			Opcode::RTS => {
				self.pc = self.pop_u16();
				self.pc += 1;
			}

			Opcode::SBC => {
				let mut value = self.bus.read(address) as u16;
				value ^= 0x00ff;
				let temp: u16 = (self.a as u16) + value + (self.flags.carry as u16);
				self.flags.carry = temp > 0xff;
				self.flags.overflow = !((self.a as u16) ^ value)
					& ((self.a as u16) ^ temp)
					& 0x80 != 0;
				self.a = temp as u8;
				self.flags.set_zn(self.a);
			}

			Opcode::SEC => {
				self.flags.carry = true;
			}

			Opcode::SED => {
				self.flags.decimal = true;
			}

			Opcode::SEI => {
				self.flags.interrupt = true;
			}

			Opcode::STA => {
				self.bus.write(address, self.a);
			}

			Opcode::STX => {
				self.bus.write(address, self.x);
			}

			Opcode::STY => {
				self.bus.write(address, self.y);
			}

			Opcode::TAX => {
				self.x = self.a;
				self.flags.set_zn(self.x);
			}

			Opcode::TAY => {
				self.y = self.a;
				self.flags.set_zn(self.y);
			}

			Opcode::TSX => {
				self.x = self.s;
				self.flags.set_zn(self.x);
			}

			Opcode::TXA => {
				self.a = self.x;
				self.flags.set_zn(self.a);
			}

			Opcode::TXS => {
				self.s = self.x;
			}

			Opcode::TYA => {
				self.a = self.y;
				self.flags.set_zn(self.a);
			}
		}
		self.cycles -= 1;
	}

	fn address_from_addressing_mode(&mut self, instruction: &Instruction) -> u16 {
		match instruction.mode {
			AddressingMode::ABS => self.read_u16_from_pc(),

			AddressingMode::ABX => {
				let base = self.read_u16_from_pc();
				let address = base.wrapping_add(self.x as u16);
				if instruction.opcode != Opcode::STA
					&& address & 0xff00 != base & 0xff00
				{
					self.cycles += 1;
				}
				address
			}

			AddressingMode::ABY => {
				let base = self.read_u16_from_pc();
				let address = base.wrapping_add(self.y as u16);
				if instruction.opcode != Opcode::STA
					&& address & 0xff00 != base & 0xff00
				{
					self.cycles += 1;
				}
				address
			}

			AddressingMode::ACC => self.a as u16,

			AddressingMode::IMM => {
				let address = self.pc;
				self.pc += 1;
				address
			}

			AddressingMode::IMP => 0x00,

			AddressingMode::IND => {
				let address = self.read_u16_from_pc();
				if address & 0x00ff == 0x00ff {
					to_u16(self.bus.read(address), self.bus.read(address & 0xff00))
				} else {
					to_u16(self.bus.read(address), self.bus.read(address + 1))
				}
			}

			AddressingMode::INX => {
				let mut address = self.bus.read(self.pc);
				self.pc += 1;
				address = address.wrapping_add(self.x);
				self.read_u8(address)
			}

			AddressingMode::INY => {
				let value = self.bus.read(self.pc);
				self.pc += 1;

				let x = self.read_u8(value);

				let address = x.wrapping_add(self.y as u16);
				if address & 0xff00 != x & 0xff00 {
					self.cycles += 1;
				}
				address
			}

			AddressingMode::REL => {
				let offset = self.bus.read(self.pc);
				self.pc += 1;
				((self.pc as i16) + (offset as i8) as i16) as u16
			}

			AddressingMode::ZPG => {
				let address = self.bus.read(self.pc);
				self.pc += 1;
				address as u16
			}

			AddressingMode::ZPX => {
				let mut address = (self.bus.read(self.pc) as u16) + (self.x as u16);
				self.pc += 1;
				address &= 0x00ff;
				address
			}

			AddressingMode::ZPY => {
				let mut address = (self.bus.read(self.pc) as u16) + (self.y as u16);
				self.pc += 1;
				address &= 0x00ff;
				address
			}
		}
	}

	fn branch(&mut self, address: u16) {
		self.cycles += 1;
		if (address & 0xff00) != (self.pc & 0xff00) {
			self.cycles += 1;
		}
		self.pc = address;
	}

	fn push(&mut self, data: u8) {
		self.bus.write(0x0100 + self.s as u16, data);
		self.s -= 1;
	}

	fn push_u16(&mut self, data: u16) {
		self.push((data >> 8) as u8);
		self.push((data & 0x00ff) as u8);
	}

	fn push_flags(&mut self) {
		let mut flag_state = self.flags.byte();
		flag_state |= 1 << 5;
		flag_state |= 1 << 4;
		self.push(flag_state);
	}

	fn pop(&mut self) -> u8 {
		self.s += 1;
		self.bus.read(0x0100 + self.s as u16)
	}

	fn pop_u16(&mut self) -> u16 {
		let low_byte = self.pop();
		let high_byte = self.pop();
		to_u16(low_byte, high_byte)
	}

	fn read_u16_from_pc(&mut self) -> u16 {
		let low_byte = self.bus.read(self.pc);
		self.pc += 1;
		let high_byte = self.bus.read(self.pc);
		self.pc += 1;
		to_u16(low_byte, high_byte)
	}

	fn read_u8(&mut self, address: u8) -> u16 {
		to_u16(
			self.bus.read(address as u16),
			self.bus.read(address.wrapping_add(1) as u16),
		)
	}

	fn read_u16(&mut self, address: u16) -> u16 {
		to_u16(
			self.bus.read(address),
			self.bus.read(address.wrapping_add(1)),
		)
	}
}

pub fn to_u16(low_byte: u8, high_byte: u8) -> u16 {
	(high_byte as u16) << 8 | (low_byte as u16)
}
