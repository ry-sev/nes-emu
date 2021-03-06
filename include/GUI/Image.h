#pragma once

#define HOVER true
#define NO_HOVER false

enum class ImageFormat {
    NONE = 0,
    RGB  = 3,
    RGBA
};

class Image {

public:
    Image(u32 width, u32 height, bool hover = NO_HOVER);
    ~Image();

    void render();
    void update();
    void scale_width(float width);
    void scale_height(float height);
    void array_to_image(const i32* array);
    void set_pixel(i32 x, i32 y, i32 color);
    float width() const { return m_texture_width; }
    float height() const { return m_texture_height; }

private:

    u32 bytes_per_channel(ImageFormat format);

    u32 m_width;
    u32 m_height;
    u32 m_texture = 0;
    float m_texture_width;
    float m_texture_height;
    ImageFormat m_format = ImageFormat::NONE;
    bool m_hover;

    u8* m_data = nullptr;
};
