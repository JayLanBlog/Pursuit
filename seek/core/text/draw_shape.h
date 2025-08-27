#pragma once
#include "core/core_global.h"
#include <section/graph/font_def.h>


//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Error rate to calculate how many segments we need to draw a smooth circle,
// taken from https://stackoverflow.com/a/2244088
#ifndef SMOOTH_CIRCLE_ERROR_RATE
#define SMOOTH_CIRCLE_ERROR_RATE    0.5f      // Circle error rate
#endif
#ifndef SPLINE_SEGMENT_DIVISIONS
#define SPLINE_SEGMENT_DIVISIONS      24      // Spline segment divisions
#endif

extern Texture2D texShapes;
extern Rectangle texShapesRec;

void SetShapesTexture(Texture2D texture, Rectangle source);

// Get texture that is used for shapes drawing
Texture2D GetShapesTexture(void);

Rectangle GetShapesTextureRectangle(void);

void DrawPixel(int posX, int posY, Color color);

void DrawPixelV(Vector2 position, Color color);

void DrawGrid(int slices, float spacing);

void DrawCircle(int centerX, int centerY, float radius, Color color);

void DrawCircleV(Vector2 center, float radius, Color color);

// Draw a piece of a circle
void DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color);


void LoadDrawQuad(void);