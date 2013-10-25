#include "bstone_rgba.h"

#include <cassert>


namespace bstone {


Rgba::Rgba() :
    r(),
    g(),
    b(),
    a(255)
{
    assert(sizeof(Rgba) == 4);
}

Rgba::Rgba(
    Uint8 value) :
        r(value),
        g(value),
        b(value),
        a(value)
{
    assert(sizeof(Rgba) == 4);
}

Rgba::Rgba(
    Uint8 r,
    Uint8 g,
    Uint8 b,
    Uint8 a) :
        r(r),
        g(g),
        b(b),
        a(a)
{
    assert(sizeof(Rgba) == 4);
}

Uint8& Rgba::operator[](
    int index)
{
    assert(index >= 0 && index <= 3);
    return (&r)[index];
}

Uint8 Rgba::operator[](
    int index) const
{
    assert(index >= 0 && index <= 3);
    return (&r)[index];
}


} // namespace bstone
