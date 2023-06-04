pub trait Mapper {
	fn cpu_write(&mut self, address: u16, data: u8) -> u16;
	fn cpu_read(&self, address: u16) -> u16;
	fn ppu_write(&mut self, address: u16) -> u16;
	fn ppu_read(&self, address: u16) -> u16;
}

pub struct Mapper000 {
	program_size: u8,
}

impl Mapper for Mapper000 {
	fn cpu_write(&mut self, address: u16, _data: u8) -> u16 {
		address
	}

	fn cpu_read(&self, address: u16) -> u16 {
		let mut mapped_address = address - 0x8000;
		if self.program_size == 1 {
			mapped_address &= 0x3fff;
		}
		mapped_address
	}

	fn ppu_write(&mut self, address: u16) -> u16 {
		address
	}

	fn ppu_read(&self, address: u16) -> u16 {
		address
	}
}

impl Mapper000 {
	pub fn new(program_size: u8) -> Self {
		Self { program_size }
	}
}

pub struct Mapper003 {
	program_size: u8,
	bank_select: u16,
}

impl Mapper for Mapper003 {
	fn cpu_write(&mut self, address: u16, data: u8) -> u16 {
		self.bank_select = (data & 0x03) as u16;
		address
	}

	fn cpu_read(&self, address: u16) -> u16 {
		let mut mapped_address = address - 0x8000;
		if self.program_size == 1 {
			mapped_address &= 0x3fff
		} else if self.program_size == 2 {
			mapped_address &= 0x7fff
		}
		mapped_address
	}

	fn ppu_write(&mut self, address: u16) -> u16 {
		address
	}

	fn ppu_read(&self, address: u16) -> u16 {
		address | (self.bank_select << 13)
	}
}

impl Mapper003 {
	pub fn new(program_size: u8) -> Self {
		Self {
			program_size,
			bank_select: 0,
		}
	}
}
