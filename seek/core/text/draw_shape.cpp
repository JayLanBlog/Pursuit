#include "draw_shape.h"
#include <rmd/gl/gl_include.h>
#include <file/sys_text.h>
#include <core/core.h>
#include <math/pr_math.h>
#include <logger/logger.h>
#include <core/pass/draw_pass.h>
#include <core/pass/draw_pass2d.h>
using namespace System;
using namespace Text;
using namespace DRAW::GL;

static Texture2D texShapes = { 1, 1, 1, 1, 7 };                // Texture used on shapes drawing (white pixel loaded by rlgl)
static Rectangle texShapesRec = { 0.0f, 0.0f, 1.0f, 1.0f };    // Texture source rectangle used on shapes drawing


// Check if point is inside circle
bool CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius)
{
    bool collision = false;

    float distanceSquared = (point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y);

    if (distanceSquared <= radius * radius) collision = true;

    return collision;
}


// Check collision between circle and rectangle
// NOTE: Reviewed version to take into account corner limit case
bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec) {
    bool collision = false;

    float recCenterX = rec.x + rec.width / 2.0f;
    float recCenterY = rec.y + rec.height / 2.0f;

    float dx = fabsf(center.x - recCenterX);
    float dy = fabsf(center.y - recCenterY);

    if (dx > (rec.width / 2.0f + radius)) { return false; }
    if (dy > (rec.height / 2.0f + radius)) { return false; }

    if (dx <= (rec.width / 2.0f)) { return true; }
    if (dy <= (rec.height / 2.0f)) { return true; }

    float cornerDistanceSq = (dx - rec.width / 2.0f) * (dx - rec.width / 2.0f) +
        (dy - rec.height / 2.0f) * (dy - rec.height / 2.0f);

    collision = (cornerDistanceSq <= (radius * radius));

    return collision;
}
// Draw rectangle outline with extended parameters
void DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color) {
    if ((lineThick > rec.width) || (lineThick > rec.height))
    {
        if (rec.width >= rec.height) lineThick = rec.height / 2;
        else if (rec.width <= rec.height) lineThick = rec.width / 2;
    }

    // When rec = { x, y, 8.0f, 6.0f } and lineThick = 2, the following
    // four rectangles are drawn ([T]op, [B]ottom, [L]eft, [R]ight):
    //
    //   TTTTTTTT
    //   TTTTTTTT
    //   LL    RR
    //   LL    RR
    //   BBBBBBBB
    //   BBBBBBBB
    //

    Rectangle top = { rec.x, rec.y, rec.width, lineThick };
    Rectangle bottom = { rec.x, rec.y - lineThick + rec.height, rec.width, lineThick };
    Rectangle left = { rec.x, rec.y + lineThick, lineThick, rec.height - lineThick * 2.0f };
    Rectangle right = { rec.x - lineThick + rec.width, rec.y + lineThick, lineThick, rec.height - lineThick * 2.0f };

    DrawRectangleRec(top, color);
    DrawRectangleRec(bottom, color);
    DrawRectangleRec(left, color);
    DrawRectangleRec(right, color);
}
// Get collision rectangle for two rectangles collision
Rectangle GetCollisionRec(Rectangle rec1, Rectangle rec2)
{
    Rectangle overlap = { 0 };

    float left = (rec1.x > rec2.x) ? rec1.x : rec2.x;
    float right1 = rec1.x + rec1.width;
    float right2 = rec2.x + rec2.width;
    float right = (right1 < right2) ? right1 : right2;
    float top = (rec1.y > rec2.y) ? rec1.y : rec2.y;
    float bottom1 = rec1.y + rec1.height;
    float bottom2 = rec2.y + rec2.height;
    float bottom = (bottom1 < bottom2) ? bottom1 : bottom2;

    if ((left < right) && (top < bottom))
    {
        overlap.x = left;
        overlap.y = top;
        overlap.width = right - left;
        overlap.height = bottom - top;
    }

    return overlap;
}



void DrawCircleV(Vector2 center, float radius, Color color) {
    DrawCircleSector(center, radius, 0, 360, 36, color);
}

void DrawCircle(int centerX, int centerY, float radius, Color color) {
    DrawCircleV({ (float)centerX, (float)centerY }, radius, color);
}


// Draw a piece of a circle
void DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color) {
    if (startAngle == endAngle) return;
    if (radius <= 0.0f) radius = 0.1f;  // Avoid div by zero

    // Function expects (endAngle > startAngle)
    if (endAngle < startAngle)
    {
        // Swap values
        float tmp = startAngle;
        startAngle = endAngle;
        endAngle = tmp;
    }

    int minSegments = (int)ceilf((endAngle - startAngle) / 90);

    if (segments < minSegments)
    {
        // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
        float th = acosf(2 * powf(1 - SMOOTH_CIRCLE_ERROR_RATE / radius, 2) - 1);
        segments = (int)((endAngle - startAngle) * ceilf(2 * PI / th) / 360);

        if (segments <= 0) segments = minSegments;
    }
    float stepLength = (endAngle - startAngle) / (float)segments;
    float angle = startAngle;
#if defined(SUPPORT_QUADS_DRAW_MODE)
    SetTexture(GetShapesTexture().id);
    Rectangle shapeRect = GetShapesTextureRectangle();
    Begin(PL_QUADS);

    // NOTE: Every QUAD actually represents two segments
    for (int i = 0; i < segments / 2; i++)
    {
        Color4ub(color.r, color.g, color.b, color.a);

        TexCoord2f(shapeRect.x / texShapes.width, shapeRect.y / texShapes.height);
        Vertex2f(center.x, center.y);

        TexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, shapeRect.y / texShapes.height);
        Vertex2f(center.x + cosf(DEG2RAD * (angle + stepLength * 2.0f)) * radius, center.y + sinf(DEG2RAD * (angle + stepLength * 2.0f)) * radius);

        TexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
        Vertex2f(center.x + cosf(DEG2RAD * (angle + stepLength)) * radius, center.y + sinf(DEG2RAD * (angle + stepLength)) * radius);

        TexCoord2f(shapeRect.x / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
        Vertex2f(center.x + cosf(DEG2RAD * angle) * radius, center.y + sinf(DEG2RAD * angle) * radius);

        angle += (stepLength * 2.0f);
    }

    // NOTE: In case number of segments is odd, we add one last piece to the cake
    if ((((unsigned int)segments) % 2) == 1)
    {
        Color4ub(color.r, color.g, color.b, color.a);

        TexCoord2f(shapeRect.x / texShapes.width, shapeRect.y / texShapes.height);
        Vertex2f(center.x, center.y);

        TexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
        Vertex2f(center.x + cosf(DEG2RAD * (angle + stepLength)) * radius, center.y + sinf(DEG2RAD * (angle + stepLength)) * radius);

        TexCoord2f(shapeRect.x / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
        Vertex2f(center.x + cosf(DEG2RAD * angle) * radius, center.y + sinf(DEG2RAD * angle) * radius);

        TexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, shapeRect.y / texShapes.height);
        Vertex2f(center.x, center.y);
    }

    SetTexture(0);

    End();
#else
    Begin(RL_TRIANGLES);
    for (int i = 0; i < segments; i++)
    {
        Color4ub(color.r, color.g, color.b, color.a);

        Vertex2f(center.x, center.y);
        Vertex2f(center.x + cosf(DEG2RAD * (angle + stepLength)) * radius, center.y + sinf(DEG2RAD * (angle + stepLength)) * radius);
        Vertex2f(center.x + cosf(DEG2RAD * angle) * radius, center.y + sinf(DEG2RAD * angle) * radius);

        angle += stepLength;
    }
    End();
#endif
}

void LoadDrawQuad(void) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

    float vertices[] = {
        // Positions         Texcoords
       -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
       -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
        1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
    };

    // Gen VAO to contain VBO
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    // Gen and fill vertex buffer (VBO)
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // Bind vertex attributes (position, texcoords)
    glEnableVertexAttribArray(PL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION);
    glVertexAttribPointer(PL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Positions
    glEnableVertexAttribArray(PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD);
    glVertexAttribPointer(PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // Texcoords

    // Draw quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    // Delete buffers (VBO and VAO)
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &quadVAO);
#endif
}


// Draw a grid centered at (0, 0, 0)
void DrawGrid(int slices, float spacing)
{
    int halfSlices = slices / 2;

    Begin(PL_LINES);
    for (int i = -halfSlices; i <= halfSlices; i++)
    {
        if (i == 0)
        {
            Color3f(0.5f, 0.5f, 0.5f);
        }
        else
        {
            Color3f(0.75f, 0.75f, 0.75f);
        }

        Vertex3f((float)i * spacing, 0.0f, (float)-halfSlices * spacing);
        Vertex3f((float)i * spacing, 0.0f, (float)halfSlices * spacing);

        Vertex3f((float)-halfSlices * spacing, 0.0f, (float)i * spacing);
        Vertex3f((float)halfSlices * spacing, 0.0f, (float)i * spacing);
    }
    End();
}


void SetShapesTexture(Texture2D texture, Rectangle source) {
    // Reset texture to default pixel if required
   // WARNING: Shapes texture should be probably better validated,
   // it can break the rendering of all shapes if misused
    if ((texture.id == 0) || (source.width == 0) || (source.height == 0))
    {
        texShapes = { 1, 1, 1, 1, 7 };
        texShapesRec ={ 0.0f, 0.0f, 1.0f, 1.0f };
    }
    else
    {
        texShapes = texture;
        texShapesRec = source;
    }
}

// Get texture that is used for shapes drawing
Texture2D GetShapesTexture(void)
{
    return texShapes;
}

Rectangle GetShapesTextureRectangle(void) {
    return texShapesRec;
}

void DrawPixel(int posX, int posY, Color color) {
    DrawPixelV({ (float)posX, (float)posY }, color);
}


void DrawPixelV(Vector2 position, Color color) {
#if defined(SUPPORT_QUADS_DRAW_MODE)
    SetTexture(GetShapesTexture().id);
    Rectangle shapeRect = GetShapesTextureRectangle();
    Begin(PL_QUADS);
    
    Normal3f(0.0f, 0.0f, 1.0f);
    DRAW::GL::Color4ub(color.r, color.g, color.b, color.a);
    
  //  Color4ub(color.r, color.g, color.b, color.a);
  //  Color4ub("1","1","1","1");
    TexCoord2f(shapeRect.x / texShapes.width, shapeRect.y / texShapes.height);
    Vertex2f(position.x, position.y);

    TexCoord2f(shapeRect.x / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
    Vertex2f(position.x, position.y + 1);

    TexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, (shapeRect.y + shapeRect.height) / texShapes.height);
    Vertex2f(position.x + 1, position.y + 1);

    TexCoord2f((shapeRect.x + shapeRect.width) / texShapes.width, shapeRect.y / texShapes.height);
    Vertex2f(position.x + 1, position.y);

    End();

    SetTexture(0);
#else

    Begin(RL_TRIANGLES);

    Color4ub(color.r, color.g, color.b, color.a);

    Vertex2f(position.x, position.y);
    Vertex2f(position.x, position.y + 1);
    Vertex2f(position.x + 1, position.y);

    Vertex2f(position.x + 1, position.y);
    Vertex2f(position.x, position.y + 1);
    Vertex2f(position.x + 1, position.y + 1);

    End();

#endif
}