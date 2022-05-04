#pragma once

class Cartridge;

union Status {
    struct {
        u8 unused : 5;
        u8 sprite_overflow : 1;
        u8 sprite_zero_hit : 1;
        u8 vertical_blank : 1;
    };
    u8 reg;
};

union Mask {
    struct {
        u8 greyscale : 1;
        u8 show_background_left : 1;
        u8 show_sprites_left: 1;
        u8 show_background : 1;
        u8 show_sprites : 1;
        u8 emphasize_red : 1;
        u8 emphasize_green : 1;
        u8 emphasize_blue : 1;
    };
    u8 reg;
};

union Control {
    struct {
        u8 nametable_x : 1;
        u8 nametable_y : 1;
        u8 increment_mode : 1;
        u8 pattern_sprite : 1;
        u8 pattern_background : 1;
        u8 sprite_size : 1;
        u8 slave_mode : 1;
        u8 enable_nmi : 1;
    };
    u8 reg;
};

union LoopyRegister {
    struct {
        u16 coarse_x : 5;
        u16 coarse_y : 5;
        u16 nametable_x : 1;
        u16 nametable_y : 1;
        u16 fine_y : 3;
        u16 unused : 1;
    };
    u16 reg;
};

class PPU {

public:
    PPU();
    ~PPU() = default;

    u8 cpu_read(u16 address);
    void cpu_write(u16 address, u8 value);
    u8 ppu_read(u16 address);
    void ppu_write(u16 address, u8 value);

    void insert_cartridge(Cartridge* cartridge);
    void clock();
    const u16& cycles() const { return m_cycles; }

    int color_from_pallete(u8 pallete, u8 pixel);

private:
    Cartridge* m_cartridge;
    u16 m_cycles = 0;

	u8 m_nametable[2][1024];
	u8 m_pattern_table[2][4096];
	u8 m_pallete_table[32];
    u32 m_pallete[0x40];

    Status m_ppu_status;
    Mask m_ppu_mask;
    Control m_ppu_control;
    LoopyRegister m_vram_address;
    LoopyRegister m_tram_address;

    u8 m_address_latch = 0x00;
    u8 m_ppu_data_buffer = 0x00;
    u16 m_ppu_address = 0x0000;
};
