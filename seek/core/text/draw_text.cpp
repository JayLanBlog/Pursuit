#include "draw_text.h"
#include <rmd/gl/gl_include.h>
#include <file/sys_text.h>
#include <core/core.h>
#include <math/pr_math.h>
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
using namespace System;
using namespace Text;
using namespace DRAW::GL;

extern bool isGpuReady;
#if defined(SUPPORT_MODULE_RTEXT) && defined(SUPPORT_DEFAULT_FONT)
void LoadFontDefault(void) {
#define BIT_CHECK(a,b) ((a) & (1u << (b)))

    // check to see if we have allready allocated the font for an image, and if we don't need to upload, then just return
    if (defaultFont.glyphs != NULL && !isGpuReady)
        return;

    // NOTE: Using UTF-8 encoding table for Unicode U+0000..U+00FF Basic Latin + Latin-1 Supplement
    // Ref: http://www.utf8-chartable.de/unicode-utf8-table.pl

    defaultFont.glyphCount = 224;   // Number of chars included in our default font
    defaultFont.glyphPadding = 0;   // Characters padding

    // Default font is directly defined here (data generated from a sprite font image)
    // This way, we reconstruct Font without creating large global variables
    // This data is automatically allocated to Stack and automatically deallocated at the end of this function
    unsigned int defaultFontData[512] = {
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200020, 0x0001b000, 0x00000000, 0x00000000, 0x8ef92520, 0x00020a00, 0x7dbe8000, 0x1f7df45f,
        0x4a2bf2a0, 0x0852091e, 0x41224000, 0x10041450, 0x2e292020, 0x08220812, 0x41222000, 0x10041450, 0x10f92020, 0x3efa084c, 0x7d22103c, 0x107df7de,
        0xe8a12020, 0x08220832, 0x05220800, 0x10450410, 0xa4a3f000, 0x08520832, 0x05220400, 0x10450410, 0xe2f92020, 0x0002085e, 0x7d3e0281, 0x107df41f,
        0x00200000, 0x8001b000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc0000fbe, 0xfbf7e00f, 0x5fbf7e7d, 0x0050bee8, 0x440808a2, 0x0a142fe8, 0x50810285, 0x0050a048,
        0x49e428a2, 0x0a142828, 0x40810284, 0x0048a048, 0x10020fbe, 0x09f7ebaf, 0xd89f3e84, 0x0047a04f, 0x09e48822, 0x0a142aa1, 0x50810284, 0x0048a048,
        0x04082822, 0x0a142fa0, 0x50810285, 0x0050a248, 0x00008fbe, 0xfbf42021, 0x5f817e7d, 0x07d09ce8, 0x00008000, 0x00000fe0, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000c0180,
        0xdfbf4282, 0x0bfbf7ef, 0x42850505, 0x004804bf, 0x50a142c6, 0x08401428, 0x42852505, 0x00a808a0, 0x50a146aa, 0x08401428, 0x42852505, 0x00081090,
        0x5fa14a92, 0x0843f7e8, 0x7e792505, 0x00082088, 0x40a15282, 0x08420128, 0x40852489, 0x00084084, 0x40a16282, 0x0842022a, 0x40852451, 0x00088082,
        0xc0bf4282, 0xf843f42f, 0x7e85fc21, 0x3e0900bf, 0x00000000, 0x00000004, 0x00000000, 0x000c0180, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x04000402, 0x41482000, 0x00000000, 0x00000800,
        0x04000404, 0x4100203c, 0x00000000, 0x00000800, 0xf7df7df0, 0x514bef85, 0xbefbefbe, 0x04513bef, 0x14414500, 0x494a2885, 0xa28a28aa, 0x04510820,
        0xf44145f0, 0x474a289d, 0xa28a28aa, 0x04510be0, 0x14414510, 0x494a2884, 0xa28a28aa, 0x02910a00, 0xf7df7df0, 0xd14a2f85, 0xbefbe8aa, 0x011f7be0,
        0x00000000, 0x00400804, 0x20080000, 0x00000000, 0x00000000, 0x00600f84, 0x20080000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0xac000000, 0x00000f01, 0x00000000, 0x00000000, 0x24000000, 0x00000f01, 0x00000000, 0x06000000, 0x24000000, 0x00000f01, 0x00000000, 0x09108000,
        0x24fa28a2, 0x00000f01, 0x00000000, 0x013e0000, 0x2242252a, 0x00000f52, 0x00000000, 0x038a8000, 0x2422222a, 0x00000f29, 0x00000000, 0x010a8000,
        0x2412252a, 0x00000f01, 0x00000000, 0x010a8000, 0x24fbe8be, 0x00000f01, 0x00000000, 0x0ebe8000, 0xac020000, 0x00000f01, 0x00000000, 0x00048000,
        0x0003e000, 0x00000f00, 0x00000000, 0x00008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000038, 0x8443b80e, 0x00203a03,
        0x02bea080, 0xf0000020, 0xc452208a, 0x04202b02, 0xf8029122, 0x07f0003b, 0xe44b388e, 0x02203a02, 0x081e8a1c, 0x0411e92a, 0xf4420be0, 0x01248202,
        0xe8140414, 0x05d104ba, 0xe7c3b880, 0x00893a0a, 0x283c0e1c, 0x04500902, 0xc4400080, 0x00448002, 0xe8208422, 0x04500002, 0x80400000, 0x05200002,
        0x083e8e00, 0x04100002, 0x804003e0, 0x07000042, 0xf8008400, 0x07f00003, 0x80400000, 0x04000022, 0x00000000, 0x00000000, 0x80400000, 0x04000002,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00800702, 0x1848a0c2, 0x84010000, 0x02920921, 0x01042642, 0x00005121, 0x42023f7f, 0x00291002,
        0xefc01422, 0x7efdfbf7, 0xefdfa109, 0x03bbbbf7, 0x28440f12, 0x42850a14, 0x20408109, 0x01111010, 0x28440408, 0x42850a14, 0x2040817f, 0x01111010,
        0xefc78204, 0x7efdfbf7, 0xe7cf8109, 0x011111f3, 0x2850a932, 0x42850a14, 0x2040a109, 0x01111010, 0x2850b840, 0x42850a14, 0xefdfbf79, 0x03bbbbf7,
        0x001fa020, 0x00000000, 0x00001000, 0x00000000, 0x00002070, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x08022800, 0x00012283, 0x02430802, 0x01010001, 0x8404147c, 0x20000144, 0x80048404, 0x00823f08, 0xdfbf4284, 0x7e03f7ef, 0x142850a1, 0x0000210a,
        0x50a14684, 0x528a1428, 0x142850a1, 0x03efa17a, 0x50a14a9e, 0x52521428, 0x142850a1, 0x02081f4a, 0x50a15284, 0x4a221428, 0xf42850a1, 0x03efa14b,
        0x50a16284, 0x4a521428, 0x042850a1, 0x0228a17a, 0xdfbf427c, 0x7e8bf7ef, 0xf7efdfbf, 0x03efbd0b, 0x00000000, 0x04000000, 0x00000000, 0x00000008,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200508, 0x00840400, 0x11458122, 0x00014210,
        0x00514294, 0x51420800, 0x20a22a94, 0x0050a508, 0x00200000, 0x00000000, 0x00050000, 0x08000000, 0xfefbefbe, 0xfbefbefb, 0xfbeb9114, 0x00fbefbe,
        0x20820820, 0x8a28a20a, 0x8a289114, 0x3e8a28a2, 0xfefbefbe, 0xfbefbe0b, 0x8a289114, 0x008a28a2, 0x228a28a2, 0x08208208, 0x8a289114, 0x088a28a2,
        0xfefbefbe, 0xfbefbefb, 0xfa2f9114, 0x00fbefbe, 0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000020, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00210100, 0x00000004, 0x00000000, 0x00000000, 0x14508200, 0x00001402, 0x00000000, 0x00000000,
        0x00000010, 0x00000020, 0x00000000, 0x00000000, 0xa28a28be, 0x00002228, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000,
        0xa28a28aa, 0x000022a8, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000, 0xbefbefbe, 0x00003e2f, 0x00000000, 0x00000000,
        0x00000004, 0x00002028, 0x00000000, 0x00000000, 0x80000000, 0x00003e0f, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

    int charsHeight = 10;
    int charsDivisor = 1;    // Every char is separated from the consecutive by a 1 pixel divisor, horizontally and vertically

    int charsWidth[224] = { 3, 1, 4, 6, 5, 7, 6, 2, 3, 3, 5, 5, 2, 4, 1, 7, 5, 2, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 3, 4, 3, 6,
                            7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 5, 6, 5, 7, 6, 6, 6, 6, 6, 6, 7, 6, 7, 7, 6, 6, 6, 2, 7, 2, 3, 5,
                            2, 5, 5, 5, 5, 5, 4, 5, 5, 1, 2, 5, 2, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 3, 1, 3, 4, 4,
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                            1, 1, 5, 5, 5, 7, 1, 5, 3, 7, 3, 5, 4, 1, 7, 4, 3, 5, 3, 3, 2, 5, 6, 1, 2, 2, 3, 5, 6, 6, 6, 6,
                            6, 6, 6, 6, 6, 6, 7, 6, 6, 6, 6, 6, 3, 3, 3, 3, 7, 6, 6, 6, 6, 6, 6, 5, 6, 6, 6, 6, 6, 6, 4, 6,
                            5, 5, 5, 5, 5, 5, 9, 5, 5, 5, 5, 5, 2, 2, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5 };

    // Re-construct image from defaultFontData and generate OpenGL texture
    //----------------------------------------------------------------------
    Image imFont = {
       CALLOC(128 * 128, 2),  // 2 bytes per pixel (gray + alpha)
       128,
      128,
      1,
      PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA
    };

    // Fill image.data with defaultFontData (convert from bit to pixel!)
    for (int i = 0, counter = 0; i < imFont.width * imFont.height; i += 32)
    {
        for (int j = 31; j >= 0; j--)
        {
            if (BIT_CHECK(defaultFontData[counter], j))
            {
                // NOTE: We are unreferencing data as short, so,
                // we must consider data as little-endian order (alpha + gray)
                ((unsigned short*)imFont.data)[i + j] = 0xffff;
            }
            else
            {
                ((unsigned char*)imFont.data)[(i + j) * sizeof(short)] = 0xff;
                ((unsigned char*)imFont.data)[(i + j) * sizeof(short) + 1] = 0x00;
            }
        }

        counter++;
    }

    if (isGpuReady)
    {
        defaultFont.texture = LoadTextureFromImage(imFont);

        // we have already loaded the font glyph data an image, and the GPU is ready, we are done
        // if we don't do this, we will leak memory by reallocating the glyphs and rects
        if (defaultFont.glyphs != NULL)
        {
            UnloadImage(imFont);
            return;
        }
    }

    // Reconstruct charSet using charsWidth[], charsHeight, charsDivisor, glyphCount
    //------------------------------------------------------------------------------

    // Allocate space for our characters info data
    // NOTE: This memory must be freed at end! --> Done by CloseWindow()
    defaultFont.glyphs = (GlyphInfo*)CALLOC(defaultFont.glyphCount, sizeof(GlyphInfo));
    defaultFont.recs = (Rectangle*)CALLOC(defaultFont.glyphCount, sizeof(Rectangle));

    int currentLine = 0;
    int currentPosX = charsDivisor;
    int testPosX = charsDivisor;

    for (int i = 0; i < defaultFont.glyphCount; i++)
    {
        defaultFont.glyphs[i].value = 32 + i;  // First char is 32

        defaultFont.recs[i].x = (float)currentPosX;
        defaultFont.recs[i].y = (float)(charsDivisor + currentLine * (charsHeight + charsDivisor));
        defaultFont.recs[i].width = (float)charsWidth[i];
        defaultFont.recs[i].height = (float)charsHeight;

        testPosX += (int)(defaultFont.recs[i].width + (float)charsDivisor);

        if (testPosX >= imFont.width)
        {
            currentLine++;
            currentPosX = 2 * charsDivisor + charsWidth[i];
            testPosX = currentPosX;

            defaultFont.recs[i].x = (float)charsDivisor;
            defaultFont.recs[i].y = (float)(charsDivisor + currentLine * (charsHeight + charsDivisor));
        }
        else currentPosX = testPosX;

        // NOTE: On default font character offsets and xAdvance are not required
        defaultFont.glyphs[i].offsetX = 0;
        defaultFont.glyphs[i].offsetY = 0;
        defaultFont.glyphs[i].advanceX = 0;

        // Fill character image data from fontClear data
        defaultFont.glyphs[i].image = ImageFromImage(imFont, defaultFont.recs[i]);
    }

    UnloadImage(imFont);

    defaultFont.baseSize = (int)defaultFont.recs[0].height;

    TRACELOG(LOG_INFO, "FONT: Default font loaded successfully (%i glyphs)", defaultFont.glyphCount);
}

void UnloadFontDefault(void) {
    for (int i = 0; i < defaultFont.glyphCount; i++) UnloadImage(defaultFont.glyphs[i].image);
    if (isGpuReady) UnloadTexture(defaultFont.texture.id);
    FREE(defaultFont.glyphs);
    FREE(defaultFont.recs);
    defaultFont.glyphCount = 0;
    defaultFont.glyphs = NULL;
    defaultFont.recs = NULL;
}
#endif
//----------------------------------------------------------------------------------
// Text strings management functions
//----------------------------------------------------------------------------------
// Get text length in bytes, check for \0 character
unsigned int TextLength(const char* text)
{
    unsigned int length = 0;

    if (text != NULL)
    {
        // NOTE: Alternative: use strlen(text)

        while (*text++) length++;
    }

    return length;
}

#if defined(SUPPORT_DEFAULT_FONT)
// Default font provided by raylib
// NOTE: Default font is loaded on InitWindow() and disposed on CloseWindow() [module: core]
static Font defaultFont = { 0 };
#endif
static int textLineSpacing = 2;


// Get the default font, useful to be used with extended parameters
Font GetFontDefault() {
#if defined(SUPPORT_DEFAULT_FONT)
    return defaultFont;
#else
    Font font = { 0 };
    return font;
#endif
}

// Check if a font is valid (font data loaded)
// WARNING: GPU texture not checked
bool IsFontValid(Font font) {
    return ((font.baseSize > 0) &&      // Validate font size
        (font.glyphCount > 0) &&    // Validate font contains some glyph
        (font.recs != NULL) &&      // Validate font recs defining glyphs on texture atlas
        (font.glyphs != NULL));     // Validate glyph data is loaded

}

void DrawText(const char* text, int posX, int posY, int fontSize, Color color) {
    
     if (GetFontDefault().texture.id != 0)
    {
        
        Vector2 position = { (float)posX, (float)posY };

        int defaultFontSize = 10;   // Default Font chars height in pixel
        if (fontSize < defaultFontSize) fontSize = defaultFontSize;
        int spacing = fontSize / defaultFontSize;

        DrawTextEx(GetFontDefault(), text, position, (float)fontSize, (float)spacing, color);
    }
}
// Draw current FPS
// NOTE: Uses default font
void DrawFPS(int posX, int posY) {
    Color color = LIME;                         // Good FPS
    int fps = GetFPS();
    if ((fps < 30) && (fps >= 15)) color = ORANGE;  // Warning FPS
    else if (fps < 15) color = RED;             // Low FPS
    //TRACELOG(LOG_INFO,"Current FPS : %d", fps);
    DrawText(TextFormat("%2i FPS", fps), posX, posY, 40, color);
}

// Draw text using Font
// NOTE: chars spacing is NOT proportional to fontSize
void DrawTextEx(Font font, const char* text, Vector2 position, float fontSize, float spacing, Color tint) {
    
    if (font.texture.id == 0) font = GetFontDefault();  // Security check in case of not valid font
    int size = TextLength(text);    // Total size in bytes of the text, scanned by codepoints in loop
    float textOffsetY = 0;          // Offset between lines (on linebreak '\n')
    float textOffsetX = 0.0f;       // Offset X to next character to draw
    float scaleFactor = fontSize / font.baseSize;         // Character quad scaling factor
  
    for (int i = 0; i < size;)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepointNext(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);
       // TRACELOG(LOG_INFO, "current font codepoint : %d £¬ index : %d", codepoint, index);
        if (codepoint == '\n')
        {
            // NOTE: Line spacing is a global variable, use SetTextLineSpacing() to setup
            textOffsetY += (fontSize + textLineSpacing);
            textOffsetX = 0.0f;
        }
        else
        {
            if ((codepoint != ' ') && (codepoint != '\t'))
            {
                DrawTextCodepoint(font, codepoint, { position.x + textOffsetX, position.y + textOffsetY }, fontSize, tint);
            }
            if (font.glyphs[index].advanceX == 0) textOffsetX += ((float)font.recs[index].width * scaleFactor + spacing);
            else textOffsetX += ((float)font.glyphs[index].advanceX * scaleFactor + spacing);
        }
        i += codepointByteCount;   // Move text bytes counter to next codepoint
    }
}
// Get next codepoint in a byte sequence and bytes processed
int GetCodepointNext(const char* text, int* codepointSize) {
    const char* ptr = text;
    int codepoint = 0x3f;       // Codepoint (defaults to '?')
    *codepointSize = 1;

    // Get current codepoint and bytes processed
    if (0xf0 == (0xf8 & ptr[0]))
    {
        // 4 byte UTF-8 codepoint
        if (((ptr[1] & 0xC0) ^ 0x80) || ((ptr[2] & 0xC0) ^ 0x80) || ((ptr[3] & 0xC0) ^ 0x80)) { return codepoint; } // 10xxxxxx checks
        codepoint = ((0x07 & ptr[0]) << 18) | ((0x3f & ptr[1]) << 12) | ((0x3f & ptr[2]) << 6) | (0x3f & ptr[3]);
        *codepointSize = 4;
    }
    else if (0xe0 == (0xf0 & ptr[0]))
    {
        // 3 byte UTF-8 codepoint */
        if (((ptr[1] & 0xC0) ^ 0x80) || ((ptr[2] & 0xC0) ^ 0x80)) { return codepoint; } // 10xxxxxx checks
        codepoint = ((0x0f & ptr[0]) << 12) | ((0x3f & ptr[1]) << 6) | (0x3f & ptr[2]);
        *codepointSize = 3;
    }
    else if (0xc0 == (0xe0 & ptr[0]))
    {
        // 2 byte UTF-8 codepoint
        if ((ptr[1] & 0xC0) ^ 0x80) { return codepoint; } // 10xxxxxx checks
        codepoint = ((0x1f & ptr[0]) << 6) | (0x3f & ptr[1]);
        *codepointSize = 2;
    }
    else if (0x00 == (0x80 & ptr[0]))
    {
        // 1 byte UTF-8 codepoint
        codepoint = ptr[0];
        *codepointSize = 1;
    }


    return codepoint;
}
// Get index position for a unicode character on font
// NOTE: If codepoint is not found in the font it fallbacks to '?'
int GetGlyphIndex(Font font, int codepoint) {
    int index = 0;
    if (!IsFontValid(font)) return index;

#define SUPPORT_UNORDERED_CHARSET
#if defined(SUPPORT_UNORDERED_CHARSET)
    int fallbackIndex = 0;      // Get index of fallback glyph '?'

    // Look for character index in the unordered charset
    for (int i = 0; i < font.glyphCount; i++)
    {
        if (font.glyphs[i].value == 63) fallbackIndex = i;

        if (font.glyphs[i].value == codepoint)
        {
            index = i;
            break;
        }
    }

    if ((index == 0) && (font.glyphs[0].value != codepoint)) index = fallbackIndex;
#else
    index = codepoint - 32;
#endif

    return index;
}
// Draw one character (codepoint)
void DrawTextCodepoint(Font font, int codepoint, Vector2 position, float fontSize, Color tint) {
    // Character index position in sprite font
   // NOTE: In case a codepoint is not available in the font, index returned points to '?'
    int index = GetGlyphIndex(font, codepoint);
    float scaleFactor = fontSize / font.baseSize;     // Character quad scaling factor

    // Character destination rectangle on screen
    // NOTE: We consider glyphPadding on drawing
    Rectangle dstRec = { position.x + font.glyphs[index].offsetX * scaleFactor - (float)font.glyphPadding * scaleFactor,
                      position.y + font.glyphs[index].offsetY * scaleFactor - (float)font.glyphPadding * scaleFactor,
                      (font.recs[index].width + 2.0f * font.glyphPadding) * scaleFactor,
                      (font.recs[index].height + 2.0f * font.glyphPadding) * scaleFactor };
    // Character source rectangle from font texture atlas
 // NOTE: We consider chars padding when drawing, it could be required for outline/glow shader effects
    Rectangle srcRec = { font.recs[index].x - (float)font.glyphPadding, font.recs[index].y - (float)font.glyphPadding,
                         font.recs[index].width + 2.0f * font.glyphPadding, font.recs[index].height + 2.0f * font.glyphPadding };
    // Draw the character texture on the screen
    DrawTexturePro(font.texture, srcRec, dstRec,{ 0, 0 }, 0.0f, tint);
}
// Draw a part of a texture (defined by a rectangle) with 'pro' parameters
// NOTE: origin is relative to destination rectangle size
void DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint) {
    // Check if texture is valid
    if (texture.id > 0)
    {
        float width = (float)texture.width;
        float height = (float)texture.height;
        bool flipX = false;
        if (source.width < 0) { flipX = true; source.width *= -1; }
        if (source.height < 0) source.y -= source.height;
        if (dest.width < 0) dest.width *= -1;
        if (dest.height < 0) dest.height *= -1;
        Vector2 topLeft = { 0 };
        Vector2 topRight = { 0 };
        Vector2 bottomLeft = { 0 };
        Vector2 bottomRight = { 0 };

        // Only calculate rotation if needed
        if (rotation == 0.0f)
        {
            float x = dest.x - origin.x;
            float y = dest.y - origin.y;
            topLeft = { x, y };
            topRight = { x + dest.width, y };
            bottomLeft = { x, y + dest.height };
            bottomRight ={ x + dest.width, y + dest.height };
        }
        else
        {
            float sinRotation = sinf(rotation * DEG2RAD);
            float cosRotation = cosf(rotation * DEG2RAD);
            float x = dest.x;
            float y = dest.y;
            float dx = -origin.x;
            float dy = -origin.y;

            topLeft.x = x + dx * cosRotation - dy * sinRotation;
            topLeft.y = y + dx * sinRotation + dy * cosRotation;

            topRight.x = x + (dx + dest.width) * cosRotation - dy * sinRotation;
            topRight.y = y + (dx + dest.width) * sinRotation + dy * cosRotation;

            bottomLeft.x = x + dx * cosRotation - (dy + dest.height) * sinRotation;
            bottomLeft.y = y + dx * sinRotation + (dy + dest.height) * cosRotation;

            bottomRight.x = x + (dx + dest.width) * cosRotation - (dy + dest.height) * sinRotation;
            bottomRight.y = y + (dx + dest.width) * sinRotation + (dy + dest.height) * cosRotation;
        }
        SetTexture(texture.id);
        Begin(PL_QUADS);

        Color4ub(tint.r, tint.g, tint.b, tint.a);
        Normal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer

        // Top-left corner for texture and quad
        if (flipX) TexCoord2f((source.x + source.width) / width, source.y / height);
        else TexCoord2f(source.x / width, source.y / height);
        Vertex2f(topLeft.x, topLeft.y);

        // Bottom-left corner for texture and quad
        if (flipX) TexCoord2f((source.x + source.width) / width, (source.y + source.height) / height);
        else TexCoord2f(source.x / width, (source.y + source.height) / height);
        Vertex2f(bottomLeft.x, bottomLeft.y);

        // Bottom-right corner for texture and quad
        if (flipX) TexCoord2f(source.x / width, (source.y + source.height) / height);
        else TexCoord2f((source.x + source.width) / width, (source.y + source.height) / height);
        Vertex2f(bottomRight.x, bottomRight.y);

        // Top-right corner for texture and quad
        if (flipX) TexCoord2f(source.x / width, source.y / height);
        else TexCoord2f((source.x + source.width) / width, source.y / height);
        Vertex2f(topRight.x, topRight.y);
        End();
        SetTexture(0);

    }
}