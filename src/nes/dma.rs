use super::{Addressable, Cpu, Ppu};
use std::ops::Range;

pub struct DmaDevice {
	pub page: u8,
	pub address: u8,
	pub data: u8,
	pub flag: bool,
	pub transfer: bool,
}

impl DmaDevice {
	pub fn new() -> Self {
		Self {
			page: 0,
			address: 0,
			data: 0,
			flag: true,
			transfer: false,
		}
	}

	pub fn reset(&mut self) {
		*self = Self::new();
	}

	pub fn clock(&mut self, clock: i32, cpu: &mut Cpu, ppu: &mut Ppu) {
		if self.flag {
			if clock % 2 == 1 {
				self.flag = false;
			}
		} else if clock % 2 == 0 {
			self.data = cpu
				.bus
				.read((self.page as u16) << 8 | (self.address as u16));
		} else {
			ppu.write_oam(self.address, self.data);
			self.address = self.address.wrapping_add(1);
			if self.address == 0x00 {
				self.transfer = false;
				self.flag = true;
			}
		}
	}
}

impl Addressable for DmaDevice {
	fn address_range(&self) -> &Range<u16> {
		&(0x4014..0x4015)
	}

	fn write(&mut self, _data: u16, data: u8) {
		self.page = data;
		self.address = 0x00;
		self.transfer = true;
	}

	fn read(&mut self, _address: u16) -> u8 {
		0
	}
}

impl Default for DmaDevice {
	fn default() -> Self {
		Self::new()
	}
}
