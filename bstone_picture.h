#ifndef BSTONE_PICTURE_H
#define BSTONE_PICTURE_H


#include "bstone_ogl_texture.h"


namespace bstone {


class Picture {
public:
    Picture();

    explicit Picture(
        int id);

    ~Picture();

    bool initialize(
        int id);

    void uninitialize();

    OglTexture* get_texture();

    bool is_initialized() const;

    int get_width() const;

    int get_height() const;

private:
    int width_;
    int height_;
    OglTexture* texture_;

    Picture(
        const Picture& that);

    Picture& operator=(
        const Picture& that);
}; // class Picture


} // namespace bstone


#endif // BSTONE_PICTURE_H
