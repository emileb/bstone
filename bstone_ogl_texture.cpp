#include "bstone_ogl_texture.h"


namespace bstone {


OglTexture::OglTexture() :
    texture_(),
    width_(),
    height_()
{
}

OglTexture::OglTexture(
    int width,
    int height,
    OglTextureFormat data_format,
    const Rgba* data) :
        texture_(),
        width_(),
        height_()
{
    initialize(width, height, data_format, data);
}

OglTexture::~OglTexture()
{
    uninitialize();
}

bool OglTexture::initialize(
    int width,
    int height,
    OglTextureFormat data_format,
    const Rgba* data)
{
    uninitialize();

    if (width <= 0)
        return false;

    if (height <= 0)
        return false;


    GLenum format;

    switch (data_format) {
    case OTF_RGB:
        format = GL_RGB;
        break;

    case OTF_RGBA:
        format = GL_RGBA;
        break;

    default:
        return false;
    }

    if (data == NULL)
        return false;

    ::glGenTextures(1, &texture_);

    if (!is_initialized())
        return false;

    ::glActiveTexture(GL_TEXTURE0);
    ::glBindTexture(GL_TEXTURE_2D, texture_);

    ::glTexImage2D(GL_TEXTURE_2D, 0, format,
        width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    width_ = width;
    height_ = height;

    return true;
}

void OglTexture::uninitialize()
{
    if (!is_initialized())
        return;

    ::glDeleteTextures(1, &texture_);
    texture_ = GL_NONE;

    width_ = 0;
    height_ = 0;
}

bool OglTexture::bind() const
{
    if (!is_initialized())
        return false;

    ::glBindTexture(GL_TEXTURE_2D, texture_);

    return true;
}

bool OglTexture::is_initialized() const
{
    return texture_ != GL_NONE;
}

int OglTexture::get_width() const
{
    return width_;
}

int OglTexture::get_height() const
{
    return height_;
}


} // namespace bstone
