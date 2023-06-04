use super::{
	mappers::{Mapper, Mapper000, Mapper003},
	Addressable,
};
use anyhow::{anyhow, Result};
use std::{fs, ops::Range, path::PathBuf};

pub struct Cartridge {
	pub program_rom: Vec<u8>,
	pub character_rom: Vec<u8>,
	pub program_ram: Vec<u8>,
	pub mapper: Box<dyn Mapper>,
	pub valid: bool,
	pub mirror: Mirror,
}

#[derive(Debug, PartialEq, Eq)]
pub enum Mirror {
	None,
	Horizontal,
	Vertical,
}

impl Addressable for Cartridge {
	fn address_range(&self) -> &Range<u16> {
		&(0x8000..0xffff)
	}

	fn read(&mut self, address: u16) -> u8 {
		let mapped_address = self.mapper.cpu_read(address);
		self.program_rom[(mapped_address as usize)]
	}

	fn write(&mut self, address: u16, data: u8) {
		self.mapper.cpu_write(address, data);
	}
}

impl Cartridge {
	pub fn new() -> Self {
		Self {
			program_rom: vec![],
			character_rom: vec![],
			program_ram: vec![],
			mapper: Box::new(Mapper000::new(0)),
			valid: false,
			mirror: Mirror::None,
		}
	}

	pub fn ppu_read(&mut self, address: u16) -> u8 {
		let mapped_address = self.mapper.ppu_read(address) as usize;
		if mapped_address >= self.character_rom.len() {
			0x00
		} else {
			self.character_rom[mapped_address]
		}
	}

	pub fn ppu_write(&mut self, address: u16, data: u8) {
		let mapped_address = self.mapper.ppu_write(address);
		self.character_rom[(mapped_address as usize)] = data;
	}

	pub fn insert(&mut self, file: &PathBuf) -> Result<()> {
		let data = fs::read(file)?;

		let mut byte_position = 0;

		if data[0..4] != [0x4e, 0x45, 0x53, 0x1a] {
			return Err(anyhow!(
				"Invalid Cartridge: NES magic bytes not found => {}",
				file.display()
			));
		}

		let prg_size = data[4];
		let chr_size = data[5];
		let mapper_1 = data[6];
		let mapper_2 = data[7];
		let mapper_number = (mapper_1 >> 4) & 0xf | (mapper_2 & 0xf0);

		self.mirror = if (mapper_1 & 0x01) != 0 {
			Mirror::Vertical
		} else {
			Mirror::Horizontal
		};

		let prg_ram_size = data[8];

		byte_position += 16;

		if mapper_1 & 0x04 != 0 {
			byte_position += 512;
		}

		{
			let size = (prg_size as usize) * 0x4000;
			self.program_rom = data[byte_position..byte_position + size].to_vec();
			byte_position += size;
		}

		{
			let size = (chr_size as usize) * 0x2000;
			self.character_rom = data[byte_position..byte_position + size].to_vec();
		}

		{
			let ram_size = (prg_ram_size as usize) * 0x204c;
			self.program_ram = vec![0; ram_size];
		}

		match mapper_number {
			0 => self.mapper = Box::new(Mapper000::new(prg_size)),
			3 => self.mapper = Box::new(Mapper003::new(prg_size)),
			_ => {
				return Err(anyhow!(
					"Invalid Cartridge: No support for Mapper{}",
					mapper_number
				));
			}
		}
		self.valid = true;

		Ok(())
	}
}

impl Default for Cartridge {
	fn default() -> Self {
		Self::new()
	}
}
