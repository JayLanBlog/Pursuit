#include "draw_shape.h"
#include <rmd/gl/gl_include.h>
#include <file/sys_text.h>
#include <core/core.h>
#include <math/pr_math.h>
#include <logger/logger.h>
#include <core/pass/draw_pass.h>
using namespace System;
using namespace Text;
using namespace DRAW::GL;

static Texture2D texShapes = { 1, 1, 1, 1, 7 };                // Texture used on shapes drawing (white pixel loaded by rlgl)
static Rectangle texShapesRec = { 0.0f, 0.0f, 1.0f, 1.0f };    // Texture source rectangle used on shapes drawing



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