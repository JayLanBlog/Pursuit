#pragma once
#include <iostream>
using namespace std;
#define GRAPHICS_API_OPENGL_33

#define SUPPORT_STANDARD_FILEIO

#define SUPPORT_TRACELOG

#define MAX_TRACELOG_MSG_LENGTH       256       // Max length of one trace-log message


#define PL_MAX_SHADER_LOCATIONS        32      // Maximum number of shader locations supported

#define EXIT_SUCCESS	0

#define EXIT_FAILURE	1

//#define SUPPORT_CONSOLE_TRACELOG

#pragma warning(disable:4996)


#if defined(__cplusplus)
#define CLITERAL(type)      type
#else
#define CLITERAL(type)      (type)
#endif

#define SUPPORT_FILEFORMAT_PNG


#ifndef PL_DEFAULT_BATCH_MAX_TEXTURE_UNITS
#define PL_DEFAULT_BATCH_MAX_TEXTURE_UNITS       4      // Maximum number of textures units that can be activated on batch drawing (SetShaderValueTexture())
#endif
// Internal Matrix stack
#ifndef PL_MAX_MATRIX_STACK_SIZE
#define PL_MAX_MATRIX_STACK_SIZE                32      // Maximum size of Matrix stack
#endif


#define SUPPORT_FILEFORMAT_OBJ


#define SUPPORT_FILEFORMAT_GLTF

#ifdef RL_SUPPORT_MESH_GPU_SKINNING
#define MAX_MESH_VERTEX_BUFFERS         9       // Maximum vertex buffers (VBO) per mesh
#else
#define MAX_MESH_VERTEX_BUFFERS         7       // Maximum vertex buffers (VBO) per mesh
#endif

// Function specifiers definition
#if defined(RAYMATH_IMPLEMENTATION)
    #if defined(_WIN32) && defined(BUILD_LIBTYPE_SHARED)
        #define RMAPI __declspec(dllexport) extern inline // We are building raylib as a Win32 shared library (.dll)
    #elif defined(BUILD_LIBTYPE_SHARED)
        #define RMAPI __attribute__((visibility("default"))) // We are building raylib as a Unix shared library (.so/.dylib)
    #elif defined(_WIN32) && defined(USE_LIBTYPE_SHARED)
        #define RMAPI __declspec(dllimport)         // We are using raylib as a Win32 shared library (.dll)
    #else
        #define RMAPI extern inline // Provide external definition
    #endif
#elif defined(RAYMATH_STATIC_INLINE)
    #define RMAPI static inline // Functions may be inlined, no external out-of-line definition
#else
    #if defined(__TINYC__)
        #define RMAPI static inline // plain inline not supported by tinycc (See issue #435)
    #else
        #define RMAPI inline        // Functions may be inlined or external definition used
    #endif
#endif


#if defined(_WIN32)
#if defined(__TINYC__)
#define __declspec(x) __attribute__((x))
#endif
#if defined(BUILD_LIBTYPE_SHARED)
#define PLAPI __declspec(dllexport)     // We are building the library as a Win32 shared library (.dll)
#elif defined(USE_LIBTYPE_SHARED)
#define PLAPI __declspec(dllimport)     // We are using the library as a Win32 shared library (.dll)
#endif
#else
#if defined(BUILD_LIBTYPE_SHARED)
#define PLAPI __attribute__((visibility("default"))) // We are building as a Unix shared library (.so/.dylib)
#endif
#endif

#ifndef PLAPI
#define PLAPI       // Functions defined as 'extern' by default (implicit specifiers)
#endif

#define PLATFORM_DESKTOP_SDL


// Default font is loaded on window initialization to be available for the user to render simple text
// NOTE: If enabled, uses external module functions to load default raylib font
#define SUPPORT_DEFAULT_FONT            1
//#define SUPPORT_IMGUI

#define SUPPORT_MODULE_RTEXT 1

#define SUPPORT_MODULE_RSHAPES 1

#define MAX_LIGHTS  4           // Max dynamic lights supported by shader


#define MAX_CUBES   30

#define SUPPORT_QUADS_DRAW_MODE         1

#define RLGL_RENDER_TEXTURES_HINT