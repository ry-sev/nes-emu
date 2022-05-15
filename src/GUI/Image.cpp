#include "Image.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

Image::Image(u32 width, u32 height, bool hover)
{
    m_hover = hover;
    m_width = width;
    m_height = height;
    m_texture_width = static_cast<float>(width);
    m_texture_height = static_cast<float>(height);
    m_format = ImageFormat::RGBA;

    auto size = m_width * m_height * bytes_per_channel(m_format);
    m_data = new u8[size];
    memset(m_data, 0, size);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)m_data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Image::~Image()
{
    delete[] m_data;
    glDeleteTextures(1, &m_texture);
}

void Image::render()
{
    auto mouse_position = ImGui::GetCursorScreenPos();
    ImGui::Image((void*)(intptr_t)m_texture, ImVec2(m_texture_width, m_texture_height));
        
    if (ImGui::IsItemHovered() && m_hover) {

        auto io = ImGui::GetIO();
        ImGui::BeginTooltip();

        auto region_sz = 32.0f;
        auto region_x = io.MousePos.x - mouse_position.x - region_sz * 0.5f;
        auto region_y = io.MousePos.y - mouse_position.y - region_sz * 0.5f;
        auto zoom = 4.0f;
        
        if (region_x < 0.0f)
            region_x = 0.0f;
        else if (region_x > m_texture_width - region_sz)
            region_x = m_texture_width - region_sz;
        
        if (region_y < 0.0f)
            region_y = 0.0f;
        else if (region_y > m_texture_height - region_sz)
            region_y = m_texture_height - region_sz;
        
        auto uv0 = ImVec2((region_x) / m_texture_width, (region_y) / m_texture_height);
        auto uv1 = ImVec2(
            (region_x + region_sz) / m_texture_width,
            (region_y + region_sz) / m_texture_height
        );
        
        ImGui::Image(
            (void*)(intptr_t)m_texture,
            ImVec2(region_sz * zoom, region_sz * zoom),
            uv0,
            uv1
        );
        
        ImGui::EndTooltip();
    }
}

void Image::update()
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_BGRA, GL_UNSIGNED_BYTE, (void*)m_data);
}

void Image::array_to_image(const i32* array)
{
    memcpy(
        reinterpret_cast<void*>(m_data),
        reinterpret_cast<const void*>(array),
        m_width * m_height * sizeof(i32)
    );
}

void Image::set_pixel(i32 x, i32 y, i32 color)
{
    if (x < 0 || x >= static_cast<i32>(m_width) || y < 0 || y >= static_cast<i32>(m_height))
        return;

    auto data = reinterpret_cast<i32*>(m_data);
    const i32 position = (y * m_width) + x;
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
