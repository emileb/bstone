#ifndef BSTONE_DRAW_BATCH_COMMAND_H
#define BSTONE_DRAW_BATCH_COMMAND_H


#include <string>


namespace bstone {


class Font;
class Sprite;
class Picture;
class Background;
class Wall;
class Latches8;
class Latch;


enum DrawBatchCommandType {
    DBCT_NONE,
    DBCT_BACKGROUND,
    DBCT_STRING,
    DBCT_RECTANGLE,
    DBCT_PICTURE,
    DBCT_SPRITE,
    DBCT_WALL,
    DBCT_LATCH8,
    DBCT_LATCH
}; // enum DrawBatchCommandType


class DrawBatchCommand {
public:
    DrawBatchCommandType type;

    int x;
    int y;
    int width;
    int height;

    int color_index;
    int latch8_index;

    Font* font;
    Sprite* sprite;
    Picture* picture;
    Background* background;
    Wall* wall;
    Latches8* latches8;
    Latch* latch;

    std::string text;
    bool is_text_centered;

    DrawBatchCommand();

    ~DrawBatchCommand();

    DrawBatchCommand(
        const DrawBatchCommand& that);

    DrawBatchCommand& operator=(
        const DrawBatchCommand& that);

    void reset();
}; // class DrawBatchCommand


} // namespace bstone


#endif // BSTONE_DRAW_BATCH_COMMAND_H
