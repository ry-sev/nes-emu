use std::cell::RefCell;
use std::ops::Range;
use std::rc::Rc;

struct Connection {
	component: Rc<RefCell<dyn Addressable>>,
	address_range: Range<u16>,
}

pub trait Addressable {
	fn address_range(&self) -> &Range<u16>;
	fn read(&mut self, address: u16) -> u8;
	fn write(&mut self, address: u16, data: u8);
}

pub struct Bus {
	connections: Vec<Connection>,
}

impl Bus {
	pub fn new() -> Self {
		Self {
			connections: vec![],
		}
	}

	pub fn connect(&mut self, component: Rc<RefCell<dyn Addressable>>) {
		let address_range = component.borrow_mut().address_range().clone();
		self.connections.push(Connection {
			component,
			address_range,
		})
	}

	pub fn read(&mut self, address: u16) -> u8 {
		for connection in &mut self.connections {
			if connection.address_range.contains(&address) {
				return connection.component.borrow_mut().read(address);
			}
		}
		0x00
	}

	pub fn write(&mut self, address: u16, data: u8) {
		for connection in &mut self.connections {
			if connection.address_range.contains(&address) {
				connection.component.borrow_mut().write(address, data);
				return;
			}
		}
	}
}

impl Default for Bus {
	fn default() -> Self {
		Self::new()
	}
}
