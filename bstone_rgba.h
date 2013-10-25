#ifndef BSTONE_RGBA_H
#define BSTONE_RGBA_H


#include "SDL.h"


namespace bstone {


class Rgba {
public:
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;

    Rgba();

    explicit Rgba(
        Uint8 value);

    Rgba(
        Uint8 r,
        Uint8 g,
        Uint8 b,
        Uint8 a = 255);

    Uint8& operator[](
        int index);

    Uint8 operator[](
        int index) const;
}; // class Rgba


} // namespace bstone


#endif // BSTONE_RGBA_H
