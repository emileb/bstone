#include "bstone_background.h"

#include "id_heads.h"


#include "bstone_rgba.h"

namespace bstone {


Background::Background() :
    texture_()
{
}

Background::Background(
    int id) :
        texture_()
{
    initialize(id);
}

Background::~Background()
{
    uninitialize();
}

bool Background::initialize(
    int id)
{
    uninitialize();

    const Uint8* data = static_cast<const Uint8*>(grsegs[id]);

    std::vector<Rgba8U> buffer(vanilla_screen_width * vanilla_screen_height);

    int q_width = vanilla_screen_width / 4;

    for (int plane = 0; plane < 4; ++plane) {
        for (int y = vanilla_screen_height - 1; y >= 0; --y) {
            for (int x = 0; x < q_width; ++x) {
                const Uint8* palette = &vgapal[3 * (*data++)];
                int buffer_offset =
                    (y * vanilla_screen_width) + (4 * x) + plane;

                buffer[buffer_offset] = Rgba8U(
                    (255 * palette[0]) / 63,
                    (255 * palette[1]) / 63,
                    (255 * palette[2]) / 63);
            }
        }
    }

    OglTexture* texture = new OglTexture(
        vanilla_screen_width, vanilla_screen_height, OTF_RGB, &buffer[0]);

    if (!texture->is_initialized()) {
        delete texture;
        return false;
    }

    texture_ = texture;

    return true;
}

void Background::uninitialize()
{
}

OglTexture* Background::get_texture()
{
    return texture_;
}

bool Background::is_initialized() const
{
    return texture_ != NULL;
}


} // namespace bstone
