#include "draw_pass2d.h"
#include "draw_pass.h"
#include <rmd/gl/gl_include.h>
#include <core/platform/wcore_sdl.h>
#include <math/pr_math.h>
#include <core/core.h>
#include <menu/console.h>
#include <core/gui/draw_ui.h>
#include <section/enum/camera_em.h>
#include <core/text/draw_shape.h>


using namespace PiplineState;
using namespace PMath;
using namespace DRAW::GL;
using namespace RHICore;

void DrawRectangleRec(Rectangle rec, Color color) {
    DrawRectanglePro(rec, { 0.0f, 0.0f }, 0.0f, color);
}

void DrawRectangleLines(int posX, int posY, int width, int height, Color color) {
    Matrix mat = GetMatrixTransform();
    float xOffset = 0.5f / mat.m0;
    float yOffset = 0.5f / mat.m5;

    Begin(PL_LINES);
    Color4ub(color.r, color.g, color.b, color.a);
    Vertex2f((float)posX + xOffset, (float)posY + yOffset);
    Vertex2f((float)posX + (float)width - xOffset, (float)posY + yOffset);

    Vertex2f((float)posX + (float)width - xOffset, (float)posY + yOffset);
    Vertex2f((float)posX + (float)width - xOffset, (float)posY + (float)height - yOffset);

    Vertex2f((float)posX + (float)width - xOffset, (float)posY + (float)height - yOffset);
    Vertex2f((float)posX + xOffset, (float)posY + (float)height - yOffset);

    Vertex2f((float)posX + xOffset, (float)posY + (float)height - yOffset);
    Vertex2f((float)posX + xOffset, (float)posY + yOffset);
    End();
}

void DrawRectangle(int posX, int posY, int width, int height, Color color) {

    DrawRectangleV( { (float)posX, (float)posY }, { (float)width, (float)height }, color);

}

// Draw a color-filled rectangle (Vector version)
// NOTE: On OpenGL 3.3 and ES2 we use QUADS to avoid drawing order issues
void DrawRectangleV(Vector2 position, Vector2 size, Color color)
{
    DrawRectanglePro({ position.x, position.y, size.x, size.y },  { 0.0f, 0.0f }, 0.0f, color);
}

// Draw a color-filled rectangle with pro parameters
void DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color) {
    Vector2 topLeft = { 0 };
    Vector2 topRight = { 0 };
    Vector2 bottomLeft = { 0 };
    Vector2 bottomRight = { 0 };
    // Only calculate rotation if needed
    if (rotation == 0.0f)
    {
        float x = rec.x - origin.x;
        float y = rec.y - origin.y;
        topLeft = { x, y };
        topRight = { x + rec.width, y };
        bottomLeft = { x, y + rec.height };
        bottomRight = { x + rec.width, y + rec.height };
    }
    else
    {
        float sinRotation = sinf(rotation * DEG2RAD);
        float cosRotation = cosf(rotation * DEG2RAD);
        float x = rec.x;
        float y = rec.y;
        float dx = -origin.x;
        float dy = -origin.y;

        topLeft.x = x + dx * cosRotation - dy * sinRotation;
        topLeft.y = y + dx * sinRotation + dy * cosRotation;

        topRight.x = x + (dx + rec.width) * cosRotation - dy * sinRotation;
        topRight.y = y + (dx + rec.width) * sinRotation + dy * cosRotation;

        bottomLeft.x = x + dx * cosRotation - (dy + rec.height) * sinRotation;
        bottomLeft.y = y + dx * sinRotation + (dy + rec.height) * cosRotation;

        bottomRight.x = x + (dx + rec.width) * cosRotation - (dy + rec.height) * sinRotation;
        bottomRight.y = y + (dx + rec.width) * sinRotation + (dy + rec.height) * cosRotation;
    }


#if defined(SUPPORT_QUADS_DRAW_MODE)
    SetTexture(GetShapesTexture().id);
    Rectangle shapeRect = GetShapesTextureRectangle();
    Texture2D texShapes =  GetShapesTexture();
    Begin(PL_QUADS);

    Normal3f(0.0f, 0.0f, 1.0f);
    Color4ub(color.r, color.g, color.b, color.a);

    TexCoord2f(shapeRect.x / texShapes.width, shapeRect.y / texShapes.height);
    Vertex2f(topLeft.x, topLeft.y);

    TexCoord2f(shapeRect.x / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
    Vertex2f(bottomLeft.x, bottomLeft.y);

    TexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
    Vertex2f(bottomRight.x, bottomRight.y);

   // TexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, shapeRect.y / texShapes.height);
    Vertex2f(topRight.x, topRight.y);

    End();

    SetTexture(0);
#else
    Begin(PL_TRIANGLES);

    Color4ub(color.r, color.g, color.b, color.a);

    Vertex2f(topLeft.x, topLeft.y);
    Vertex2f(bottomLeft.x, bottomLeft.y);
    Vertex2f(topRight.x, topRight.y);

    Vertex2f(topRight.x, topRight.y);
    Vertex2f(bottomLeft.x, bottomLeft.y);
    Vertex2f(bottomRight.x, bottomRight.y);

    End();
#endif

}