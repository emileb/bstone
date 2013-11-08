#ifndef BSTONE_LATCHES8_H
#define BSTONE_LATCHES8_H


#include <vector>

#include "bstone_ogl_texture.h"


namespace bstone {


class Latch8 {
public:
    float tc_x0;
    float tc_y0;
    float tc_x1;
    float tc_y1;

    Latch8();
}; // class Latch8


class Latches8 {
public:
    Latches8();

    explicit Latches8(
        int id);

    ~Latches8();

    bool initialize(
        int id);

    void uninitialize();

    OglTexture* get_texture();

    bool is_initialized() const;

    int get_width() const;

    int get_height() const;

    const Latch8& get_latch(
        int index) const;

private:
    typedef std::vector<Latch8> Latches;

    OglTexture* texture_;
    Latches latches_;

    Latches8(
        const Latches8& that);

    Latches8& operator=(
        const Latches8& that);

    static void suggest_texture_dimensions(
        int& width,
        int& height);
}; // class Latches8


} // namespace bstone


#endif // BSTONE_LATCHES8_H
