use super::{
	cartridge::{Cartridge, Mirror},
	Addressable,
};
use std::{cell::RefCell, ops::Range, rc::Rc};

pub const DISPLAY_WIDTH: usize = 256;
pub const DISPLAY_HEIGHT: usize = 240;

const PALETTE: [u32; 64] = [
	0x7c7c7c, 0x0000fc, 0x0000bc, 0x4428bc, 0x940084, 0xa80020, 0xa81000, 0x881400,
	0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000,
	0xbcbcbc, 0x0078f8, 0x0058f8, 0x6844fc, 0xd800cc, 0xe40058, 0xf83800, 0xe45c10,
	0xac7c00, 0x00b800, 0x00a800, 0x00a844, 0x008888, 0x000000, 0x000000, 0x000000,
	0xf8f8f8, 0x3cbcfc, 0x6888fc, 0x9878f8, 0xf878f8, 0xf85898, 0xf87858, 0xfca044,
	0xf8b800, 0xb8f818, 0x58d854, 0x58f898, 0x00e8d8, 0x787878, 0x000000, 0x000000,
	0xfcfcfc, 0xa4e4fc, 0xb8b8f8, 0xd8b8f8, 0xf8b8f8, 0xf8a4c0, 0xf0d0b0, 0xfce0a8,
	0xf8d878, 0xd8f878, 0xb8f8b8, 0xb8f8d8, 0x00fcfc, 0xf8d8f8, 0x000000, 0x000000,
];

pub struct Screen {
	buffer: Vec<u32>,
	pub complete: bool,
}

impl Screen {
	pub fn new() -> Self {
		Self {
			buffer: vec![0; DISPLAY_WIDTH * DISPLAY_HEIGHT],
			complete: true,
		}
	}
}

impl Default for Screen {
	fn default() -> Self {
		Self::new()
	}
}

impl Screen {
	pub fn set_pixel(&mut self, x: usize, y: usize, c: u32) {
		self.buffer[x + y * DISPLAY_WIDTH] = c;
	}

	pub fn get_pixel(&self, x: usize, y: usize) -> u32 {
		self.buffer[x + y * DISPLAY_WIDTH]
	}
}

#[derive(Default, Clone)]
pub struct CtrlReg {
	pub nametable_x: bool,        // 0
	pub nametable_y: bool,        // 1
	pub increment: bool,          // 2
	pub pattern_sprite: bool,     // 3
	pub pattern_background: bool, // 4
	pub is_wide_sprite: bool,     // 5
	pub master_slave: bool,       // 6
	pub generate_nmi: bool,       // 7
}

impl CtrlReg {
	pub fn set_byte(&mut self, b: u8) {
		self.nametable_x = (b & 1) != 0;
		self.nametable_y = ((b >> 1) & 1) != 0;
		self.increment = ((b >> 2) & 1) != 0;
		self.pattern_sprite = ((b >> 3) & 1) != 0;
		self.pattern_background = ((b >> 4) & 1) != 0;
		self.is_wide_sprite = ((b >> 5) & 1) != 0;
		self.master_slave = ((b >> 6) & 1) != 0;
		self.generate_nmi = ((b >> 7) & 1) != 0;
	}
}

#[derive(Default, Clone)]
pub struct MaskReg {
	pub grayscale: bool,            // 0
	pub show_background_left: bool, // 1
	pub show_sprites_left: bool,    // 2
	pub show_background: bool,      // 3
	pub show_sprites: bool,         // 4
	pub emphasize_red: bool,        // 5
	pub emphasize_green: bool,      // 6
	pub emphasize_blue: bool,       // 7
}

impl MaskReg {
	pub fn set_byte(&mut self, b: u8) {
		self.grayscale = (b & 1) != 0;
		self.show_background_left = ((b >> 1) & 1) != 0;
		self.show_sprites_left = ((b >> 2) & 1) != 0;
		self.show_background = ((b >> 3) & 1) != 0;
		self.show_sprites = ((b >> 4) & 1) != 0;
		self.emphasize_red = ((b >> 5) & 1) != 0;
		self.emphasize_green = ((b >> 6) & 1) != 0;
		self.emphasize_blue = ((b >> 7) & 1) != 0;
	}
}

#[derive(Default, Clone)]
pub struct StatusRegister {
	pub unused: u8,            // 5 bits
	pub sprite_overflow: bool, // 5
	pub sprite_zero_hit: bool, // 6
	pub vertical_blank: bool,  // 7
}

impl StatusRegister {
	pub fn to_byte(&self) -> u8 {
		self.unused
			| (self.sprite_overflow as u8) << 5
			| (self.sprite_zero_hit as u8) << 6
			| (self.vertical_blank as u8) << 7
	}
}

#[derive(Default, Clone)]
pub struct LoopyAddress {
	pub coarse_x: u8, // 5 bits
	pub coarse_y: u8, // 5 bits
	pub nametable_x: bool,
	pub nametable_y: bool,
	pub fine_y: u8, // 3 bits
	pub unused: bool,
}

impl LoopyAddress {
	pub fn set_data(&mut self, data: u16) {
		self.coarse_x = (data as u8) & 0b00011111;
		self.coarse_y = ((data >> 5) as u8) & 0b00011111;
		self.nametable_x = ((data >> 10) & 1) != 0;
		self.nametable_y = ((data >> 11) & 1) != 0;
		self.fine_y = ((data >> 12) as u8) & 0b00000111;
		self.unused = ((data >> 15) & 1) != 0;
	}

	pub fn to_data(&self) -> u16 {
		((self.coarse_x & 0b00011111) as u16)
			| ((self.coarse_y & 0b00011111) as u16) << 5
			| (self.nametable_x as u16) << 10
			| (self.nametable_y as u16) << 11
			| ((self.fine_y & 0b00000111) as u16) << 12
			| (self.unused as u16) << 15
	}
}

#[derive(Default, Clone, Copy)]
pub struct ObjectAttributeEntry {
	pub y: u8,
	pub id: u8,
	pub attr: u8,
	pub x: u8,
}

#[derive(Default, Clone, Copy)]
pub struct SpriteRenderState {
	pub scanline: [ObjectAttributeEntry; 8],
	count: u8,
	shifter_pattern_lo: [u8; 8],
	shifter_pattern_hi: [u8; 8],
}

impl SpriteRenderState {
	pub fn update_shifters(&mut self) {
		for i in 0..self.count as usize {
			if self.scanline[i].x > 0 {
				self.scanline[i].x -= 1;
			} else {
				self.shifter_pattern_lo[i] <<= 1;
				self.shifter_pattern_hi[i] <<= 1;
			}
		}
	}
}

#[derive(Default, Clone, Copy)]
pub struct BgRenderState {
	tile_id: u8,
	tile_attrib: u8,
	tile_lsb: u8,
	tile_msb: u8,
	shifter_pattern_lo: u16,
	shifter_pattern_hi: u16,
	shifter_attrib_lo: u16,
	shifter_attrib_hi: u16,
}

impl BgRenderState {
	pub fn load_shifters(&mut self) {
		self.shifter_pattern_lo =
			(self.shifter_pattern_lo & 0xff00) | (self.tile_lsb as u16);
		self.shifter_pattern_hi =
			(self.shifter_pattern_hi & 0xff00) | (self.tile_msb as u16);
		self.shifter_attrib_lo &= 0xff00;
		if self.tile_attrib & 0b01 != 0 {
			self.shifter_attrib_lo |= 0xff;
		}
		self.shifter_attrib_hi &= 0xff00;
		if self.tile_attrib & 0b10 != 0 {
			self.shifter_attrib_hi |= 0xff;
		}
	}

	pub fn update_shifters(&mut self) {
		self.shifter_pattern_lo <<= 1;
		self.shifter_pattern_hi <<= 1;
		self.shifter_attrib_lo <<= 1;
		self.shifter_attrib_hi <<= 1;
	}
}

pub struct Ppu {
	pub screen: Screen,
	cartridge: Rc<RefCell<Cartridge>>,
	pub cycle: i16,
	pub scanline: i16,
	odd_frame: bool,
	ctrl: CtrlReg,
	mask: MaskReg,
	status: StatusRegister,
	pub nmi: bool,
	oam_address: u8,
	oam_memory: [ObjectAttributeEntry; 64],
	name_table: [[u8; 1024]; 2],
	palette_table: [u8; 32],
	bg_state: BgRenderState,
	sprite_state: SpriteRenderState,
	sprite_zero_hit_possible: bool,
	sprite_zero_being_rendered: bool,
	loopy_latch: bool,
	ppu_data_buf: u8,
	vram_address: LoopyAddress,
	tram_address: LoopyAddress,
	fine_x: u8,
}

impl Ppu {
	pub fn new(cartridge: Rc<RefCell<Cartridge>>) -> Self {
		Self {
			screen: Screen::default(),
			cartridge,
			cycle: 0,
			scanline: 0,
			odd_frame: false,
			ctrl: CtrlReg::default(),
			mask: MaskReg::default(),
			status: StatusRegister::default(),
			nmi: false,
			oam_address: 0,
			oam_memory: [ObjectAttributeEntry::default(); 64],
			name_table: [[0; 1024]; 2],
			palette_table: [0; 32],
			bg_state: BgRenderState::default(),
			sprite_state: SpriteRenderState::default(),
			sprite_zero_hit_possible: false,
			sprite_zero_being_rendered: false,
			loopy_latch: false,
			ppu_data_buf: 0,
			vram_address: LoopyAddress::default(),
			tram_address: LoopyAddress::default(),
			fine_x: 0,
		}
	}

	pub fn reset(&mut self) {
		*self = Ppu::new(self.cartridge.clone());
	}

	pub fn ppu_write(&mut self, address: u16, data: u8) {
		let address = address & 0x3fff;
		if address < 0x2000 {
			self.cartridge.borrow_mut().ppu_write(address, data);
		} else if address <= 0x3eff {
			let address = address & 0x0fff;
			if self.cartridge.borrow_mut().mirror == Mirror::Vertical {
				if address <= 0x03ff || (0x0800..=0x0bff).contains(&address) {
					self.name_table[0][(address & 0x03ff) as usize] = data;
				} else {
					self.name_table[1][(address & 0x03ff) as usize] = data;
				}
			} else if address <= 0x07ff {
				self.name_table[0][(address & 0x03ff) as usize] = data;
			} else {
				self.name_table[1][(address & 0x03ff) as usize] = data;
			}
		} else if address <= 0x3fff {
			let mut address = address & 0x001f;
			address = {
				match address {
					0x0010 => 0x0000,
					0x0014 => 0x0004,
					0x0018 => 0x0008,
					0x001c => 0x000c,
					_ => address,
				}
			};
			self.palette_table[address as usize] = data;
		}
	}

	pub fn ppu_read(&mut self, address: u16) -> u8 {
		match address {
			0x0000..=0x1fff => self.cartridge.borrow_mut().ppu_read(address),
			0x2000..=0x3eff => {
				let address = address & 0x0fff;
				match self.cartridge.borrow_mut().mirror {
					Mirror::Horizontal => {
						let offset = address & 0x03ff;
						match address {
							0x0000..=0x07ff => self.name_table[0][offset as usize],
							0x0800..=0x0fff => self.name_table[1][offset as usize],
							_ => panic!("Invalid nametable read at 0x{:x}", address),
						}
					}
					Mirror::Vertical => {
						let offset = address & 0x03ff;
						match address {
							0x0000..=0x03ff => self.name_table[0][offset as usize],
							0x0400..=0x07ff => self.name_table[1][offset as usize],
							0x0800..=0x0bff => self.name_table[0][offset as usize],
							0x0c00..=0x0fff => self.name_table[1][offset as usize],
							_ => panic!("Invalid nametable read at 0x{:x}", address),
						}
					}
					_ => panic!("Unrecognized Mirror"),
				}
			}
			0x3f00..=0x3fff => self.palette_table[(address & 0x001f) as usize],
			_ => 0,
		}
	}

	pub fn write_oam(&mut self, address: u8, data: u8) {
		let index = (address / 4) as usize;
		match address % 4 {
			0 => {
				self.oam_memory[index].y = data;
			}
			1 => {
				self.oam_memory[index].id = data;
			}
			2 => {
				self.oam_memory[index].attr = data;
			}
			3 => {
				self.oam_memory[index].x = data;
			}
			_ => {}
		}
	}

	fn increment_vram_address(&mut self) {
		let mut reg = self.vram_address.to_data();
		reg = reg.wrapping_add(if self.ctrl.increment { 32 } else { 1 });
		self.vram_address.set_data(reg);
	}

	fn increment_scroll_x(&mut self) {
		if self.mask.show_background || self.mask.show_sprites {
			if self.vram_address.coarse_x == 31 {
				self.vram_address.coarse_x = 0;
				self.vram_address.nametable_x = !self.vram_address.nametable_x;
			} else {
				self.vram_address.coarse_x += 1;
			}
		}
	}

	fn increment_scroll_y(&mut self) {
		if self.mask.show_background || self.mask.show_sprites {
			if self.vram_address.fine_y < 7 {
				self.vram_address.fine_y += 1;
			} else {
				self.vram_address.fine_y = 0;
				if self.vram_address.coarse_y == 29 {
					self.vram_address.coarse_y = 0;
					self.vram_address.nametable_y = !self.vram_address.nametable_y;
				} else if self.vram_address.coarse_y == 31 {
					self.vram_address.coarse_y = 0;
				} else {
					self.vram_address.coarse_y += 1;
				}
			}
		}
	}

	fn transfer_address_x(&mut self) {
		if self.mask.show_background || self.mask.show_sprites {
			self.vram_address.nametable_x = self.tram_address.nametable_x;
			self.vram_address.coarse_x = self.tram_address.coarse_x;
		}
	}

	fn transfer_address_y(&mut self) {
		if self.mask.show_background || self.mask.show_sprites {
			self.vram_address.fine_y = self.tram_address.fine_y;
			self.vram_address.nametable_y = self.tram_address.nametable_y;
			self.vram_address.coarse_y = self.tram_address.coarse_y;
		}
	}

	fn update_shifters(&mut self) {
		if self.mask.show_background {
			self.bg_state.update_shifters();
		}

		if self.mask.show_sprites && self.cycle >= 1 && self.cycle < 258 {
			self.sprite_state.update_shifters();
		}
	}

	pub fn clock(&mut self) {
		if self.scanline >= -1 && self.scanline < 240 {
			if self.scanline == 0
				&& self.cycle == 0
				&& self.odd_frame
				&& (self.mask.show_background || self.mask.show_sprites)
			{
				self.cycle = 1;
			}

			if self.scanline == -1 && self.cycle == 1 {
				self.status.vertical_blank = false;
				self.status.sprite_overflow = false;
				self.status.sprite_zero_hit = false;
				for i in 0..8 {
					self.sprite_state.shifter_pattern_lo[i] = 0;
					self.sprite_state.shifter_pattern_hi[i] = 0;
				}
			}

			if (self.cycle >= 2 && self.cycle < 258)
				|| (self.cycle >= 321 && self.cycle < 338)
			{
				self.update_shifters();

				match (self.cycle - 1) % 8 {
					0 => {
						self.bg_state.load_shifters();

						self.bg_state.tile_id = self.ppu_read(
							0x2000 | (self.vram_address.to_data() & 0x0fff),
						);
					}
					2 => {
						self.bg_state.tile_attrib = self.ppu_read(
							0x23c0
								| ((self.vram_address.nametable_y as u16) << 11)
								| ((self.vram_address.nametable_x as u16) << 10)
								| (((self.vram_address.coarse_y as u16) >> 2) << 3)
								| ((self.vram_address.coarse_x as u16) >> 2),
						);

						if self.vram_address.coarse_y & 0x02 != 0 {
							self.bg_state.tile_attrib >>= 4;
						}
						if self.vram_address.coarse_x & 0x02 != 0 {
							self.bg_state.tile_attrib >>= 2;
						}
						self.bg_state.tile_attrib &= 0x03;
					}
					4 => {
						self.bg_state.tile_lsb = self.ppu_read(
							((self.ctrl.pattern_background as u16) << 12)
								+ ((self.bg_state.tile_id as u16) << 4)
								+ (self.vram_address.fine_y as u16),
						);
					}
					6 => {
						self.bg_state.tile_msb = self.ppu_read(
							((self.ctrl.pattern_background as u16) << 12)
								+ ((self.bg_state.tile_id as u16) << 4)
								+ ((self.vram_address.fine_y as u16) + 8),
						);
					}
					7 => {
						self.increment_scroll_x();
					}
					_ => {}
				}
			}

			if self.cycle == 256 {
				self.increment_scroll_y();
			}

			if self.cycle == 257 {
				self.bg_state.load_shifters();
				self.transfer_address_x();
			}

			if self.cycle == 338 || self.cycle == 340 {
				self.bg_state.tile_id =
					self.ppu_read(0x2000 | (self.vram_address.to_data() & 0x0fff));
			}

			if self.scanline == -1 && self.cycle >= 280 && self.cycle < 305 {
				self.transfer_address_y();
			}

			if self.cycle == 257 && self.scanline >= 0 {
				self.sprite_state.scanline = [ObjectAttributeEntry::default(); 8];
				self.sprite_state.count = 0;

				for i in 0..8 {
					self.sprite_state.shifter_pattern_lo[i] = 0;
					self.sprite_state.shifter_pattern_hi[i] = 0;
				}

				let mut oam_index: u8 = 0;
				self.sprite_zero_hit_possible = false;

				while oam_index < 64 && self.sprite_state.count < 9 {
					let diff = (self.scanline)
						- (self.oam_memory[oam_index as usize].y as i16);

					let sprite_size = if self.ctrl.is_wide_sprite { 16 } else { 8 };
					if diff >= 0 && diff < sprite_size && self.sprite_state.count < 8
					{
						if self.sprite_state.count < 8 {
							if oam_index == 0 {
								self.sprite_zero_hit_possible = true;
							}

							self.sprite_state.scanline
								[self.sprite_state.count as usize] = self.oam_memory[oam_index as usize];
						}
						self.sprite_state.count += 1;
					}
					oam_index += 1;
				}

				self.status.sprite_overflow = self.sprite_state.count >= 8;
			}

			if self.cycle == 340 {
				for i in 0..self.sprite_state.count as usize {
					let mut sprite_pattern_addr_lo: u16;

					let scan_id = self.sprite_state.scanline[i].id as u16;
					let scan_y =
						self.scanline - self.sprite_state.scanline[i].y as i16;

					if !self.ctrl.is_wide_sprite {
						// 8x8 sprite
						if self.sprite_state.scanline[i].attr & 0x80 == 0 {
							sprite_pattern_addr_lo = ((self.ctrl.pattern_sprite
								as u16) << 12) | (scan_id << 4)
								| (scan_y as u16);
						} else {
							sprite_pattern_addr_lo = ((self.ctrl.pattern_sprite
								as u16) << 12) | (scan_id << 4)
								| ((7 - scan_y) as u16);
						}
					} else {
						// 8x16 sprite
						sprite_pattern_addr_lo =
							((scan_id & 0x01) << 12) | ((scan_id & 0xfe) << 4);

						if self.sprite_state.scanline[i].attr & 0x80 != 0 {
							sprite_pattern_addr_lo |= (scan_y as u16) & 0x07;
						} else {
							sprite_pattern_addr_lo |= (7 - scan_y) as u16 & 0x07;
						}
					}

					let sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

					let mut sprite_pattern_bits_lo =
						self.ppu_read(sprite_pattern_addr_lo);
					let mut sprite_pattern_bits_hi =
						self.ppu_read(sprite_pattern_addr_hi);

					if self.sprite_state.scanline[i].attr & 0x40 != 0 {
						sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
						sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
					}

					self.sprite_state.shifter_pattern_lo[i] = sprite_pattern_bits_lo;
					self.sprite_state.shifter_pattern_hi[i] = sprite_pattern_bits_hi;
				}
			}
		}

		if self.scanline >= 241
			&& self.scanline < 261
			&& self.scanline == 241
			&& self.cycle == 1
		{
			self.status.vertical_blank = true;
			if self.ctrl.generate_nmi {
				self.nmi = true;
			}
		}

		let mut bg_pixel: u8 = 0x00;
		let mut bg_palette: u8 = 0x00;

		if self.mask.show_background
			&& (self.mask.show_background_left || (self.cycle >= 9))
		{
			let bit_mux: u16 = 0x8000 >> self.fine_x;

			let p0_pixel: u8 =
				((self.bg_state.shifter_pattern_lo & bit_mux) > 0) as u8;
			let p1_pixel: u8 =
				((self.bg_state.shifter_pattern_hi & bit_mux) > 0) as u8;

			bg_pixel = (p1_pixel << 1) | p0_pixel;

			let bg_pal0: u8 =
				((self.bg_state.shifter_attrib_lo & bit_mux) > 0) as u8;
			let bg_pal1: u8 =
				((self.bg_state.shifter_attrib_hi & bit_mux) > 0) as u8;
			bg_palette = (bg_pal1 << 1) | bg_pal0;
		}

		let mut fg_pixel: u8 = 0x00;
		let mut fg_palette: u8 = 0x00;
		let mut fg_priority: bool = false;

		if self.mask.show_sprites && (self.mask.show_sprites_left || self.cycle >= 9)
		{
			self.sprite_zero_being_rendered = false;

			for i in 0..self.sprite_state.count as usize {
				if self.sprite_state.scanline[i].x == 0 {
					let fg_pixel_lo: u8 =
						((self.sprite_state.shifter_pattern_lo[i] & 0x80) > 0) as u8;
					let fg_pixel_hi: u8 =
						((self.sprite_state.shifter_pattern_hi[i] & 0x80) > 0) as u8;
					fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

					fg_palette = (self.sprite_state.scanline[i].attr & 0x03) + 0x04;
					fg_priority = (self.sprite_state.scanline[i].attr & 0x20) == 0;

					if fg_pixel != 0 {
						if i == 0 {
							self.sprite_zero_being_rendered = true;
						}
						break;
					}
				}
			}
		}

		let mut pixel: u8 = 0x00;
		let mut palette: u8 = 0x00;

		if bg_pixel == 0 && fg_pixel == 0 {
			pixel = 0x00;
			palette = 0x00;
		} else if bg_pixel == 0 && fg_pixel > 0 {
			pixel = fg_pixel;
			palette = fg_palette;
		} else if bg_pixel > 0 && fg_pixel == 0 {
			pixel = bg_pixel;
			palette = bg_palette;
		} else if bg_pixel > 0 && fg_pixel > 0 {
			if fg_priority {
				pixel = fg_pixel;
				palette = fg_palette;
			} else {
				pixel = bg_pixel;
				palette = bg_palette;
			}

			if self.sprite_zero_hit_possible
				&& self.sprite_zero_being_rendered
				&& self.mask.show_background & self.mask.show_sprites
			{
				if !(self.mask.show_background_left | self.mask.show_sprites_left) {
					if self.cycle >= 9 && self.cycle < 258 {
						self.status.sprite_zero_hit = true;
					}
				} else if self.cycle >= 1 && self.cycle < 258 {
					self.status.sprite_zero_hit = true;
				}
			}
		}

		let color = self.get_color_from_palette(palette, pixel);
		if self.cycle > 0
			&& self.cycle <= 256
			&& self.scanline >= 0
			&& self.scanline < 240
		{
			self.screen.set_pixel(
				(self.cycle - 1) as usize,
				self.scanline as usize,
				color,
			);
		}

		self.cycle += 1;

		if self.cycle >= 341 {
			self.cycle = 0;
			self.scanline += 1;
			if self.scanline >= 261 {
				self.scanline = -1;
				self.screen.complete = true;
				self.odd_frame = !self.odd_frame;
			}
		}
	}

	pub fn get_color_from_palette(&mut self, palette: u8, pixel: u8) -> u32 {
		PALETTE[(self.ppu_read(0x3f00 + ((palette << 2) + pixel) as u16) & 0x3f)
			as usize]
	}
}

impl Addressable for Ppu {
	fn address_range(&self) -> &Range<u16> {
		&(0x2000..0x3FFF)
	}

	fn write(&mut self, address: u16, data: u8) {
		match address & 0x0007 {
			0x0000 => {
				self.ctrl.set_byte(data);
				self.tram_address.nametable_x = self.ctrl.nametable_x;
				self.tram_address.nametable_y = self.ctrl.nametable_y;
			}
			0x0001 => {
				self.mask.set_byte(data);
			}
			0x0002 => {
				// status
			}
			0x0003 => {
				self.oam_address = data;
			}
			0x0004 => {
				self.write_oam(self.oam_address, data);
			}
			0x0005 => {
				if !self.loopy_latch {
					self.fine_x = data & 0x07;
					self.tram_address.coarse_x = data >> 3;
					self.loopy_latch = true;
				} else {
					self.tram_address.fine_y = data & 0x07;
					self.tram_address.coarse_y = data >> 3;
					self.loopy_latch = false;
				}
			}
			0x0006 => {
				if !self.loopy_latch {
					self.tram_address.set_data(
						(((data as u16) & 0x3f) << 8)
							| (self.tram_address.to_data() & 0x00FF),
					);
					self.loopy_latch = true;
				} else {
					self.tram_address.set_data(
						(self.tram_address.to_data() & 0xff00) | (data as u16),
					);
					self.vram_address = self.tram_address.clone();
					self.loopy_latch = false;
				}
			}
			0x0007 => {
				self.ppu_write(self.vram_address.to_data(), data);
				self.increment_vram_address();
			}
			_ => panic!("Invalid PPU address"),
		}
	}

	fn read(&mut self, address: u16) -> u8 {
		let mut data: u8;

		match address & 0x0007 {
			0x0002 => {
				data = (self.status.to_byte() & 0xe0) | (self.ppu_data_buf & 0x1f);
				self.status.vertical_blank = false;
				self.loopy_latch = false;
			}
			0x0004 => {
				let index = (self.oam_address / 4) as usize;
				data = {
					match self.oam_address % 4 {
						0 => self.oam_memory[index].y,
						1 => self.oam_memory[index].id,
						2 => self.oam_memory[index].attr,
						3 => self.oam_memory[index].x,
						_ => 0,
					}
				};
			}
			0x0007 => {
				data = self.ppu_data_buf;
				self.ppu_data_buf = self.ppu_read(self.vram_address.to_data());
				if self.vram_address.to_data() >= 0x3f00 {
					data = self.ppu_data_buf;
				}
				self.increment_vram_address();
			}
			_ => panic!("Invalid PPU address"),
		}

		data
	}
}

fn flipbyte(b: u8) -> u8 {
	let mut b = (b & 0xf0) >> 4 | (b & 0x0f) << 4;
	b = (b & 0xcc) >> 2 | (b & 0x33) << 2;
	b = (b & 0xaa) >> 1 | (b & 0x55) << 1;
	b
}
