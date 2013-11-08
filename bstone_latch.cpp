#include "bstone_latch.h"

#include "id_heads.h"


namespace bstone {


Latch::Latch() :
    width_(),
    height_(),
    texture_()
{
}

Latch::Latch(
    int id) :
        width_(),
        height_(),
        texture_()
{
    initialize(id);
}

Latch::~Latch()
{
    uninitialize();
}

bool Latch::initialize(
    int id)
{
    uninitialize();

    const Uint8* data = static_cast<const Uint8*>(grsegs[id]);

    int width = pictable[id - STARTPICS].width;
    int q_width = width / 4;
    int height = pictable[id - STARTPICS].height;
    std::vector<Rgba8U> buffer(width * height);

    for (int plane = 0; plane < 4; ++plane) {
        for (int y = height - 1; y >= 0; --y) {
            for (int x = 0; x < q_width; ++x) {
                int buffer_offset = (y * width) + (4 * x) + plane;
                buffer[buffer_offset] = ::g_palette[*data++];
            }
        }
    }

    OglTexture* texture =
        new OglTexture(width, height, OTF_RGB, &buffer[0]);

    if (!texture->is_initialized()) {
        delete texture;
        return false;
    }

    width_ = width;
    height_ = height;
    texture_ = texture;

    return true;
}

void Latch::uninitialize()
{
    width_ = 0;
    height_ = 0;

    delete texture_;
    texture_ = NULL;
}

OglTexture* Latch::get_texture()
{
    return texture_;
}

bool Latch::is_initialized() const
{
    return texture_ != NULL;
}

int Latch::get_width() const
{
    return width_;
}

int Latch::get_height() const
{
    return height_;
}


} // namespace bstone
