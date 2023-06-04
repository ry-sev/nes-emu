use super::Addressable;
use std::ops::Range;

const RAM_SIZE: u16 = 0x800;
const RAM_RANGE: Range<u16> = 0x0000..0x1fff;

pub struct Ram {
	pub data: Vec<u8>,
}

impl Addressable for Ram {
	fn address_range(&self) -> &Range<u16> {
		&RAM_RANGE
	}

	fn write(&mut self, addr: u16, data: u8) {
		self.data[(addr & (RAM_SIZE - 1)) as usize] = data;
	}

	fn read(&mut self, addr: u16) -> u8 {
		self.data[(addr & (RAM_SIZE - 1)) as usize]
	}
}

impl Ram {
	pub fn new() -> Self {
		Self {
			data: vec![0; RAM_SIZE as usize],
		}
	}
}

impl Default for Ram {
	fn default() -> Self {
		Self::new()
	}
}
