#ifndef BSTONE_FONT_H
#define BSTONE_FONT_H


#include <vector>

#include "bstone_ogl_texture.h"


namespace bstone {


class FontChar {
public:
    int width;

    // Texture coordinates
    //

    // left-bottom point
    float tc_x0;
    float tc_y0;

    // right-top point
    float tc_x1;
    float tc_y1;

    FontChar();
}; // class FontChar


class Font {
public:
    Font();

    explicit Font(
        int id);

    ~Font();

    bool initialize(
        int id);

    void uninitialize();

    void measure_string(
        const std::string& string,
        int& width,
        int& height) const;

    void measure_text(
        const std::string& text,
        int& width,
        int& height) const;

    const FontChar& get_char(
        int index) const;

    int get_height() const;

    OglTexture* get_texture();

    bool is_initialized() const;

private:
    typedef std::vector<FontChar> Chars;
    typedef Chars::iterator CharsIt;
    typedef Chars::const_iterator CharsCIt;

    bool is_initialized_;
    int height_;
    Chars chars_;
    OglTexture* texture_;

    Font(
        const Font& that);

    Font& operator=(
        const Font& that);

    static void suggest_texture_dimensions(
        const int* chars_widths,
        int chars_height,
        int& width,
        int& height);
}; // class Font


} // namespace bstone


#endif // BSTONE_FONT_H
