#pragma once
#include "core/core_global.h"
void DrawRectangleLines(int posX, int posY, int width, int height, Color color);


void DrawRectangle(int posX, int posY, int width, int height, Color color);


void DrawRectangleV(Vector2 position, Vector2 size, Color color);

void DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color);

void DrawRectangleRec(Rectangle rec, Color color);