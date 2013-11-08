#ifndef BSTONE_RESOURCES_H
#define BSTONE_RESOURCES_H


#include "bstone_background.h"
#include "bstone_font.h"
#include "bstone_latch.h"
#include "bstone_latches8.h"
#include "bstone_picture.h"
#include "bstone_sprite.h"
#include "bstone_wall.h"
#include "bstone_resource_list.h"


namespace bstone {


class Resources {
public:
    typedef ResourceList<Font> Fonts;
    typedef ResourceList<Sprite> Sprites;
    typedef ResourceList<Picture> Pictures;
    typedef ResourceList<Background> Backgrounds;
    typedef ResourceList<Wall> Walls;
    typedef ResourceList<Latch> Latches;

    Resources();

    ~Resources();

    void uninitialize();

    Fonts& get_fonts();

    Sprites& get_sprites();

    Pictures& get_pictures();

    Backgrounds& get_backgrounds();

    Walls& get_walls();

    Latches8& get_latches8();

    Latches& get_latches();

private:
    Fonts fonts_;
    Sprites sprites_;
    Pictures pictures_;
    Backgrounds backgrounds_;
    Walls walls_;
    Latches8 latches8_;
    Latches latches_;

    Resources(
        const Resources& that);

    Resources& operator=(
        const Resources& that);
}; // class Resources


} // namespace bstone


#endif // BSTONE_RESOURCES_H
