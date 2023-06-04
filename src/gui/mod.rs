mod display;

use crate::Nes;
use anyhow::Result;
use display::Screen;
use std::path::PathBuf;

fn get_roms() -> Result<Vec<PathBuf>> {
	let roms = std::fs::read_dir("roms")?
		.filter_map(|res| res.ok())
		.map(|dir_entry| dir_entry.path())
		.filter_map(|path| {
			if path.extension().map_or(false, |ext| ext == "nes") {
				Some(path)
			} else {
				None
			}
		})
		.collect::<Vec<_>>();

	Ok(roms)
}

pub struct App {
	roms: Vec<PathBuf>,
	nes: Nes,
	screen: Screen,
}

impl App {
	pub fn new() -> Self {
		Self {
			roms: get_roms().unwrap(),
			nes: Nes::new(),
			screen: Screen::default(),
		}
	}

	pub fn handle_key_event(&mut self, ui: &mut egui::Ui) {
		use egui::Key;

		let mut controller = self.nes.controller.borrow_mut();

		for (key, byte) in [
			(Key::X, 0x80),
			(Key::Z, 0x40),
			(Key::A, 0x20),
			(Key::S, 0x10),
			(Key::ArrowUp, 0x8),
			(Key::ArrowDown, 0x04),
			(Key::ArrowLeft, 0x02),
			(Key::ArrowRight, 0x01),
		] {
			ui.input_mut(|i| {
				if i.key_down(key) {
					controller.input |= byte;
				} else {
					controller.input &= !byte;
				}
			});
		}
	}
}

impl eframe::App for App {
	fn update(&mut self, ctx: &egui::Context, frame: &mut eframe::Frame) {
		egui::TopBottomPanel::top("menu").show(ctx, |ui| {
			egui::menu::bar(ui, |ui| {
				egui::widgets::global_dark_light_mode_switch(ui);
				ui.separator();
				ui.menu_button("Games", |ui| {
					for rom_file in &self.roms {
						let filename =
							rom_file.file_name().unwrap().to_str().unwrap();
						if ui.button(filename).clicked() {
							if let Err(e) = self.nes.insert_cartridge(rom_file) {
								println!("{}", e);
							}
						}
					}
				});
				if ui.button("Quit").clicked() {
					frame.close();
				}
				egui::warn_if_debug_build(ui);
			});
		});

		#[rustfmt::skip]
		let my_frame = egui::Frame {
			inner_margin: egui::style::Margin { left: 0., right: 0., top: 0., bottom: 0., },
			outer_margin: egui::style::Margin { left: 0., right: 0., top: 0., bottom: 0., },
			rounding: egui::Rounding { nw: 0., ne: 0., sw: 0., se: 0.},
			..Default::default()
		};

		egui::CentralPanel::default()
			.frame(my_frame)
			.show(ctx, |ui| {
				ctx.request_repaint();
				self.handle_key_event(ui);
				self.nes.update(ui.input(|i| i.stable_dt.min(0.1)));
				{
					let mut ppu = self.nes.ppu.borrow_mut();
					if ppu.screen.complete {
						ppu.screen.complete = false;
						self.screen.update(&ppu);
					}
				}
				self.screen.ui(ctx, ui);
			});
	}
}

impl Default for App {
	fn default() -> Self {
		Self::new()
	}
}
