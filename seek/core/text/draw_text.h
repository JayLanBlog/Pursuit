#pragma once
#include "core/core_global.h"
#include <section/graph/font_def.h>

#if defined(SUPPORT_MODULE_RTEXT) && defined(SUPPORT_DEFAULT_FONT)
void LoadFontDefault(void);

void UnloadFontDefault(void);
#endif
extern Font defaultFont;

// Check if a font is valid (font data loaded)
// WARNING: GPU texture not checked
bool IsFontValid(Font font);

// Get the default font, useful to be used with extended parameters
Font GetFontDefault();
// Get text length in bytes, check for \0 character
//unsigned int TextLength(const char* text);
void DrawText(const char* text, int posX, int posY, int fontSize, Color color);
// Draw current FPS
// NOTE: Uses default font
void DrawFPS(int posX, int posY);
// Draw text using Font
// NOTE: chars spacing is NOT proportional to fontSize
void DrawTextEx(Font font, const char* text, Vector2 position, float fontSize, float spacing, Color tint);
// Get next codepoint in a byte sequence and bytes processed
int GetCodepointNext(const char* text, int* codepointSize);
// Get index position for a unicode character on font
// NOTE: If codepoint is not found in the font it fallbacks to '?'
int GetGlyphIndex(Font font, int codepoint);
// Draw one character (codepoint)
void DrawTextCodepoint(Font font, int codepoint, Vector2 position, float fontSize, Color tint);
// Draw a part of a texture (defined by a rectangle) with 'pro' parameters
// NOTE: origin is relative to destination rectangle size
void DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);


void DrawTextureToFrameBuffer(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);

// Load Font from file into GPU memory (VRAM)
Font LoadFont(const char* fileName);

Font LoadBMFont(const char* fileName);

Font LoadFontFromMemory(const char* fileType, const unsigned char* fileData, int dataSize, int fontSize, int* codepoints, int codepointCount);

// Load an Image font file (XNA style)
Font LoadFontFromImage(Image image, Color key, int firstChar);

Font LoadFontEx(const char* fileName, int fontSize, int* codepoints, int codepointCount);


GlyphInfo* LoadFontData(const unsigned char* fileData, int dataSize, int fontSize, int* codepoints, int codepointCount, int type);

// Unload Font from GPU memory (VRAM)
void UnloadFont(Font font);

// Unload font glyphs info data (RAM)
void UnloadFontData(GlyphInfo* glyphs, int glyphCount);

// Draw an image (source) within an image (destination)
// NOTE: Color tint is applied to source image
void ImageDraw(Image* dst, Image src, Rectangle srcRec, Rectangle dstRec, Color tint);

// Generate image font atlas using chars info
// NOTE: Packing method: 0-Default, 1-Skyline
#if defined(SUPPORT_FILEFORMAT_TTF) || defined(SUPPORT_FILEFORMAT_BDF)
Image GenImageFontAtlas(const GlyphInfo* glyphs, Rectangle** glyphRecs, int glyphCount, int fontSize, int padding, int packMethod);

// NOTE: Returns the number of bytes read
int GetLine(const char* origin, char* buffer, int maxLength);

#endif