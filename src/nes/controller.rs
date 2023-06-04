use super::Addressable;
use std::ops::Range;

pub struct Controller {
	pub input: u8,
	pub state: u8,
}

impl Addressable for Controller {
	fn address_range(&self) -> &Range<u16> {
		&(0x4016..0x4017)
	}

	fn write(&mut self, _: u16, _: u8) {
		self.state = self.input;
	}

	fn read(&mut self, _: u16) -> u8 {
		let data = ((self.state & 0x80) > 0) as u8;
		self.state <<= 1;
		data
	}
}

impl Controller {
	pub fn new() -> Self {
		Self { input: 0, state: 0 }
	}
}

impl Default for Controller {
	fn default() -> Self {
		Self::new()
	}
}
