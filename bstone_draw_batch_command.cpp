#include "bstone_draw_batch_command.h"


namespace bstone {


DrawBatchCommand::DrawBatchCommand()
{
    reset();
}

DrawBatchCommand::~DrawBatchCommand()
{
}

DrawBatchCommand::DrawBatchCommand(
    const DrawBatchCommand& that) :
        type(that.type),
        x(that.x),
        y(that.y),
        width(that.width),
        height(that.height),
        latch8_index(that.latch8_index),
        color_index(that.color_index),
        font(that.font),
        sprite(that.sprite),
        picture(that.picture),
        background(that.background),
        wall(that.wall),
        latches8(that.latches8),
        latch(that.latch),
        text(that.text),
        is_text_centered(that.is_text_centered)
{
}

DrawBatchCommand& DrawBatchCommand::operator=(
    const DrawBatchCommand& that)
{
    if (&that != this) {
        type = that.type;
        x = that.x;
        y = that.y;
        width = that.width;
        height = that.height;
        latch8_index = that.latch8_index;
        color_index = that.color_index;
        font = that.font;
        sprite = that.sprite;
        picture = that.picture;
        background = that.background;
        wall = that.wall;
        latches8 = that.latches8;
        latch = that.latch;
        text = that.text;
        is_text_centered = that.is_text_centered;
    }

    return *this;
}

void DrawBatchCommand::reset()
{
    type = DBCT_NONE;
    x = 0;
    y = 0;
    width = 0;
    height = 0;
    latch8_index = -1;
    color_index = -1;
    font = NULL;
    sprite = NULL;
    picture = NULL;
    background = NULL;
    wall = NULL;
    latches8 = NULL;
    latch = NULL;
    text.clear();
    is_text_centered = false;
}


} // namespace bstone
