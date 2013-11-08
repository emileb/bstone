#ifndef BSTONE_SPRITE_H
#define BSTONE_SPRITE_H


#include "bstone_ogl_texture.h"


namespace bstone {


class Sprite {
public:
    Sprite();

    Sprite(
        int id);

    ~Sprite();

    bool initialize(
        int id);

    void uninitialize();

    OglTexture* get_texture();

    bool is_initialized() const;

private:
    int x_;
    int y_;

    int width_;
    int height_;

    float tc_x0_;
    float tc_y0_;

    float tc_x1_;
    float tc_y1_;

    OglTexture* texture_;

    Sprite(
        const Sprite& that);

    Sprite& operator=(
        const Sprite& that);
}; // class Sprite


} // namespace bstone


#endif // BSTONE_SPRITE_H
