#include "bstone_resources.h"


namespace bstone {


Resources::Resources()
{
}

Resources::~Resources()
{
    uninitialize();
}

void Resources::uninitialize()
{
    get_fonts().clear();
    get_sprites().clear();
    get_pictures().clear();
    get_backgrounds().clear();
    get_walls().clear();
    get_latches8().uninitialize();
    get_latches().clear();
}

Resources::Fonts& Resources::get_fonts()
{
    return fonts_;
}

Resources::Sprites& Resources::get_sprites()
{
    return sprites_;
}

Resources::Pictures& Resources::get_pictures()
{
    return pictures_;
}

Resources::Backgrounds& Resources::get_backgrounds()
{
    return backgrounds_;
}

Resources::Walls& Resources::get_walls()
{
    return walls_;
}

Latches8& Resources::get_latches8()
{
    return latches8_;
}

Resources::Latches& Resources::get_latches()
{
    return latches_;
}


} // namespace bstone
