#ifndef BSTONE_WALL_H
#define BSTONE_WALL_H


#include "bstone_ogl_texture.h"


namespace bstone {


class Wall {
public:
    Wall();

    explicit Wall(
        int id);

    ~Wall();

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

    Wall(
        const Wall& that);

    Wall& operator=(
        const Wall& that);
}; // class Wall


} // namespace bstone


#endif // BSTONE_WALL_H
