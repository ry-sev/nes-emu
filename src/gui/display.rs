use crate::nes::{Ppu, DISPLAY_HEIGHT, DISPLAY_WIDTH};

struct Texture {
	width: usize,
	height: usize,
	buffer: Vec<u8>,
}

impl Texture {
	fn new(width: usize, height: usize) -> Self {
		Self {
			width,
			height,
			buffer: vec![0; width * height * 4],
		}
	}

	fn set_pixel(&mut self, x: usize, y: usize, c: u32) {
		let r = ((c & 0xff0000) >> 16) as u8;
		let g = ((c & 0xff00) >> 8) as u8;
		let b = (c & 0xff) as u8;

		let x0 = x * 4;
		let y0 = y * 4;
		let pos = y0 * self.width;
		self.buffer[pos + x0..pos + x0 + 4].copy_from_slice(&[r, g, b, 0xff]);
	}
}

pub struct Screen {
	scale: f32,
	texture: Texture,
}

impl Screen {
	pub fn new() -> Self {
		Self {
			scale: 3.0_f32,
			texture: Texture::new(DISPLAY_WIDTH, DISPLAY_HEIGHT),
		}
	}

	pub fn ui(&mut self, ctx: &egui::Context, ui: &mut egui::Ui) {
		let mut image = egui::ColorImage::new(
			[self.texture.width, self.texture.height],
			egui::Color32::TRANSPARENT,
		);

		image.pixels = self
			.texture
			.buffer
			.chunks(4)
			.map(|p| egui::Color32::from_rgba_premultiplied(p[0], p[1], p[2], p[3]))
			.collect();

		let retained_image =
			egui_extras::RetainedImage::from_color_image("screen.png", image);

		let size = egui::Vec2::new(
			retained_image.size()[0] as f32 * self.scale,
			retained_image.size()[1] as f32 * self.scale,
		);

		ui.image(retained_image.texture_id(ctx), size);
	}

	pub fn update(&mut self, ppu: &Ppu) {
		for x in 0..self.texture.width {
			for y in 0..self.texture.height {
				self.texture.set_pixel(x, y, ppu.screen.get_pixel(x, y));
			}
		}
	}
}

impl Default for Screen {
	fn default() -> Self {
		Self::new()
	}
}
