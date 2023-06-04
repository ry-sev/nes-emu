mod bus;
mod cartridge;
mod controller;
mod cpu;
mod dma;
mod mappers;
mod ppu;
mod ram;

use anyhow::Result;
use bus::Bus;
use cartridge::Cartridge;
use controller::Controller;
use cpu::Cpu;
use dma::DmaDevice;
use ram::Ram;
use std::{cell::RefCell, path::PathBuf, rc::Rc};

pub use bus::Addressable;
pub use ppu::{Ppu, DISPLAY_HEIGHT, DISPLAY_WIDTH};

const FRAME_TIME: f32 = 1.0 / 60.0;

pub struct Nes {
	cpu: Cpu,
	//ram: Rc<RefCell<Ram>>,
	cartridge: Rc<RefCell<Cartridge>>,
	dma: Rc<RefCell<DmaDevice>>,
	pub ppu: Rc<RefCell<Ppu>>,
	clock: i32,
	rom_loaded: bool,
	frame_time: FrameTime,
	pub controller: Rc<RefCell<Controller>>,
}

impl Nes {
	pub fn new() -> Self {
		let mut cpu = Cpu::default();

		let ram = Rc::new(RefCell::new(Ram::default()));
		let cartridge = Rc::new(RefCell::new(Cartridge::new()));
		let ppu = Rc::new(RefCell::new(Ppu::new(cartridge.clone())));
		let dma = Rc::new(RefCell::new(DmaDevice::new()));
		let controller = Rc::new(RefCell::new(Controller::default()));

		cpu.bus.connect(ram);
		cpu.bus.connect(cartridge.clone());
		cpu.bus.connect(ppu.clone());
		cpu.bus.connect(dma.clone());
		cpu.bus.connect(controller.clone());

		Self {
			cpu,
			//ram,
			cartridge,
			dma,
			ppu,
			clock: 0,
			rom_loaded: false,
			frame_time: FrameTime::default(),
			controller,
		}
	}

	pub fn insert_cartridge(&mut self, rom: &PathBuf) -> Result<()> {
		self.cartridge.borrow_mut().insert(rom)?;
		self.cpu.reset();
		self.ppu.borrow_mut().reset();
		self.dma.borrow_mut().reset();
		self.clock = 1;
		self.rom_loaded = true;
		Ok(())
	}

	pub fn update(&mut self, dt: f32) {
		if !self.rom_loaded {
			return;
		}

		if !self.frame_time.update(dt) {
			return;
		}

		while !self.ppu.borrow().screen.complete {
			self.clock();
		}
	}

	pub fn clock(&mut self) {
		self.ppu.borrow_mut().clock();

		if self.clock % 3 == 0 {
			if self.dma.borrow_mut().transfer {
				self.dma.borrow_mut().clock(
					self.clock,
					&mut self.cpu,
					&mut self.ppu.borrow_mut(),
				);
			} else {
				self.cpu.clock();
			}
		}

		if self.ppu.borrow().nmi {
			self.ppu.borrow_mut().nmi = false;
			self.cpu.nmi();
		}

		self.clock += 1;
	}
}

impl Default for Nes {
	fn default() -> Self {
		Self::new()
	}
}

#[derive(Default)]
pub struct FrameTime {
	pub dt: f32,
	pub dt_accum: f32,
	pub fps: f32,
}

impl FrameTime {
	pub fn update(&mut self, dt: f32) -> bool {
		// limit to 60 fps
		self.dt_accum += dt;
		self.dt += dt;
		if self.dt_accum < FRAME_TIME {
			return false;
		}
		self.fps = 1.0 / self.dt;
		self.dt_accum -= FRAME_TIME;
		self.dt = 0.0;
		true
	}
}
