#include "Image.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

Image::Image(u32 width, u32 height)
{
    m_width = width;
    m_height = height;
    m_texture_width = static_cast<float>(width);
    m_texture_height = static_cast<float>(height);
    m_format = ImageFormat::RGB;

    auto size = m_width * m_height * bytes_per_channel(m_format);
    m_data = new u8[size];
    memset(m_data, 0, size);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)m_data);

    glBindTexture(GL_TEXTURE_2D, 0);

}

Image::~Image()
{
    delete[] m_data;
    glDeleteTextures(1, &m_texture);
}

void Image::render()
{
    ImGui::Image((void*)(intptr_t)m_texture, ImVec2(m_texture_width, m_texture_height));
}

void Image::update()
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, (void*)m_data);
}

void Image::set_pixel(u32 x, u32 y, u32 color)
{
    if (x >= m_width || y >= m_height)
        return;

    u32* data = reinterpret_cast<u32*>(m_data);
    const u32 position = (y * m_width) + x;
    data[position] = color;
}


void Image::scale_width(float width)
{
    if (m_texture_width == width)
        return;

    auto scale = width / m_texture_width;
    m_texture_width = width;
    m_texture_height *= scale;
}

void Image::scale_height(float height)
{
    if (m_texture_height == height)
        return;

    auto scale = height / m_texture_height;
    m_texture_height = height;
    m_texture_width *= scale;
}

u32 Image::bytes_per_channel(ImageFormat format)
{
	switch (format)
	{
        case ImageFormat::RGB:  return 3;
		case ImageFormat::RGBA: return 4;
        default: return 0;
	}
}
