#ifndef BSTONE_RGBA_H
#define BSTONE_RGBA_H


#include <cassert>

#include "SDL.h"


namespace bstone {


template<class T>
class Rgba {
public:
    T r;
    T g;
    T b;
    T a;

    Rgba() :
        r(),
        g(),
        b(),
        a(get_max_value(T()))
    {
    }

    explicit Rgba(
        T value) :
            r(value),
            g(value),
            b(value),
            a(value)
    {
    }

    Rgba(
        T r,
        T g,
        T b,
        T a = get_max_value(T())) :
            r(r),
            g(g),
            b(b),
            a(a)
    {
    }

    T& operator[](
        int index)
    {
        assert(index >= 0 && index <= 3);
        return (&r)[index];
    }

    const T& operator[](
        int index) const
    {
        assert(index >= 0 && index <= 3);
        return (&r)[index];
    }

    static Rgba<float> from_u8_to_f32(
        const Rgba<Uint8>& u8)
    {
        return Rgba<float>(
            u8.r * get_one_255th(),
            u8.g * get_one_255th(),
            u8.b * get_one_255th(),
            u8.a * get_one_255th());
    }

private:
    template<class U>
    static U get_max_value(
        U);

    template<>
    static Uint8 get_max_value(
        Uint8)
    {
        return 255;
    }

    template<>
    static float get_max_value(
        float)
    {
        return 1.0F;
    }

    static float get_one_255th()
    {
        return 1.0F / 255.0F;
    }
}; // class Rgba


typedef Rgba<Uint8> Rgba8U;
typedef Rgba<float> Rgba32F;


} // namespace bstone


#endif // BSTONE_RGBA_H
