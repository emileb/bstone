#ifndef BSTONE_BACKGROUND_H
#define BSTONE_BACKGROUND_H


#include "bstone_ogl_texture.h"


namespace bstone {


class Background {
public:
    Background();

    Background(
        int id);

    ~Background();

    bool initialize(
        int id);

    void uninitialize();

    OglTexture* get_texture();

    bool is_initialized() const;

private:
    OglTexture* texture_;

    Background(
        const Background& that);

    Background& operator=(
        const Background& that);
}; // class Background


} // namespace bstone


#endif // BSTONE_BACKGROUND_H
