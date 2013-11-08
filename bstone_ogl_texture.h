#ifndef BSTONE_OGL_TEXTURE_H
#define BSTONE_OGL_TEXTURE_H


#include "bstone_ogl_api.h"
#include "bstone_rgba.h"


namespace bstone {


enum OglTextureFormat {
    OTF_RGB,
    OTF_RGBA,
}; // enum OglTextureFormat


class OglTexture {
public:
    OglTexture();

    OglTexture(
        int width,
        int height,
        OglTextureFormat data_format,
        const Rgba8U* data);

    ~OglTexture();

    bool initialize(
        int width,
        int height,
        OglTextureFormat data_format,
        const Rgba8U* data);

    void uninitialize();

    bool bind() const;

    bool is_initialized() const;

    int get_width() const;

    int get_height() const;

private:
    GLuint texture_;
    int width_;
    int height_;

    OglTexture(
        const OglTexture& that);

    OglTexture& operator=(
        const OglTexture& that);
}; // class OglTexture


} // namespace bstone


#endif // BSTONE_OGL_TEXTURE_H
