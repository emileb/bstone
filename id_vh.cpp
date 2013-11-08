// ID_VH.C

#include "id_heads.h"


void VL_LatchToScreen(int source, int width, int height, int x, int y);
void IN_StartAck(void);
boolean IN_CheckAck (void);
void CalcTics (void);
void ForceUpdateStatusBar(void);


#define	SCREENWIDTH		80
#define CHARWIDTH		2
#define TILEWIDTH		4
#define GRPLANES		4
#define BYTEPIXELS		4

#define SCREENXMASK		(~3)
#define SCREENXPLUS		(3)
#define SCREENXDIV		(4)

#define VIEWWIDTH		80

#define PIXTOBLOCK		4		// 16 pixels to an update block

//#define UNCACHEGRCHUNK(chunk)	{MM_FreePtr(&grsegs[chunk]);grneeded[chunk]&=~ca_levelbit;}

//Uint8	update[UPDATEHIGH][UPDATEWIDE];	// MIKE this is the second declaration for this variable!?!?

//==========================================================================

pictabletype	*pictable;
pictabletype   *picmtable;			


int px;
int py;
Uint8 fontcolor;
Uint8 backcolor;
int fontnumber;

// FIXME
#if 0
Sint16 bufferwidth,bufferheight;
#endif // 0

boolean allcaps = false;


//==========================================================================

void	VWL_UpdateScreenBlocks (void);

//==========================================================================

// FIXME
#if 0
void VW_DrawPropString (const char* string)
{
    fontstruct* font;
    int width;
    int height;
    size_t i;
    size_t string_length;
    int j;
    int k;
    Uint8* source;
    Uint8 *dest;
    Uint8 ch;

    font = (fontstruct*)grsegs[STARTFONT + fontnumber];
    height = font->height;

    dest = &vga_memory[(4 * bufferofs) + (py * vanilla_screen_width) + px];

    string_length = strlen(string);

    for (i = 0; i < string_length; ++i) {
        ch = string[i];
        width = font->width[ch];
        source = ((Uint8*)font) + font->location[ch];

        for (j = 0; j < width; ++j) {
            for (k = 0; k < height; ++k) {
                if (source[k * width])
                    dest[k * vanilla_screen_width] = fontcolor;
            }

            ++source;
            ++px;
            ++dest;
        }
    }
}
#endif // 0

void VW_DrawPropString(
    const char* string)
{
    int width;
    int height;
    bstone::Font* font = ::g_resources.get_fonts().add(::fontnumber);
    font->measure_string(string, width, height);

    bstone::DrawBatchCommand command;
    command.type = bstone::DBCT_STRING;
    command.font = ::g_resources.get_fonts().add(::fontnumber);
    command.color_index = fontcolor;
    command.x = px;
    command.y = py;
    command.text = string;

    ::g_draw_batch.add_command(command);

    px += width;
}

//==========================================================================

#if 0

/*
=================
=
= VL_MungePic
=
=================
*/

void VL_MungePic (Uint8 *source, unsigned width, unsigned height)
{
	unsigned	x,y,plane,size,pwidth;
	Uint8		*temp, *dest, *srcline;

	size = width*height;

	if (width&3)
		VH_ERROR(VL_MUNGEPIC_NO_DIV_FOUR);

//
// copy the pic to a temp buffer
//
	MM_GetPtr (&(memptr)temp,size);
	_fmemcpy (temp,source,size);

//
// munge it back into the original buffer
//
	dest = source;
	pwidth = width/4;

	for (plane=0;plane<4;plane++)
	{
		srcline = temp;
		for (y=0;y<height;y++)
		{
			for (x=0;x<pwidth;x++)
				*dest++ = *(srcline+x*4+plane);
			srcline+=width;
		}
	}

	MM_FreePtr (&(memptr)temp);
}

#endif

// FIXME
#if 0
void VWL_MeasureString(
    const char* string,
    Uint16* width,
    Uint16* height,
    fontstruct* font)
{
	*height = font->height;
	for (*width = 0;*string;string++)
		*width += font->width[*((const Uint8*)string)];	// proportional width
}
#endif // 0

void VWL_MeasureString(
    const char* string,
    int& width,
    int& height,
    int font_index)
{
    bstone::Font* font = ::g_resources.get_fonts().add(font_index);
    font->measure_string(string, width, height);
}

// FIXME
#if 0
void	VW_MeasurePropString (const char* string, Uint16* width, Uint16* height)
{
	VWL_MeasureString(string,width,height,(fontstruct *)grsegs[STARTFONT+fontnumber]);
}
#endif // 0

void VW_MeasurePropString(
    const char* string,
    int& width,
    int& height)
{
    ::VWL_MeasureString(string, width, height, fontnumber);
}

#if 0

void	VW_MeasureMPropString  (char *string, Uint16 *width, Uint16 *height)
{
	VWL_MeasureString(string,width,height,(fontstruct *)grsegs[STARTFONTM+fontnumber]);
}
#endif


/*
=============================================================================

				Double buffer management routines

=============================================================================
*/


/*
=======================
=
= VW_MarkUpdateBlock
=
= Takes a pixel bounded block and marks the tiles in bufferblocks
= Returns 0 if the entire block is off the buffer screen
=
=======================
*/

Sint16 VW_MarkUpdateBlock (Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2)
{
// FIXME
#if 0
	Sint16	x,y,xt1,yt1,xt2,yt2,nextline;
	Uint8 *mark;

	xt1 = x1>>PIXTOBLOCK;
	yt1 = y1>>PIXTOBLOCK;

	xt2 = x2>>PIXTOBLOCK;
	yt2 = y2>>PIXTOBLOCK;

	if (xt1<0)
		xt1=0;
	else if (xt1>=UPDATEWIDE)
		return 0;

	if (yt1<0)
		yt1=0;
	else if (yt1>UPDATEHIGH)
		return 0;

	if (xt2<0)
		return 0;
	else if (xt2>=UPDATEWIDE)
		xt2 = UPDATEWIDE-1;

	if (yt2<0)
		return 0;
	else if (yt2>=UPDATEHIGH)
		yt2 = UPDATEHIGH-1;

	mark = updateptr + uwidthtable[yt1] + xt1;
	nextline = UPDATEWIDE - (xt2-xt1) - 1;

	for (y=yt1;y<=yt2;y++)
	{
		for (x=xt1;x<=xt2;x++)
			*mark++ = 1;			// this tile will need to be updated

		mark += nextline;
	}
#endif // 0

	return 1;
}

// FIXME
#if 0
void VWB_DrawTile8 (Sint16 x, Sint16 y, Sint16 tile)
{
	if (VW_MarkUpdateBlock (x,y,x+7,y+7))
		LatchDrawChar(x,y,tile);
}
#endif // 0

void VWB_DrawTile8(
    Sint16 x,
    Sint16 y,
    Sint16 tile)
{
    bstone::DrawBatchCommand command;
    command.type = bstone::DBCT_LATCH8;
    command.latch8_index = tile;
    command.latches8 = &::g_resources.get_latches8();
    command.x = x;
    command.y = y;
    ::g_draw_batch.add_command(command);
}

#if 0

void VWB_DrawTile8M (int x, int y, int tile)
{
	if (VW_MarkUpdateBlock (x,y,x+7,y+7))
		VL_MemToScreen (((Uint8 *)grsegs[STARTTILE8M])+tile*64,8,8,x,y);
}
#endif

// FIXME
#if 0
void VWB_DrawPic(int x, int y, int chunknum)
{
	Sint16	picnum = static_cast<Sint16>(chunknum - STARTPICS);
	Uint16 width,height;

	x &= ~7;

	width = pictable[picnum].width;
	height = pictable[picnum].height;

	if (VW_MarkUpdateBlock (static_cast<Sint16>(x),static_cast<Sint16>(y),static_cast<Sint16>(x+width-1),static_cast<Sint16>(y+height-1)))
		VL_MemToScreen (static_cast<const Uint8*>(grsegs[chunknum]),width,height,x,y);
}
#endif // 0

void VWB_DrawPic(
    int x,
    int y,
    int chunknum)
{
    x &= ~7;

    bstone::Picture* picture = ::g_resources.get_pictures().add(chunknum);

    bstone::DrawBatchCommand command;
    command.type = bstone::DBCT_PICTURE;
    command.picture = picture;
    command.x = x;
    command.y = y;
    ::g_draw_batch.add_command(command);
}


//--------------------------------------------------------------------------
// VWB_DrawMPic()
//--------------------------------------------------------------------------
void VWB_DrawMPic (Sint16 x, Sint16 y, Sint16 chunknum)
{
	Sint16	picnum = chunknum - STARTPICS;
	Uint16 width,height;

	width = pictable[picnum].width;
	height = pictable[picnum].height;

	if (VW_MarkUpdateBlock (x,y,x+width-1,y+height-1))
		VL_MaskMemToScreen (static_cast<const Uint8*>(grsegs[chunknum]),width,height,x,y,255);
}


void VWB_DrawPropString(const char* string)
{
// FIXME
#if 0
	Sint16 x;

	x=px;
#endif // 0

	VW_DrawPropString (string);

// FIXME
#if 0
	VW_MarkUpdateBlock(x,py,px-1,py+bufferheight-1);
#endif // 0
}


void VWB_Bar (Sint16 x, Sint16 y, Sint16 width, Sint16 height, Sint16 color)
{
	if (VW_MarkUpdateBlock (x,y,x+width,y+height-1) )
		VW_Bar (x,y,width,height,color);
}

void VWB_Plot (Sint16 x, Sint16 y, Sint16 color)
{
	if (VW_MarkUpdateBlock (x,y,x,y))
		VW_Plot(x,y,color);
}

void VWB_Hlin (Sint16 x1, Sint16 x2, Sint16 y, Sint16 color)
{
	if (VW_MarkUpdateBlock (x1,y,x2,y))
		VW_Hlin(x1,x2,y,color);
}

void VWB_Vlin (Sint16 y1, Sint16 y2, Sint16 x, Sint16 color)
{
	if (VW_MarkUpdateBlock (x,y1,x,y2))
		VW_Vlin(y1,y2,x,color);
}


#if 0		/// MADE A MACRO
void VW_UpdateScreen (void)
{
	VH_UpdateScreen ();
}
#endif


/*
=============================================================================

						WOLFENSTEIN STUFF

=============================================================================
*/

/*
=====================
=
= LatchDrawPic
=
=====================
*/

// FIXME
#if 0
void LatchDrawPic (Uint16 x, Uint16 y, Uint16 picnum)
{
	Uint16 wide, height, source;

	x <<= 3;
	wide = pictable[picnum-STARTPICS].width;
	height = pictable[picnum-STARTPICS].height;
	source = latchpics[2+picnum-LATCHPICS_LUMP_START];

	if (VW_MarkUpdateBlock (x,y,x+wide-1,y+height-1))
		VL_LatchToScreen (source,wide/4,height,x,y);
}
#endif // 0

void LatchDrawPic(
    Uint16 x,
    Uint16 y,
    Uint16 picnum)
{
    bstone::DrawBatchCommand command;
    command.type = bstone::DBCT_LATCH;
    command.latch = ::g_resources.get_latches().add(picnum);
    command.x = x;
    command.y = y;
    ::g_draw_batch.add_command(command);
}


//==========================================================================

/*
===================
=
= LoadLatchMem
=
===================
*/

//unsigned LatchMemFree = 0xffff;		
Uint16	destoff;

// FIXME
#if 0
void LoadLatchMem (void)
{
	Sint16	i,width,height;
	Uint8	*src;
	Uint16	picnum=0;


//
// tile 8s
//
	latchpics[picnum++] = freelatch;
	CA_CacheGrChunk (STARTTILE8);
	src = (Uint8 *)grsegs[STARTTILE8];
	destoff = freelatch;

	for (i=0;i<NUMTILE8;i++)
	{
		VL_MemToLatch (src,8,8,destoff);
		src += 64;
		destoff +=16;
	}
	UNCACHEGRCHUNK (STARTTILE8);

#if 0	// ran out of latch space!
//
// tile 16s
//
	src = (Uint8 *)grsegs[STARTTILE16];
	latchpics[picnum++] = destoff;

	for (i=0;i<NUMTILE16;i++)
	{
		CA_CacheGrChunk (STARTTILE16+i);
		src = (Uint8 *)grsegs[STARTTILE16+i];
		VL_MemToLatch (src,16,16,destoff);
		destoff+=64;
		if (src)
			UNCACHEGRCHUNK (STARTTILE16+i);
	}
#endif

//
// pics
//
	picnum++;
	for (i=LATCHPICS_LUMP_START;i<=LATCHPICS_LUMP_END;i++)	  
	{
		latchpics[picnum++] = destoff;
		CA_CacheGrChunk (i);
		width = pictable[i-STARTPICS].width;
		height = pictable[i-STARTPICS].height;
		VL_MemToLatch (static_cast<const Uint8*>(grsegs[i]),width,height,destoff);
		destoff += width/4 *height;
		UNCACHEGRCHUNK(i);
	}

	EGAMAPMASK(15);
}
#endif // 0

// FIXME
#if 1
void LoadLatchMem()
{
    Sint16 i;
    Sint16 width;
    Sint16 height;
    Uint8* src;

    //
    // tile 8s
    //
    ::CA_CacheGrChunk(STARTTILE8);
    ::g_resources.get_latches8().initialize(STARTTILE8);
    ::UNCACHEGRCHUNK(STARTTILE8);

    //
    // pics
    //
    for (i = LATCHPICS_LUMP_START; i <= LATCHPICS_LUMP_END; ++i) {
        ::CA_CacheGrChunk(i);
        width = ::pictable[i - STARTPICS].width;
        height = ::pictable[i - STARTPICS].height;
        ::g_resources.get_latches().add(i);
        ::UNCACHEGRCHUNK(i);
    }
}
#endif // 0

//==========================================================================


/*
===================
=
= FizzleFade
=
= returns true if aborted
=
===================
*/

extern	ControlInfo	c;

boolean FizzleFade(
    int source,
    int dest,
    int width,
    int height,
    int frames,
    boolean abortable)
{
    int pixperframe;
    int remain_pixels;
    Uint16 x;
    Uint16 y;
    int p;
    int frame;
    Sint32 rndval;
    boolean carry;
    int src_offset;
    int dst_offset;
    int pixel_offset;
    int pixel_count;

    y = 0;
    rndval = 1;
    pixperframe = 64000 / frames;
    remain_pixels = 64000 - (frames * pixperframe);
    src_offset = 4 * source;
    dst_offset = 4 * dest;

    IN_StartAck();

    TimeCount = 0;
    frame = 0;
    LastScan = 0;

    do {
        if (abortable && IN_CheckAck())
            return true;

        pixel_count = pixperframe + remain_pixels;
        remain_pixels = 0;

        for (p = 0; p < pixel_count; ++p) {
            x = (rndval >> 8) & 0xFFFF;
            y = ((rndval & 0xFF) - 1) & 0xFF;

            carry = ((rndval & 1) != 0);

            rndval >>= 1;

            if (carry)
                rndval ^= 0x00012000;

            if (x > width || y > height)
                continue;

            pixel_offset = (y * vanilla_screen_width) + x;

            vga_memory[dst_offset + pixel_offset] =
                vga_memory[src_offset + pixel_offset];

            if (rndval == 1)
                return false;
        }

        VL_RefreshScreen();

        ++frame;

        while (TimeCount < static_cast<Uint32>(frame)) // don't go too fast
            ;

        CalcTics();

        if ((frame & 3) == 0)
            ForceUpdateStatusBar();
    } while (true);
}
