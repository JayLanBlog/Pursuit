#include "draw_text.h"
#include <rmd/gl/gl_include.h>
#include <file/sys_text.h>
#include <core/core.h>
#include <math/pr_math.h>
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <file/image.h>

#if defined(SUPPORT_FILEFORMAT_TTF)
#if defined(__GNUC__) // GCC and Clang
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <imstb_truetype.h>
#if defined(__GNUC__) // GCC and Clang
#pragma GCC diagnostic pop
#endif

#endif

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
//unsigned int TextLength(const char* text)
//{
//    unsigned int length = 0;
//
//    if (text != NULL)
//    {
//        // NOTE: Alternative: use strlen(text)
//
//        while (*text++) length++;
//    }
//
//    return length;
//}

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


void DrawTextureToFrameBuffer(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint) {
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
            bottomRight = { x + dest.width, y + dest.height };
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
        else  TexCoord2f((source.x + source.width) / width, (source.y + source.height) / height);
        // else TexCoord2f(source.x / width, source.y / height); // 0 , 0
        Vertex2f(topLeft.x,  topLeft.y); // 0.0

        // Bottom-left corner for texture and quad
        if (flipX) TexCoord2f((source.x + source.width) / width, (source.y + source.height) / height);
      //  else TexCoord2f(source.x / width, (source.y + source.height) / height); // 0 ,1
        else TexCoord2f((source.x + source.width) / width, source.y / height);
        Vertex2f(bottomLeft.x, bottomLeft.y);

        // Bottom-right corner for texture and quad
        if (flipX) TexCoord2f(source.x / width, (source.y + source.height) / height);
        else TexCoord2f(source.x / width, source.y / height); // 0 , 0
        // else TexCoord2f((source.x + source.width) / width, (source.y + source.height) / height);
        Vertex2f(bottomRight.x, bottomRight.y);

        // Top-right corner for texture and quad
        if (flipX) TexCoord2f(source.x / width, source.y / height);
        //else TexCoord2f((source.x + source.width) / width, source.y / height);
        else TexCoord2f(source.x / width, (source.y + source.height) / height); // 0 ,1
        Vertex2f(topRight.x, topRight.y);
        End();
        SetTexture(0);

    }



}// Load Font from file into GPU memory (VRAM)
Font LoadFont(const char* fileName)
{
    // Default values for ttf font generation
#ifndef FONT_TTF_DEFAULT_SIZE
#define FONT_TTF_DEFAULT_SIZE           32      // TTF font generation default char size (char-height)
#endif
#ifndef FONT_TTF_DEFAULT_NUMCHARS
#define FONT_TTF_DEFAULT_NUMCHARS       95      // TTF font generation default charset: 95 glyphs (ASCII 32..126)
#endif
#ifndef FONT_TTF_DEFAULT_FIRST_CHAR
#define FONT_TTF_DEFAULT_FIRST_CHAR     32      // TTF font generation default first char for image sprite font (32-Space)
#endif
#ifndef FONT_TTF_DEFAULT_CHARS_PADDING
#define FONT_TTF_DEFAULT_CHARS_PADDING   4      // TTF font generation default chars padding
#endif

    Font font = { 0 };

#if defined(SUPPORT_FILEFORMAT_TTF)
    if (IsFileExtension(fileName, ".ttf") || IsFileExtension(fileName, ".otf")) font = LoadFontEx(fileName, FONT_TTF_DEFAULT_SIZE, NULL, FONT_TTF_DEFAULT_NUMCHARS);
    else
#endif
#if defined(SUPPORT_FILEFORMAT_FNT)
        if (IsFileExtension(fileName, ".fnt"))
        {
            int a = 1;
            font = LoadBMFont(fileName);
        }
           
        else
#endif
#if defined(SUPPORT_FILEFORMAT_BDF)
            if (IsFileExtension(fileName, ".bdf")) font = LoadFontEx(fileName, FONT_TTF_DEFAULT_SIZE, NULL, FONT_TTF_DEFAULT_NUMCHARS);
            else
#endif
            {
                Image image = LoadImageSource(fileName);
                if (image.data != NULL) font = LoadFontFromImage(image, MAGENTA, FONT_TTF_DEFAULT_FIRST_CHAR);
                UnloadImage(image);
            }

    if (isGpuReady)
    {
        if (font.texture.id == 0) TRACELOG(LOG_WARNING, "FONT: [%s] Failed to load font texture -> Using default font", fileName);
        else
        {
            SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);    // By default, we set point filter (the best performance)
            TRACELOG(LOG_INFO, "FONT: Data loaded successfully (%i pixel size | %i glyphs)", FONT_TTF_DEFAULT_SIZE, FONT_TTF_DEFAULT_NUMCHARS);
        }
    }

    return font;
}

// Load an Image font file (XNA style)
Font LoadFontFromImage(Image image, Color key, int firstChar) {
#ifndef MAX_GLYPHS_FROM_IMAGE
#define MAX_GLYPHS_FROM_IMAGE   256     // Maximum number of glyphs supported on image scan
#endif

#define COLOR_EQUAL(col1, col2) ((col1.r == col2.r) && (col1.g == col2.g) && (col1.b == col2.b) && (col1.a == col2.a))

    Font font = GetFontDefault();

    int charSpacing = 0;
    int lineSpacing = 0;

    int x = 0;
    int y = 0;

    // We allocate a temporal arrays for chars data measures,
    // once we get the actual number of chars, we copy data to a sized arrays
    int tempCharValues[MAX_GLYPHS_FROM_IMAGE] = { 0 };
    Rectangle tempCharRecs[MAX_GLYPHS_FROM_IMAGE] = { 0 };

    Color* pixels = LoadImageColors(image);

    // Parse image data to get charSpacing and lineSpacing
    for (y = 0; y < image.height; y++)
    {
        for (x = 0; x < image.width; x++)
        {
            if (!COLOR_EQUAL(pixels[y * image.width + x], key)) break;
        }

        if (!COLOR_EQUAL(pixels[y * image.width + x], key)) break;
    }

    if ((x == 0) || (y == 0)) return font; // Security check

    charSpacing = x;
    lineSpacing = y;

    int charHeight = 0;
    int j = 0;

    while (!COLOR_EQUAL(pixels[(lineSpacing + j) * image.width + charSpacing], key)) j++;

    charHeight = j;

    // Check array values to get characters: value, x, y, w, h
    int index = 0;
    int lineToRead = 0;
    int xPosToRead = charSpacing;

    // Parse image data to get rectangle sizes
    while ((lineSpacing + lineToRead * (charHeight + lineSpacing)) < image.height)
    {
        while ((xPosToRead < image.width) &&
            !COLOR_EQUAL((pixels[(lineSpacing + (charHeight + lineSpacing) * lineToRead) * image.width + xPosToRead]), key))
        {
            tempCharValues[index] = firstChar + index;

            tempCharRecs[index].x = (float)xPosToRead;
            tempCharRecs[index].y = (float)(lineSpacing + lineToRead * (charHeight + lineSpacing));
            tempCharRecs[index].height = (float)charHeight;

            int charWidth = 0;

            while (!COLOR_EQUAL(pixels[(lineSpacing + (charHeight + lineSpacing) * lineToRead) * image.width + xPosToRead + charWidth], key)) charWidth++;

            tempCharRecs[index].width = (float)charWidth;

            index++;

            xPosToRead += (charWidth + charSpacing);
        }

        lineToRead++;
        xPosToRead = charSpacing;
    }

    // NOTE: We need to remove key color borders from image to avoid weird
    // artifacts on texture scaling when using TEXTURE_FILTER_BILINEAR or TEXTURE_FILTER_TRILINEAR
    for (int i = 0; i < image.height * image.width; i++) if (COLOR_EQUAL(pixels[i], key)) pixels[i] = BLANK;

    // Create a new image with the processed color data (key color replaced by BLANK)
    Image fontClear = {
         pixels,
         image.width,
         image.height,
         1,
         PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    // Set font with all data parsed from image
    if (isGpuReady) font.texture = LoadTextureFromImage(fontClear); // Convert processed image to OpenGL texture
    font.glyphCount = index;
    font.glyphPadding = 0;

    // We got tempCharValues and tempCharsRecs populated with chars data
    // Now we move temp data to sized charValues and charRecs arrays
    font.glyphs = (GlyphInfo*)MALLOC(font.glyphCount * sizeof(GlyphInfo));
    font.recs = (Rectangle*)MALLOC(font.glyphCount * sizeof(Rectangle));

    for (int i = 0; i < font.glyphCount; i++)
    {
        font.glyphs[i].value = tempCharValues[i];

        // Get character rectangle in the font atlas texture
        font.recs[i] = tempCharRecs[i];

        // NOTE: On image based fonts (XNA style), character offsets and xAdvance are not required (set to 0)
        font.glyphs[i].offsetX = 0;
        font.glyphs[i].offsetY = 0;
        font.glyphs[i].advanceX = 0;

        // Fill character image data from fontClear data
        font.glyphs[i].image = ImageFromImage(fontClear, tempCharRecs[i]);
    }

    UnloadImage(fontClear);     // Unload processed image once converted to texture

    font.baseSize = (int)font.recs[0].height;

    return font;
}

GlyphInfo* LoadFontData(const unsigned char* fileData, int dataSize, int fontSize, int* codepoints, int codepointCount, int type) {
    // NOTE: Using some SDF generation default values,
 // trades off precision with ability to handle *smaller* sizes
#ifndef FONT_SDF_CHAR_PADDING
#define FONT_SDF_CHAR_PADDING            4      // SDF font generation char padding
#endif
#ifndef FONT_SDF_ON_EDGE_VALUE
#define FONT_SDF_ON_EDGE_VALUE         128      // SDF font generation on edge value
#endif
#ifndef FONT_SDF_PIXEL_DIST_SCALE
#define FONT_SDF_PIXEL_DIST_SCALE     64.0f     // SDF font generation pixel distance scale
#endif
#ifndef FONT_BITMAP_ALPHA_THRESHOLD
#define FONT_BITMAP_ALPHA_THRESHOLD     80      // Bitmap (B&W) font generation alpha threshold
#endif

    GlyphInfo* chars = NULL;

#if defined(SUPPORT_FILEFORMAT_TTF)
    // Load font data (including pixel data) from TTF memory file
    // NOTE: Loaded information should be enough to generate font image atlas, using any packaging method
    if (fileData != NULL)
    {
        bool genFontChars = false;
        stbtt_fontinfo fontInfo = { 0 };

        if (stbtt_InitFont(&fontInfo, (unsigned char*)fileData, 0))     // Initialize font for data reading
        {
            // Calculate font scale factor
            float scaleFactor = stbtt_ScaleForPixelHeight(&fontInfo, (float)fontSize);

            // Calculate font basic metrics
            // NOTE: ascent is equivalent to font baseline
            int ascent, descent, lineGap;
            stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

            // In case no chars count provided, default to 95
            codepointCount = (codepointCount > 0) ? codepointCount : 95;

            // Fill fontChars in case not provided externally
            // NOTE: By default we fill glyphCount consecutively, starting at 32 (Space)
            if (codepoints == NULL)
            {
                codepoints = (int*)MALLOC(codepointCount * sizeof(int));
                for (int i = 0; i < codepointCount; i++) codepoints[i] = i + 32;
                genFontChars = true;
            }

            chars = (GlyphInfo*)CALLOC(codepointCount, sizeof(GlyphInfo));

            // NOTE: Using simple packaging, one char after another
            for (int i = 0; i < codepointCount; i++)
            {
                int chw = 0, chh = 0;   // Character width and height (on generation)
                int ch = codepoints[i];  // Character value to get info for
                chars[i].value = ch;

                //  Render a unicode codepoint to a bitmap
                //      stbtt_GetCodepointBitmap()           -- allocates and returns a bitmap
                //      stbtt_GetCodepointBitmapBox()        -- how big the bitmap must be
                //      stbtt_MakeCodepointBitmap()          -- renders into bitmap you provide

                // Check if a glyph is available in the font
                // WARNING: if (index == 0), glyph not found, it could fallback to default .notdef glyph (if defined in font)
                int index = stbtt_FindGlyphIndex(&fontInfo, ch);

                if (index > 0)
                {
                    switch (type)
                    {
                    case FONT_DEFAULT:
                    case FONT_BITMAP: chars[i].image.data = stbtt_GetCodepointBitmap(&fontInfo, scaleFactor, scaleFactor, ch, &chw, &chh, &chars[i].offsetX, &chars[i].offsetY); break;
                    case FONT_SDF: if (ch != 32) chars[i].image.data = stbtt_GetCodepointSDF(&fontInfo, scaleFactor, ch, FONT_SDF_CHAR_PADDING, FONT_SDF_ON_EDGE_VALUE, FONT_SDF_PIXEL_DIST_SCALE, &chw, &chh, &chars[i].offsetX, &chars[i].offsetY); break;
                    default: break;
                    }

                    if (chars[i].image.data != NULL)    // Glyph data has been found in the font
                    {
                        stbtt_GetCodepointHMetrics(&fontInfo, ch, &chars[i].advanceX, NULL);
                        chars[i].advanceX = (int)((float)chars[i].advanceX * scaleFactor);

                        if (chh > fontSize) TRACELOG(LOG_WARNING, "FONT: Character [0x%08x] size is bigger than expected font size", ch);

                        // Load characters images
                        chars[i].image.width = chw;
                        chars[i].image.height = chh;
                        chars[i].image.mipmaps = 1;
                        chars[i].image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;

                        chars[i].offsetY += (int)((float)ascent * scaleFactor);
                    }

                    // NOTE: We create an empty image for space character,
                    // it could be further required for atlas packing
                    if (ch == 32)
                    {
                        stbtt_GetCodepointHMetrics(&fontInfo, ch, &chars[i].advanceX, NULL);
                        chars[i].advanceX = (int)((float)chars[i].advanceX * scaleFactor);

                        Image imSpace = {
                            CALLOC(chars[i].advanceX * fontSize, 2),
                            chars[i].advanceX,
                            fontSize,
                             1,
                            PIXELFORMAT_UNCOMPRESSED_GRAYSCALE
                        };

                        chars[i].image = imSpace;
                    }

                    if (type == FONT_BITMAP)
                    {
                        // Aliased bitmap (black & white) font generation, avoiding anti-aliasing
                        // NOTE: For optimum results, bitmap font should be generated at base pixel size
                        for (int p = 0; p < chw * chh; p++)
                        {
                            if (((unsigned char*)chars[i].image.data)[p] < FONT_BITMAP_ALPHA_THRESHOLD) ((unsigned char*)chars[i].image.data)[p] = 0;
                            else ((unsigned char*)chars[i].image.data)[p] = 255;
                        }
                    }
                }
                else
                {
                    // TODO: Use some fallback glyph for codepoints not found in the font
                }
            }
        }
        else TRACELOG(LOG_WARNING, "FONT: Failed to process TTF font data");

        if (genFontChars) FREE(codepoints);
    }
#endif

    return chars;
}


// Generate image font atlas using chars info
// NOTE: Packing method: 0-Default, 1-Skyline
#if defined(SUPPORT_FILEFORMAT_TTF) || defined(SUPPORT_FILEFORMAT_BDF)
int GetLine(const char* origin, char* buffer, int maxLength) {
    int count = 0;
    for (; count < maxLength - 1; count++) if (origin[count] == '\n') break;
    memcpy(buffer, origin, count);
    buffer[count] = '\0';
    return count;
}

Image GenImageFontAtlas(const GlyphInfo* glyphs, Rectangle** glyphRecs, int glyphCount, int fontSize, int padding, int packMethod) {
    Image atlas = { 0 };

    if (glyphs == NULL)
    {
        TRACELOG(LOG_WARNING, "FONT: Provided chars info not valid, returning empty image atlas");
        return atlas;
    }

    *glyphRecs = NULL;

    // In case no chars count provided we suppose default of 95
    glyphCount = (glyphCount > 0) ? glyphCount : 95;

    // NOTE: Rectangles memory is loaded here!
    Rectangle* recs = (Rectangle*)MALLOC(glyphCount * sizeof(Rectangle));

    // Calculate image size based on total glyph width and glyph row count
    int totalWidth = 0;
    int maxGlyphWidth = 0;

    for (int i = 0; i < glyphCount; i++)
    {
        if (glyphs[i].image.width > maxGlyphWidth) maxGlyphWidth = glyphs[i].image.width;
        totalWidth += glyphs[i].image.width + 2 * padding;
    }

    //#define SUPPORT_FONT_ATLAS_SIZE_CONSERVATIVE
#if defined(SUPPORT_FONT_ATLAS_SIZE_CONSERVATIVE)
    int rowCount = 0;
    int imageSize = 64;  // Define minimum starting value to avoid unnecessary calculation steps for very small images

    // NOTE: maxGlyphWidth is maximum possible space left at the end of row
    while (totalWidth > (imageSize - maxGlyphWidth) * rowCount)
    {
        imageSize *= 2;                                 // Double the size of image (to keep POT)
        rowCount = imageSize / (fontSize + 2 * padding);    // Calculate new row count for the new image size
    }

    atlas.width = imageSize;   // Atlas bitmap width
    atlas.height = imageSize;  // Atlas bitmap height
#else
    int paddedFontSize = fontSize + 2 * padding;
    // No need for a so-conservative atlas generation
    float totalArea = totalWidth * paddedFontSize * 1.2f;
    float imageMinSize = sqrtf(totalArea);
    int imageSize = (int)powf(2, ceilf(logf(imageMinSize) / logf(2)));

    if (totalArea < ((imageSize * imageSize) / 2))
    {
        atlas.width = imageSize;    // Atlas bitmap width
        atlas.height = imageSize / 2; // Atlas bitmap height
    }
    else
    {
        atlas.width = imageSize;   // Atlas bitmap width
        atlas.height = imageSize;  // Atlas bitmap height
    }
#endif

    atlas.data = (unsigned char*)CALLOC(1, atlas.width * atlas.height);   // Create a bitmap to store characters (8 bpp)
    atlas.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
    atlas.mipmaps = 1;

    // DEBUG: We can see padding in the generated image setting a gray background...
    //for (int i = 0; i < atlas.width*atlas.height; i++) ((unsigned char *)atlas.data)[i] = 100;

    if (packMethod == 0)   // Use basic packing algorithm
    {
        int offsetX = padding;
        int offsetY = padding;

        // NOTE: Using simple packaging, one char after another
        for (int i = 0; i < glyphCount; i++)
        {
            // Check remaining space for glyph
            if (offsetX >= (atlas.width - glyphs[i].image.width - 2 * padding))
            {
                offsetX = padding;

                // NOTE: Be careful on offsetY for SDF fonts, by default SDF
                // use an internal padding of 4 pixels, it means char rectangle
                // height is bigger than fontSize, it could be up to (fontSize + 8)
                offsetY += (fontSize + 2 * padding);

                if (offsetY > (atlas.height - fontSize - padding))
                {
                    for (int j = i + 1; j < glyphCount; j++)
                    {
                        TRACELOG(LOG_WARNING, "FONT: Failed to package character (%i)", j);
                        // Make sure remaining recs contain valid data
                        recs[j].x = 0;
                        recs[j].y = 0;
                        recs[j].width = 0;
                        recs[j].height = 0;
                    }
                    break;
                }
            }

            // Copy pixel data from glyph image to atlas
            for (int y = 0; y < glyphs[i].image.height; y++)
            {
                for (int x = 0; x < glyphs[i].image.width; x++)
                {
                    ((unsigned char*)atlas.data)[(offsetY + y) * atlas.width + (offsetX + x)] = ((unsigned char*)glyphs[i].image.data)[y * glyphs[i].image.width + x];
                }
            }

            // Fill chars rectangles in atlas info
            recs[i].x = (float)offsetX;
            recs[i].y = (float)offsetY;
            recs[i].width = (float)glyphs[i].image.width;
            recs[i].height = (float)glyphs[i].image.height;

            // Move atlas position X for next character drawing
            offsetX += (glyphs[i].image.width + 2 * padding);
        }
    }
    else if (packMethod == 1)  // Use Skyline rect packing algorithm (stb_pack_rect)
    {
        stbrp_context* context = (stbrp_context*)MALLOC(sizeof(*context));
        stbrp_node* nodes = (stbrp_node*)MALLOC(glyphCount * sizeof(*nodes));

        stbrp_init_target(context, atlas.width, atlas.height, nodes, glyphCount);
        stbrp_rect* rects = (stbrp_rect*)MALLOC(glyphCount * sizeof(stbrp_rect));

        // Fill rectangles for packaging
        for (int i = 0; i < glyphCount; i++)
        {
            rects[i].id = i;
            rects[i].w = glyphs[i].image.width + 2 * padding;
            rects[i].h = glyphs[i].image.height + 2 * padding;
        }

        // Package rectangles into atlas
        stbrp_pack_rects(context, rects, glyphCount);

        for (int i = 0; i < glyphCount; i++)
        {
            // It returns char rectangles in atlas
            recs[i].x = rects[i].x + (float)padding;
            recs[i].y = rects[i].y + (float)padding;
            recs[i].width = (float)glyphs[i].image.width;
            recs[i].height = (float)glyphs[i].image.height;

            if (rects[i].was_packed)
            {
                // Copy pixel data from fc.data to atlas
                for (int y = 0; y < glyphs[i].image.height; y++)
                {
                    for (int x = 0; x < glyphs[i].image.width; x++)
                    {
                        ((unsigned char*)atlas.data)[(rects[i].y + padding + y) * atlas.width + (rects[i].x + padding + x)] = ((unsigned char*)glyphs[i].image.data)[y * glyphs[i].image.width + x];
                    }
                }
            }
            else TRACELOG(LOG_WARNING, "FONT: Failed to package character (%i)", i);
        }

        FREE(rects);
        FREE(nodes);
        FREE(context);
    }

#if defined(SUPPORT_FONT_ATLAS_WHITE_REC)
    // Add a 3x3 white rectangle at the bottom-right corner of the generated atlas,
    // useful to use as the white texture to draw shapes with raylib, using this rectangle
    // shapes and text can be backed into a single draw call: SetShapesTexture()
    for (int i = 0, k = atlas.width * atlas.height - 1; i < 3; i++)
    {
        ((unsigned char*)atlas.data)[k - 0] = 255;
        ((unsigned char*)atlas.data)[k - 1] = 255;
        ((unsigned char*)atlas.data)[k - 2] = 255;
        k -= atlas.width;
    }
#endif

    // Convert image data from GRAYSCALE to GRAY_ALPHA
    unsigned char* dataGrayAlpha = (unsigned char*)MALLOC(atlas.width * atlas.height * sizeof(unsigned char) * 2); // Two channels

    for (int i = 0, k = 0; i < atlas.width * atlas.height; i++, k += 2)
    {
        dataGrayAlpha[k] = 255;
        dataGrayAlpha[k + 1] = ((unsigned char*)atlas.data)[i];
    }

    FREE(atlas.data);
    atlas.data = dataGrayAlpha;
    atlas.format = PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA;

    *glyphRecs = recs;

    return atlas;
}
#endif

Font LoadFontFromMemory(const char* fileType, const unsigned char* fileData, int dataSize, int fontSize, int* codepoints, int codepointCount) {
    Font font = { 0 };

    char fileExtLower[16] = { 0 };
    strncpy(fileExtLower, TextToLower(fileType), 16 - 1);

    font.baseSize = fontSize;
    font.glyphCount = (codepointCount > 0) ? codepointCount : 95;
    font.glyphPadding = 0;

#if defined(SUPPORT_FILEFORMAT_TTF)
    if (TextIsEqual(fileExtLower, ".ttf") ||
        TextIsEqual(fileExtLower, ".otf"))
    {
        font.glyphs = LoadFontData(fileData, dataSize, font.baseSize, codepoints, font.glyphCount, FONT_DEFAULT);
    }
    else
#endif
#if defined(SUPPORT_FILEFORMAT_BDF)
        if (TextIsEqual(fileExtLower, ".bdf"))
        {
            font.glyphs = LoadFontDataBDF(fileData, dataSize, codepoints, font.glyphCount, &font.baseSize);
        }
        else
#endif
        {
            font.glyphs = NULL;
        }

#if defined(SUPPORT_FILEFORMAT_TTF) || defined(SUPPORT_FILEFORMAT_BDF)
    if (font.glyphs != NULL)
    {
        font.glyphPadding = FONT_TTF_DEFAULT_CHARS_PADDING;

        Image atlas = GenImageFontAtlas(font.glyphs, &font.recs, font.glyphCount, font.baseSize, font.glyphPadding, 0);
        if (isGpuReady) font.texture = LoadTextureFromImage(atlas);

        // Update glyphs[i].image to use alpha, required to be used on ImageDrawText()
        for (int i = 0; i < font.glyphCount; i++)
        {
            UnloadImage(font.glyphs[i].image);
            font.glyphs[i].image = ImageFromImage(atlas, font.recs[i]);
        }

        UnloadImage(atlas);

        TRACELOG(LOG_INFO, "FONT: Data loaded successfully (%i pixel size | %i glyphs)", font.baseSize, font.glyphCount);
    }
    else font = GetFontDefault();
#else
    font = GetFontDefault();
#endif

    return font;
}

Font LoadFontEx(const char* fileName, int fontSize, int* codepoints, int codepointCount) {
    Font font = { 0 };

    // Loading file to memory
    int dataSize = 0;
    unsigned char* fileData = LoadFileData(fileName, &dataSize);

    if (fileData != NULL)
    {
        // Loading font from memory data
        font = LoadFontFromMemory(GetFileExtension(fileName), fileData, dataSize, fontSize, codepoints, codepointCount);

        UnloadFileData(fileData);
    }

    return font;
}


Font LoadBMFont(const char* fileName) {
#define MAX_BUFFER_SIZE       256
#define MAX_FONT_IMAGE_PAGES    8

    Font font = { 0 };

    char buffer[MAX_BUFFER_SIZE] = { 0 };
    char* searchPoint = NULL;

    int fontSize = 0;
    int glyphCount = 0;

    int imWidth = 0;
    int imHeight = 0;
    int pageCount = 1;
    char imFileName[MAX_FONT_IMAGE_PAGES][129] = { 0 };

    int base = 0;       // Useless data
    int readBytes = 0;  // Data bytes read
    int readVars = 0;   // Variables filled by sscanf()

    char* fileText = LoadFileText(fileName);

    if (fileText == NULL) return font;

    char* fileTextPtr = fileText;

    // NOTE: We skip first line, it contains no useful information
    readBytes = GetLine(fileTextPtr, buffer, MAX_BUFFER_SIZE);
    fileTextPtr += (readBytes + 1);

    // Read line data
    readBytes = GetLine(fileTextPtr, buffer, MAX_BUFFER_SIZE);
    searchPoint = strstr(buffer, "lineHeight");
    readVars = sscanf(searchPoint, "lineHeight=%i base=%i scaleW=%i scaleH=%i pages=%i", &fontSize, &base, &imWidth, &imHeight, &pageCount);
    fileTextPtr += (readBytes + 1);

    if (readVars < 4) { UnloadFileText(fileText); return font; } // Some data not available, file malformed

    if (pageCount > MAX_FONT_IMAGE_PAGES)
    {
        TRACELOG(LOG_WARNING, "FONT: [%s] Font defines more pages than supported: %i/%i", fileName, pageCount, MAX_FONT_IMAGE_PAGES);
        pageCount = MAX_FONT_IMAGE_PAGES;
    }

    for (int i = 0; i < pageCount; i++)
    {
        readBytes = GetLine(fileTextPtr, buffer, MAX_BUFFER_SIZE);
        searchPoint = strstr(buffer, "file");
        readVars = sscanf(searchPoint, "file=\"%128[^\"]\"", imFileName[i]);
        fileTextPtr += (readBytes + 1);

        if (readVars < 1) { UnloadFileText(fileText); return font; } // No fileName read
    }

    readBytes = GetLine(fileTextPtr, buffer, MAX_BUFFER_SIZE);
    searchPoint = strstr(buffer, "count");
    readVars = sscanf(searchPoint, "count=%i", &glyphCount);
    fileTextPtr += (readBytes + 1);

    if (readVars < 1) { UnloadFileText(fileText); return font; } // No glyphCount read

    // Load all required images for further compose
    Image* imFonts = (Image*)CALLOC(pageCount, sizeof(Image)); // Font atlases, multiple images

    for (int i = 0; i < pageCount; i++)
    {
        imFonts[i] = LoadImageSource(TextFormat("%s/%s", GetDirectoryPath(fileName), imFileName[i]));

        if (imFonts[i].format == PIXELFORMAT_UNCOMPRESSED_GRAYSCALE)
        {
            // Convert image to GRAYSCALE + ALPHA, using the mask as the alpha channel
            Image imFontAlpha = {
                 CALLOC(imFonts[i].width * imFonts[i].height, 2),
                 imFonts[i].width,
                 imFonts[i].height,
                 1,
                PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA
            };

            for (int p = 0, pi = 0; p < (imFonts[i].width * imFonts[i].height * 2); p += 2, pi++)
            {
                ((unsigned char*)(imFontAlpha.data))[p] = 0xff;
                ((unsigned char*)(imFontAlpha.data))[p + 1] = ((unsigned char*)imFonts[i].data)[pi];
            }

            UnloadImage(imFonts[i]);
            imFonts[i] = imFontAlpha;
        }
    }

    Image fullFont = imFonts[0];
    for (int i = 1; i < pageCount; i++) UnloadImage(imFonts[i]);

    // If multiple atlas, then merge atlas
    // NOTE: WARNING: This process could be really slow!
    if (pageCount > 1)
    {
        // Resize font atlas to draw additional images
        ImageResizeCanvas(&fullFont, imWidth, imHeight * pageCount, 0, 0, BLACK);

        for (int i = 1; i < pageCount; i++)
        {
            Rectangle srcRec = { 0.0f, 0.0f, (float)imWidth, (float)imHeight };
            Rectangle destRec = { 0.0f, (float)imHeight * (float)i, (float)imWidth, (float)imHeight };
            ImageDraw(&fullFont, imFonts[i], srcRec, destRec, WHITE);
        }
    }

    FREE(imFonts);

    if (isGpuReady) font.texture = LoadTextureFromImage(fullFont);

    // Fill font characters info data
    font.baseSize = fontSize;
    font.glyphCount = glyphCount;
    font.glyphPadding = 0;
    font.glyphs = (GlyphInfo*)MALLOC(glyphCount * sizeof(GlyphInfo));
    font.recs = (Rectangle*)MALLOC(glyphCount * sizeof(Rectangle));

    int charId, charX, charY, charWidth, charHeight, charOffsetX, charOffsetY, charAdvanceX, pageID;

    for (int i = 0; i < glyphCount; i++)
    {
        readBytes = GetLine(fileTextPtr, buffer, MAX_BUFFER_SIZE);
        readVars = sscanf(buffer, "char id=%i x=%i y=%i width=%i height=%i xoffset=%i yoffset=%i xadvance=%i page=%i",
            &charId, &charX, &charY, &charWidth, &charHeight, &charOffsetX, &charOffsetY, &charAdvanceX, &pageID);
        fileTextPtr += (readBytes + 1);

        if (readVars == 9)  // Make sure all char data has been properly read
        {
            // Get character rectangle in the font atlas texture
            font.recs[i] ={ (float)charX, (float)charY + (float)imHeight * pageID, (float)charWidth, (float)charHeight };

            // Save data properly in sprite font
            font.glyphs[i].value = charId;
            font.glyphs[i].offsetX = charOffsetX;
            font.glyphs[i].offsetY = charOffsetY;
            font.glyphs[i].advanceX = charAdvanceX;

            // Fill character image data from full font data
            font.glyphs[i].image = ImageFromImage(fullFont, font.recs[i]);
        }
        else
        {
            font.glyphs[i].image = GenImageColor((int)font.recs[i].width, (int)font.recs[i].height, BLACK);
            TRACELOG(LOG_WARNING, "FONT: [%s] Some characters data not correctly provided", fileName);
        }
    }

    UnloadImage(fullFont);
    UnloadFileText(fileText);

    if (isGpuReady && (font.texture.id == 0))
    {
        UnloadFont(font);
        font = GetFontDefault();
        TRACELOG(LOG_WARNING, "FONT: [%s] Failed to load texture, reverted to default font", fileName);
    }
    else TRACELOG(LOG_INFO, "FONT: [%s] Font loaded successfully (%i glyphs)", fileName, font.glyphCount);

    return font;
}   
// Unload font glyphs info data (RAM)
void UnloadFontData(GlyphInfo* glyphs, int glyphCount) {
    if (glyphs != NULL)
    {
        for (int i = 0; i < glyphCount; i++) UnloadImage(glyphs[i].image);

        FREE(glyphs);
    }
}



// Draw an image (source) within an image (destination)
// NOTE: Color tint is applied to source image
void ImageDraw(Image* dst, Image src, Rectangle srcRec, Rectangle dstRec, Color tint) {
    // Security check to avoid program crash
    if ((dst->data == NULL) || (dst->width == 0) || (dst->height == 0) ||
        (src.data == NULL) || (src.width == 0) || (src.height == 0)) return;

    if (dst->format >= PIXELFORMAT_COMPRESSED_DXT1_RGB) TRACELOG(LOG_WARNING, "Image drawing not supported for compressed formats");
    else
    {
        Image srcMod = { 0 };       // Source copy (in case it was required)
        Image* srcPtr = &src;       // Pointer to source image
        bool useSrcMod = false;     // Track source copy required

        // Source rectangle out-of-bounds security checks
        if (srcRec.x < 0) { srcRec.width += srcRec.x; srcRec.x = 0; }
        if (srcRec.y < 0) { srcRec.height += srcRec.y; srcRec.y = 0; }
        if ((srcRec.x + srcRec.width) > src.width) srcRec.width = src.width - srcRec.x;
        if ((srcRec.y + srcRec.height) > src.height) srcRec.height = src.height - srcRec.y;

        // Check if source rectangle needs to be resized to destination rectangle
        // In that case, we make a copy of source, and we apply all required transform
        if (((int)srcRec.width != (int)dstRec.width) || ((int)srcRec.height != (int)dstRec.height))
        {
            srcMod = ImageFromImage(src, srcRec);   // Create image from another image
            ImageResize(&srcMod, (int)dstRec.width, (int)dstRec.height);   // Resize to destination rectangle
            srcRec = { 0, 0, (float)srcMod.width, (float)srcMod.height };

            srcPtr = &srcMod;
            useSrcMod = true;
        }

        // Destination rectangle out-of-bounds security checks
        if (dstRec.x < 0)
        {
            srcRec.x -= dstRec.x;
            srcRec.width += dstRec.x;
            dstRec.x = 0;
        }
        else if ((dstRec.x + srcRec.width) > dst->width) srcRec.width = dst->width - dstRec.x;

        if (dstRec.y < 0)
        {
            srcRec.y -= dstRec.y;
            srcRec.height += dstRec.y;
            dstRec.y = 0;
        }
        else if ((dstRec.y + srcRec.height) > dst->height) srcRec.height = dst->height - dstRec.y;

        if (dst->width < srcRec.width) srcRec.width = (float)dst->width;
        if (dst->height < srcRec.height) srcRec.height = (float)dst->height;

        // This blitting method is quite fast! The process followed is:
        // for every pixel -> [get_src_format/get_dst_format -> blend -> format_to_dst]
        // Some optimization ideas:
        //    [x] Avoid creating source copy if not required (no resize required)
        //    [x] Optimize ImageResize() for pixel format (alternative: ImageResizeNN())
        //    [x] Optimize ColorAlphaBlend() to avoid processing (alpha = 0) and (alpha = 1)
        //    [x] Optimize ColorAlphaBlend() for faster operations (maybe avoiding divs?)
        //    [x] Consider fast path: no alpha blending required cases (src has no alpha)
        //    [x] Consider fast path: same src/dst format with no alpha -> direct line copy
        //    [-] GetPixelColor(): Get Vector4 instead of Color, easier for ColorAlphaBlend()
        //    [ ] Support f32bit channels drawing

        // TODO: Support PIXELFORMAT_UNCOMPRESSED_R32G32B32A32 and PIXELFORMAT_UNCOMPRESSED_R1616B16A16

        Color colSrc, colDst, blend;
        bool blendRequired = true;

        // Fast path: Avoid blend if source has no alpha to blend
        if ((tint.a == 255) &&
            ((srcPtr->format == PIXELFORMAT_UNCOMPRESSED_GRAYSCALE) ||
                (srcPtr->format == PIXELFORMAT_UNCOMPRESSED_R5G6B5) ||
                (srcPtr->format == PIXELFORMAT_UNCOMPRESSED_R8G8B8) ||
                (srcPtr->format == PIXELFORMAT_UNCOMPRESSED_R32) ||
                (srcPtr->format == PIXELFORMAT_UNCOMPRESSED_R32G32B32) ||
                (srcPtr->format == PIXELFORMAT_UNCOMPRESSED_R16) ||
                (srcPtr->format == PIXELFORMAT_UNCOMPRESSED_R16G16B16)))
            blendRequired = false;

        int strideDst = GetPixelDataSize(dst->width, 1, dst->format);
        int bytesPerPixelDst = strideDst / (dst->width);

        int strideSrc = GetPixelDataSize(srcPtr->width, 1, srcPtr->format);
        int bytesPerPixelSrc = strideSrc / (srcPtr->width);

        unsigned char* pSrcBase = (unsigned char*)srcPtr->data + ((int)srcRec.y * srcPtr->width + (int)srcRec.x) * bytesPerPixelSrc;
        unsigned char* pDstBase = (unsigned char*)dst->data + ((int)dstRec.y * dst->width + (int)dstRec.x) * bytesPerPixelDst;

        for (int y = 0; y < (int)srcRec.height; y++)
        {
            unsigned char* pSrc = pSrcBase;
            unsigned char* pDst = pDstBase;

            // Fast path: Avoid moving pixel by pixel if no blend required and same format
            if (!blendRequired && (srcPtr->format == dst->format)) memcpy(pDst, pSrc, (int)(srcRec.width) * bytesPerPixelSrc);
            else
            {
                for (int x = 0; x < (int)srcRec.width; x++)
                {
                    colSrc = GetPixelColor(pSrc, srcPtr->format);
                    colDst = GetPixelColor(pDst, dst->format);

                    // Fast path: Avoid blend if source has no alpha to blend
                    if (blendRequired) blend = ColorAlphaBlend(colDst, colSrc, tint);
                    else blend = colSrc;

                    SetPixelColor(pDst, blend, dst->format);

                    pDst += bytesPerPixelDst;
                    pSrc += bytesPerPixelSrc;
                }
            }

            pSrcBase += strideSrc;
            pDstBase += strideDst;
        }

        if (useSrcMod) UnloadImage(srcMod);     // Unload source modified image

        if ((dst->mipmaps > 1) && (src.mipmaps > 1))
        {
            Image mipmapDst = *dst;
            mipmapDst.data = (char*)mipmapDst.data + GetPixelDataSize(mipmapDst.width, mipmapDst.height, mipmapDst.format);
            mipmapDst.width /= 2;
            mipmapDst.height /= 2;
            mipmapDst.mipmaps--;

            Image mipmapSrc = src;
            mipmapSrc.data = (char*)mipmapSrc.data + GetPixelDataSize(mipmapSrc.width, mipmapSrc.height, mipmapSrc.format);
            mipmapSrc.width /= 2;
            mipmapSrc.height /= 2;
            mipmapSrc.mipmaps--;

            Rectangle mipmapSrcRec = srcRec;
            mipmapSrcRec.width /= 2;
            mipmapSrcRec.height /= 2;
            mipmapSrcRec.x /= 2;
            mipmapSrcRec.y /= 2;

            Rectangle mipmapDstRec = dstRec;
            mipmapDstRec.width /= 2;
            mipmapDstRec.height /= 2;
            mipmapDstRec.x /= 2;
            mipmapDstRec.y /= 2;

            ImageDraw(&mipmapDst, mipmapSrc, mipmapSrcRec, mipmapDstRec, tint);
        }
    }
}

// Unload Font from GPU memory (VRAM)
void UnloadFont(Font font) {
    // NOTE: Make sure font is not default font (fallback)
    if (font.texture.id != GetFontDefault().texture.id)
    {
        UnloadFontData(font.glyphs, font.glyphCount);
        if (isGpuReady) UnloadTexture(font.texture.id);
        FREE(font.recs);

        TRACELOGD("FONT: Unloaded font data from RAM and VRAM");
    }
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