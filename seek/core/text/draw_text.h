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
unsigned int TextLength(const char* text);
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