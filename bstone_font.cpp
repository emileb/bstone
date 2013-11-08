#include "bstone_font.h"

#include <stdexcept>

#include "id_heads.h"

#include "bstone_rgba.h"


namespace bstone {


FontChar::FontChar() :
    width(),
    tc_x0(),
    tc_y0(),
    tc_x1(),
    tc_y1()
{
}

Font::Font() :
    is_initialized_(),
    height_(),
    texture_()
{
}

Font::Font(
    int id) :
        is_initialized_(),
        height_(),
        texture_()
{
    initialize(id);
}

Font::~Font()
{
    uninitialize();
}

bool Font::initialize(
    int id)
{
    uninitialize();

    id += STARTFONT;

    if (id < 0 || id >= NUMCHUNKS)
        throw std::out_of_range("id");

    ::CA_CacheGrChunk(static_cast<Sint16>(id));

    MemoryBinaryReader reader(grsegs[id], 65536);

    int height = Endian::le(reader.read_u16());

    // Offsets.

    std::vector<int> offsets;
    offsets.resize(256);

    for (int i = 0; i < 256; ++i)
        offsets[i] = Endian::le(reader.read_u16());

    // Widths.

    std::vector<int> widths;
    widths.resize(256);

    for (int i = 0; i < 256; ++i)
        widths[i] = reader.read_u8();

    Chars chars;
    chars.resize(256);

    int texture_width;
    int texture_height;

    suggest_texture_dimensions(&widths[0], height,
        texture_width, texture_height);

    std::vector<Rgba8U> buffer;
    buffer.resize(texture_width * texture_height, Rgba8U(0));

    const float texture_width_rf = 1.0F / texture_width;
    const float texture_height_rf = 1.0F / texture_height;

    int tx = 0;
    int ty = 0;

    for (int i = 0; i < 256; ++i) {
        int width = widths[i];

        if (width == 0)
            continue;

        if ((tx + width) > texture_width) {
            tx = 0;
            ty += height + 1;
        }

        const Uint8* char_data = static_cast<const Uint8*>(grsegs[id]);
        char_data = &char_data[offsets[i]];

        for (int cy = 0; cy < height; ++cy) {
            for (int cx = 0; cx < width; ++cx) {
                bool has_pixel = ((*char_data++) != 0);

                if (!has_pixel)
                    continue;

                int x = tx + cx;
                int y = ty + (height - 1 - cy);
                int offset = (y * texture_width) + x;

                buffer[offset] = Rgba8U(255);
            }
        }

        FontChar& ogl_char = chars[i];

        ogl_char.width = width;

        ogl_char.tc_x0 = tx * texture_width_rf;
        ogl_char.tc_y0 = ty * texture_height_rf;

        ogl_char.tc_x1 = (tx + width) * texture_width_rf;
        ogl_char.tc_y1 = (ty + height) * texture_height_rf;

        tx += width;
    }

    OglTexture* texture = new OglTexture(
        texture_width, texture_height, OTF_RGBA, &buffer[0]);

    if (!texture->is_initialized()) {
        delete texture;
        ::Quit("%s: %s", "OGL", "Failed to generate a font texture.");
    }

    is_initialized_ = true;
    height_ = height;
    chars_.swap(chars);
    texture_ = texture;

    return true;
}

void Font::uninitialize()
{
    is_initialized_ = false;
    height_ = 0;
    chars_.clear();

    delete texture_;
    texture_ = NULL;
}

void Font::measure_string(
    const std::string& string,
    int& width,
    int& height) const
{
    width = 0;
    height = 0;

    if (!is_initialized())
        return;

    if (string.empty())
        return;

    size_t length = string.size();

    for (size_t i = 0; i < length; ++i) {
        int char_index = static_cast<Uint8>(string[i]);
        width += chars_[char_index].width;
    }

    height = height_;
}

void Font::measure_text(
    const std::string& text,
    int& width,
    int& height) const
{
    width = 0;
    height = 0;

    if (!is_initialized())
        return;

    if (text.empty())
        return;

    int current_width = 0;
    size_t length = text.size();

    height = height_;

    for (size_t i = 0; i < length; ++i) {
        int char_index = static_cast<Uint8>(text[i]);

        if (char_index == '\n') {
            if (current_width > width)
                width = current_width;

            current_width = 0;
            height += height_;
        }
        current_width += chars_[char_index].width;
    }

    if (current_width > width)
        width = current_width;
}

const FontChar& Font::get_char(
    int index) const
{
    if (!is_initialized())
        throw std::logic_error("Not initialized.");

    if (index < 0 || index >= 256)
        throw std::out_of_range("index");

    return chars_[index];
}

int Font::get_height() const
{
    return height_;
}

OglTexture* Font::get_texture()
{
    return texture_;
}

bool Font::is_initialized() const
{
    return is_initialized_;
}

// (static)
void Font::suggest_texture_dimensions(
    const int* chars_widths,
    int chars_height,
    int& width,
    int& height)
{
    int total_width = 0;

    for (int i = 0; i < 256; ++i)
        total_width += chars_widths[i];

    // (By increasing height by 1 we avoid artefacts when using
    // non-nearest texture filtering. Since characters itself
    // already padded horizontally with 1 pixel in file we do not need
    // to increase a width.)
    int area = total_width * (chars_height + 1);

    int texture_width = 1;

    while ((texture_width * texture_width) < area)
        texture_width *= 2;


    int x = 0;
    int y = chars_height + 1;

    for (int i = 0; i < 256; ++i) {
        int char_width = chars_widths[i];

        if (char_width == 0)
            continue;

        x += char_width;

        if (x > texture_width) {
            x = 0;
            y += chars_height + 1;
        }
    }

    int texture_height = 1;

    while (texture_height < y)
        texture_height *= 2;

    if (texture_width > OglApi::get_max_2d_texture_size() ||
        texture_height > OglApi::get_max_2d_texture_size())
    {
        ::Quit("%s: %s", "OGL", "Font texture is too big.");
    }

    width = texture_width;
    height = texture_height;
}


} // namespace bstone
