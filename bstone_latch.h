#ifndef BSTONE_LATCH_H
#define BSTONE_LATCH_H


#include "bstone_ogl_texture.h"


namespace bstone {


class Latch {
public:
    Latch();

    explicit Latch(
        int id);

    ~Latch();

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

    Latch(
        const Latch& that);

    Latch& operator=(
        const Latch& that);
}; // class Latch


} // namespace bstone


#endif // BSTONE_LATCH_H
