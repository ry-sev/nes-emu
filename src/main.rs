mod gui;
mod nes;

use anyhow::{anyhow, Result};
use gui::App;
use nes::Nes;

const WINDOW_WIDTH: f32 = 768.0;
const WINDOW_HEIGHT: f32 = 744.0;

fn main() -> Result<()> {
	let options = eframe::NativeOptions {
		initial_window_size: Some(egui::vec2(WINDOW_WIDTH, WINDOW_HEIGHT)),
		resizable: false,
		default_theme: eframe::Theme::Dark,
		always_on_top: true,
		..Default::default()
	};

	let app = App::default();

	if let Err(e) =
		eframe::run_native("NES Emulator", options, Box::new(|_cc| Box::new(app)))
	{
		return Err(anyhow!("{e}"));
	}

	Ok(())
}
