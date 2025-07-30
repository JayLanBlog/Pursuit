#pragma once
#include <config/platform_def.h>

// rmodels: Configuration values
//------------------------------------------------------------------------------------
#define MAX_MATERIAL_MAPS              12       // Maximum number of shader maps supported


namespace Seek {
    // Color, 4 components, R8G8B8A8 (32bit)
    typedef struct Color {
        unsigned char r;        // Color red value
        unsigned char g;        // Color green value
        unsigned char b;        // Color blue value
        unsigned char a;        // Color alpha value
    } Color;

#define RL_COLOR_TYPE
#define RL_RECTANGLE_TYPE
#define RL_VECTOR2_TYPE
#define RL_VECTOR3_TYPE
#define RL_VECTOR4_TYPE
#define RL_QUATERNION_TYPE
#define RL_MATRIX_TYPE

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define LIGHTGRAY  CLITERAL(Color){ 200, 200, 200, 255 }   // Light Gray
#define GRAY       CLITERAL(Color){ 130, 130, 130, 255 }   // Gray
#define DARKGRAY   CLITERAL(Color){ 80, 80, 80, 255 }      // Dark Gray
#define YELLOW     CLITERAL(Color){ 253, 249, 0, 255 }     // Yellow
#define GOLD       CLITERAL(Color){ 255, 203, 0, 255 }     // Gold
#define ORANGE     CLITERAL(Color){ 255, 161, 0, 255 }     // Orange
#define PINK       CLITERAL(Color){ 255, 109, 194, 255 }   // Pink
#define RED        CLITERAL(Color){ 230, 41, 55, 255 }     // Red
#define MAROON     CLITERAL(Color){ 190, 33, 55, 255 }     // Maroon
#define GREEN      CLITERAL(Color){ 0, 228, 48, 255 }      // Green
#define LIME       CLITERAL(Color){ 0, 158, 47, 255 }      // Lime
#define DARKGREEN  CLITERAL(Color){ 0, 117, 44, 255 }      // Dark Green
#define SKYBLUE    CLITERAL(Color){ 102, 191, 255, 255 }   // Sky Blue
#define BLUE       CLITERAL(Color){ 0, 121, 241, 255 }     // Blue
#define DARKBLUE   CLITERAL(Color){ 0, 82, 172, 255 }      // Dark Blue
#define PURPLE     CLITERAL(Color){ 200, 122, 255, 255 }   // Purple
#define VIOLET     CLITERAL(Color){ 135, 60, 190, 255 }    // Violet
#define DARKPURPLE CLITERAL(Color){ 112, 31, 126, 255 }    // Dark Purple
#define BEIGE      CLITERAL(Color){ 211, 176, 131, 255 }   // Beige
#define BROWN      CLITERAL(Color){ 127, 106, 79, 255 }    // Brown
#define DARKBROWN  CLITERAL(Color){ 76, 63, 47, 255 }      // Dark Brown

#define WHITE      CLITERAL(Color){ 255, 255, 255, 255 }   // White
#define BLACK      CLITERAL(Color){ 0, 0, 0, 255 }         // Black
#define BLANK      CLITERAL(Color){ 0, 0, 0, 0 }           // Blank (Transparent)
#define MAGENTA    CLITERAL(Color){ 255, 0, 255, 255 }     // Magenta
#define RAYWHITE   CLITERAL(Color){ 245, 245, 245, 255 }   // My own White (raylib logo)


    // Image, pixel data stored in CPU memory (RAM)
    typedef struct Image {
        void* data;             // Image raw data
        int width;              // Image base width
        int height;             // Image base height
        int mipmaps;            // Mipmap levels, 1 by default
        int format;             // Data format (PixelFormat type)
    } Image;

    // Texture, tex data stored in GPU memory (VRAM)
    typedef struct Texture {
        unsigned int id;        // OpenGL texture id
        int width;              // Texture base width
        int height;             // Texture base height
        int mipmaps;            // Mipmap levels, 1 by default
        int format;             // Data format (PixelFormat type)
    } Texture;

    // Texture2D, same as Texture
    typedef Texture Texture2D;

    // TextureCubemap, same as Texture
    typedef Texture TextureCubemap;


    // RenderTexture, fbo for texture rendering
    typedef struct RenderTexture {
        unsigned int id;        // OpenGL framebuffer object id
        Texture texture;        // Color buffer attachment texture
        Texture depth;          // Depth buffer attachment texture
    } RenderTexture;

    // RenderTexture2D, same as RenderTexture
    typedef RenderTexture RenderTexture2D;

    // Shader
    typedef struct Shader {
        unsigned int id;        // Shader program id
        int* locs;              // Shader locations array (PL_MAX_SHADER_LOCATIONS)
    } Shader;

    // MaterialMap
    typedef struct MaterialMap {
        Texture2D texture;      // Material map texture
        Color color;            // Material map color
        float value;            // Material map value
    } MaterialMap;

    // Material, includes shader and maps
    typedef struct Material {
        Shader shader;          // Material shader
        MaterialMap* maps;      // Material maps array (MAX_MATERIAL_MAPS)
        float params[4];        // Material generic parameters (if required)
    } Material;




}