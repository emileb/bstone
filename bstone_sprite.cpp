#include "bstone_sprite.h"

#include "id_heads.h"

#include "bstone_rgba.h"


namespace bstone {


Sprite::Sprite() :
    x_(),
    y_(),
    width_(),
    height_(),
    tc_x0_(),
    tc_y0_(),
    tc_x1_(),
    tc_y1_(),
    texture_()
{
}

Sprite::Sprite(
    int id) :
        x_(),
        y_(),
        width_(),
        height_(),
        tc_x0_(),
        tc_y0_(),
        tc_x1_(),
        tc_y1_(),
        texture_()
{
    initialize(id);
}

Sprite::~Sprite()
{
    uninitialize();
}

bool Sprite::initialize(
    int id)
{
    uninitialize();

    const Uint8* data =
        static_cast<const Uint8*>(::PM_GetSpritePage(id));

    MemoryBinaryReader reader(data, 8192);

    int first_post = Endian::le(reader.read_u16());
    int last_post = Endian::le(reader.read_u16());
    int width = last_post - first_post + 1;

    std::vector<int> offsets;
    offsets.resize(width);

    for (int i = 0; i < width; ++i)
        offsets[i] = Endian::le(reader.read_u16());

    std::vector<Rgba8U> buffer;
    buffer.resize(64 * 64, Rgba8U(0));

    int min_y = 64;
    int max_y = 0;

    for (int x = first_post; x <= last_post; ++x) {
        reader.set_position(offsets[x - first_post]);

        int end_y = Endian::le(reader.read_u16()) / 2;

        while (end_y > 0) {
            int pixel_offset = Endian::le(reader.read_u16());
            int beg_y = Endian::le(reader.read_u16()) / 2;
            pixel_offset += beg_y;
            pixel_offset %= 65536;
            const Uint8* pixel = &data[pixel_offset];

            for (int y = beg_y; y < end_y; ++y) {
                int offset = ((63 - y) * 64) + x;
                buffer[offset] = ::g_palette[*pixel++];
            }

            min_y = std::min(min_y, 63 - beg_y);
            max_y = std::max(max_y, 64 - end_y);

            end_y = Endian::le(reader.read_u16()) / 2;
        }
    }

    int height = max_y - min_y + 1;

    OglTexture* texture = new OglTexture(64, 64, OTF_RGBA, &buffer[0]);

    if (!texture->is_initialized()) {
        delete texture;
        return false;
    }

    x_ = first_post;
    y_ = min_y;

    width_ = width;
    height_ = height;

    const float r_64_f = 1.0F / 64.0F;

    tc_x0_ = x_ * r_64_f;
    tc_y0_ = y_ * r_64_f;

    tc_x1_ = (x_ + width_) * r_64_f;
    tc_y1_ = (y_ + height_) * r_64_f;

    texture_ = texture;

    return true;
}

void Sprite::uninitialize()
{
    if (!is_initialized())
        return;

    x_ = 0;
    y_ = 0;

    width_ = 0;
    height_ = 0;

    delete texture_;
    texture_ = NULL;
}

OglTexture* Sprite::get_texture()
{
    return texture_;
}

bool Sprite::is_initialized() const
{
    return texture_ != NULL;
}


} // namespace bstone
