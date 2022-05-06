#pragma once

enum class ImageFormat {
    NONE = 0,
    RGB  = 3,
    RGBA
};

class Image {

public:
    Image(u32 width, u32 height);
    ~Image();

    void render();
    void update();
    void scale_width(float width);
    void scale_height(float height);
    void array_to_image(const u32* array);
    void set_pixel(u32 x, u32 y, u32 color);
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

    u8* m_data = nullptr;
};
