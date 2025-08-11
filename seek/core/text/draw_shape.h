#pragma once
#include "core/core_global.h"
#include <section/graph/font_def.h>


extern Texture2D texShapes;
extern Rectangle texShapesRec;

void SetShapesTexture(Texture2D texture, Rectangle source);

// Get texture that is used for shapes drawing
Texture2D GetShapesTexture(void);

Rectangle GetShapesTextureRectangle(void);

void DrawPixel(int posX, int posY, Color color);

void DrawPixelV(Vector2 position, Color color);

void DrawGrid(int slices, float spacing);


void LoadDrawQuad(void);