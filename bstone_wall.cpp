#include "bstone_wall.h"

#include "id_heads.h"
#include "bstone_rgba.h"


namespace bstone {


Wall::Wall() :
    width_(),
    height_(),
    texture_()
{
}

Wall::Wall(
    int id) :
        width_(),
        height_(),
        texture_()
{
    initialize(id);
}

Wall::~Wall()
{
    uninitialize();
}

bool Wall::initialize(
    int id)
{
    uninitialize();

    const Uint8* data = static_cast<const Uint8*>(::PM_GetPage(id));

    std::vector<Rgba8U> buffer(64 * 64);

    for (int x = 0; x < 64; ++x) {
        for (int y = 63; y >= 0; --y) {
            int buffer_offset = (y * 64) + x;
            buffer[buffer_offset] = ::g_palette[*data++];
        }
    }

    OglTexture* texture =
        new OglTexture(64, 64, OTF_RGB, &buffer[0]);

    if (!texture->is_initialized()) {
        delete texture;
        return false;
    }

    width_ = 64;
    height_ = 64;
    texture_ = texture;

    return true;
}

void Wall::uninitialize()
{
    width_ = 0;
    height_ = 0;

    delete texture_;
    texture_ = NULL;
}

OglTexture* Wall::get_texture()
{
    return texture_;
}

bool Wall::is_initialized() const
{
    return texture_ != NULL;
}

int Wall::get_width() const
{
    return width_;
}

int Wall::get_height() const
{
    return height_;
}


} // namespace bstone
