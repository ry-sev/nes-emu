#pragma once
#include <memory>

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

union PPUSTATUS {
    struct {
        u8 unused : 5;
        u8 sprite_overflow : 1;
        u8 sprite_zero_hit : 1;
        u8 vertical_blank : 1;
    };
    u8 byte;
};

union PPUMASK {
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
    u8 byte;
};

union PPUCTRL {
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
    u8 byte;
};

union LOOPY {
    struct {
        u16 coarse_x : 5;
        u16 coarse_y : 5;
        u16 nametable_x : 1;
        u16 nametable_y : 1;
        u16 fine_y : 3;
        u16 unused : 1;
    };
    u16 byte;
};

struct PPURegisters {
    PPUSTATUS status;
    PPUMASK mask;
    PPUCTRL control;
    LOOPY vram_address;
    LOOPY tram_address;
};

class Cartridge;

class PPU {

public:
    PPU();
    ~PPU();

    u8 cpu_read(u16 address);
    void cpu_write(u16 address, u8 value);
    u8 ppu_read(u16 address);
    void ppu_write(u16 address, u8 value);

    void insert_cartridge(std::shared_ptr<Cartridge> cartridge);
    void clock();
    u16 cycles() const { return m_cycles; }
    const u32* screen() const { return m_screen; }
    bool frame_is_complete() { return m_frame_complete; }

    u32 color_from_palette(u8 palette, u8 pixel);

private:
    std::shared_ptr<Cartridge> m_cartridge;
    u16 m_cycles = 0;
    u16 m_scan_line = 0;
    bool m_frame_complete = false;
    u32* m_screen = nullptr;

    struct MemoryMap {
        u8 nametable[2][1024];
    	u8 pattern_table[2][4096];
    	u8 palette_table[0x40];
        const u32 palette[0x40] = {
            0xFF545454, 0xFF001E74, 0xFF081090, 0xFF300088, 0xFF440064, 0xFF5C0030,
            0xFF540400, 0xFF3C1800, 0xFF202A00, 0xFF083A00, 0xFF004000, 0xFF003C00,
            0xFF00323C, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF989698, 0xFF084CC4,
            0xFF3032EC, 0xFF5C1EE4, 0xFF8814B0, 0xFFA01464, 0xFF982220, 0xFF783C00,
            0xFF545A00, 0xFF287200, 0xFF087C00, 0xFF007628, 0xFF006678, 0xFF000000,
            0xFF000000, 0xFF000000, 0xFFECEEEC, 0xFF4C9AEC, 0xFF787CEC, 0xFFB062EC,
            0xFFE454EC, 0xFFEC58B4, 0xFFEC6A64, 0xFFD48820, 0xFFA0AA00, 0xFF74C400,
            0xFF4CD020, 0xFF38CC6C, 0xFF38B4CC, 0xFF3C3C3C, 0xFF000000, 0xFF000000,
            0xFFECEEEC, 0xFFA8CCEC, 0xFFBCBCEC, 0xFFD4B2EC, 0xFFECAEEC, 0xFFECAED4,
            0xFFECD4AE, 0xFFE4C490, 0xFFCCD278, 0xFFB4DE78, 0xFFA8E290, 0xFF98E2B4,
            0xFFA0D6E4, 0xFFA0A2A0, 0xFF000000, 0xFF000000
        };
    } m_memory_map;

    PPURegisters m_registers;

    u8 m_address_latch = 0x00;
    u8 m_ppu_data_buffer = 0x00;
    u16 m_ppu_address = 0x0000;
};
