#include "bstone_latches8.h"

#include <stdexcept>

#include "id_heads.h"


namespace bstone {


// ========================================================================
// Latch8

Latch8::Latch8() :
    tc_x0(),
    tc_y0(),
    tc_x1(),
    tc_y1()
{
}

// Latch8
// ========================================================================

// ========================================================================
// Latches8

Latches8::Latches8() :
    texture_()
{
}

Latches8::Latches8(
    int id) :
        texture_()
{
    initialize(id);
}

Latches8::~Latches8()
{
    uninitialize();
}

bool Latches8::initialize(
    int id)
{
    uninitialize();

    const Uint8* data = static_cast<const Uint8*>(::grsegs[id]);

    int texture_width = 0;
    int texture_height = 0;
    suggest_texture_dimensions(texture_width, texture_height);

    Latches latches(NUMTILE8);
    std::vector<Rgba8U> buffer(texture_width * texture_height);

    const float r_texture_width_f = 1.0F / texture_width;
    const float r_texture_height_f = 1.0F / texture_height;

    int tile_x = 0;
    int tile_y = 0;

    for (int i = 0; i < NUMTILE8; ++i) {
        if ((tile_x + 9) > texture_width) {
            tile_x = 0;
            tile_y += 9;
        }

        for (int plane = 0; plane < 4; ++plane) {
            for (int y = 7; y >= 0; --y) {
                for (int x = 0; x < 4; ++x) {
                    int pixel_offset = ((tile_y + y) * texture_width) +
                        tile_x + (4 * x) + plane;

                    buffer[pixel_offset] = ::g_palette[*data++];
                }
            }
        }

        Latch8& latch = latches[i];

        latch.tc_x0 = tile_x * r_texture_width_f;
        latch.tc_y0 = tile_y * r_texture_height_f;

        latch.tc_x1 = (tile_x + 8) * r_texture_width_f;
        latch.tc_y1 = (tile_y + 8) * r_texture_height_f;

        tile_x += 9;
    }

    OglTexture* texture = new OglTexture(
        texture_width, texture_height, OTF_RGB, &buffer[0]);

    if (!texture->is_initialized()) {
        delete texture;
        return false;
    }

    texture_ = texture;
    latches_.swap(latches);

    return true;
}

void Latches8::uninitialize()
{
    if (!is_initialized())
        return;

    delete texture_;
    texture_ = NULL;

    Latches().swap(latches_);
}

OglTexture* Latches8::get_texture()
{
    return texture_;
}

bool Latches8::is_initialized() const
{
    return texture_ != NULL;
}

int Latches8::get_width() const
{
    return 8;
}

int Latches8::get_height() const
{
    return 8;
}

const Latch8& Latches8::get_latch(
    int index) const
{
    if (index < 0 || index > NUMTILE8)
        throw std::out_of_range("index");

    return latches_[index];
}

// (static)
void Latches8::suggest_texture_dimensions(
    int& width,
    int& height)
{
    const int tile_width = 8;
    const int tile_height = 8;

    // (By increasing width and height by 1 we
    // avoid artefacts when using non-nearest texture filtering.)
    int total_width = (tile_width + 1) * NUMTILE8;
    int area = (tile_height + 1) * total_width;

    int texture_width = 1;

    while ((texture_width * texture_width) < area)
        texture_width *= 2;

    int x = 0;
    int y = tile_height + 1;

    for (int i = 0; i < NUMTILE8; ++i) {
        x += tile_width + 1;

        if (x > texture_width) {
            x = 0;
            y += tile_height + 1;
        }
    }

    int texture_height = 1;

    while (texture_height < y)
        texture_height *= 2;

    if (texture_width > OglApi::get_max_2d_texture_size() ||
        texture_height > OglApi::get_max_2d_texture_size())
    {
        ::Quit("%s: %s", "OGL", "Tile8 texture is too big.");
    }

    width = texture_width;
    height = texture_height;
}

// Latches8
// ========================================================================


} // namespace bstone
