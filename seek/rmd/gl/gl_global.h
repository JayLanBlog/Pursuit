#pragma once
#include <glad/glad.h>
#include <config/platform_def.h>
#include<section/martrix/cg_def.h>
#include <rmd/gl/gl_batch.h>
using namespace Seek;
#define PL_SUPPORT_MESH_GPU_SKINNING           1      // GPU skinning, comment if your GPU does not support more than 8 VBOs

//#define PL_DEFAULT_BATCH_BUFFER_ELEMENTS    4096    // Default internal render batch elements limits
#define PL_DEFAULT_BATCH_BUFFERS               1      // Default number of batch buffers (multi-buffering)
#define PL_DEFAULT_BATCH_DRAWCALLS           256      // Default number of batch draw calls (by state changes: mode, texture)
#define PL_DEFAULT_BATCH_MAX_TEXTURE_UNITS     4      // Maximum number of textures units that can be activated on batch drawing (SetShaderValueTexture())



#define PL_READ_FRAMEBUFFER                     0x8CA8      // GL_READ_FRAMEBUFFER
#define PL_DRAW_FRAMEBUFFER                     0x8CA9      // GL_DRAW_FRAMEBUFFER


// Default internal render batch elements limits
#ifndef PL_DEFAULT_BATCH_BUFFER_ELEMENTS
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
    // This is the maximum amount of elements (quads) per batch
    // NOTE: Be careful with text, every letter maps to a quad
#define PL_DEFAULT_BATCH_BUFFER_ELEMENTS  8192
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
    // We reduce memory sizes for embedded systems (RPI and HTML5)
    // NOTE: On HTML5 (emscripten) this is allocated on heap,
    // by default it's only 16MB!...just take care...
#define PL_DEFAULT_BATCH_BUFFER_ELEMENTS  2048
#endif
#endif


// Primitive assembly draw modes
#define PL_LINES                                0x0001      // GL_LINES
#define PL_TRIANGLES                            0x0004      // GL_TRIANGLES
#define PL_QUADS                                0x0007      // GL_QUADS

// Matrix modes (equivalent to OpenGL)
#define PL_MODELVIEW                            0x1700      // GL_MODELVIEW
#define PL_PROJECTION                           0x1701      // GL_PROJECTION
#define PL_TEXTURE                              0x1702      // GL_TEXTURE


#define PL_MAX_MATRIX_STACK_SIZE              32      // Maximum size of internal Matrix stack


// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
typedef struct plVertexBuffer {
    int elementCount;           // Number of elements in the buffer (QUADS)

    float* vertices;            // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float* texcoords;           // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    float* normals;             // Vertex normal (XYZ - 3 components per vertex) (shader-location = 2)
    unsigned char* colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
    unsigned int* indices;      // Vertex indices (in case vertex data comes indexed) (6 indices per quad)
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
    unsigned short* indices;    // Vertex indices (in case vertex data comes indexed) (6 indices per quad)
#endif
    unsigned int vaoId;         // OpenGL Vertex Array Object id
    unsigned int vboId[5];      // OpenGL Vertex Buffer Objects id (5 types of vertex data)
} plVertexBuffer;



namespace Batch {
    // rlRenderBatch type
    typedef struct plRenderBatch {
        int bufferCount;            // Number of vertex buffers (multi-buffering support)
        int currentBuffer;          // Current buffer tracking in case of multi-buffering
        plVertexBuffer* vertexBuffer; // Dynamic buffer(s) for vertex data

        plDrawCall* draws;          // Draw calls array, depends on textureId
        int drawCounter;            // Draw calls counter
        float currentDepth;         // Current depth value for next draw
    } plRenderBatch;

    // Render batch management
    //------------------------------------------------------------------------------------------------
    // Load render batch
    plRenderBatch LoadRenderBatch(int numBuffers, int bufferElements);

    void UnloadRenderBatch(plRenderBatch batch);
    // Draw render batch
    // NOTE: We require a pointer to reset batch and increase current buffer (multi-buffer)
    void DrawRenderBatch(plRenderBatch* batch);

    // Set the active render batch for plgl
    void SetRenderBatchActive(plRenderBatch* batch);

    // Update and draw internal render batch
    void DrawRenderBatchActive(void);

    // Check internal buffer overflow for a given number of vertex
    // and force a rlRenderBatch draw call if required
    bool CheckRenderBatchLimit(int vCount);
}

using namespace Batch;



typedef struct PurglData {

    plRenderBatch* currentBatch;
    plRenderBatch defaultBatch;             // Default internal render batch

    struct {
        int vertexCounter;                  // Current active render batch vertex counter (generic, used for all batches)
        float texcoordx, texcoordy;         // Current active texture coordinate (added on glVertex*())
        float normalx, normaly, normalz;    // Current active normal (added on glVertex*())
        unsigned char colorr, colorg, colorb, colora;   // Current active color (added on glVertex*())
        int currentMatrixMode;              // Current matrix mode
        Matrix* currentMatrix;              // Current matrix pointer
        Matrix modelview;                   // Default modelview matrix
        Matrix projection;                  // Default projection 
        Matrix transform;                   // Transform matrix to be used with rlTranslate, rlRotate, rlScale
        bool transformRequired;             // Require transform matrix application to current draw-call vertex (if required)
        Matrix stack[PL_MAX_MATRIX_STACK_SIZE];// Matrix stack for push/pop
        int stackCounter;                   // Matrix stack counter

        unsigned int defaultTextureId;      // Default texture used on shapes/poly drawing (required by shader)
        unsigned int activeTextureId[PL_DEFAULT_BATCH_MAX_TEXTURE_UNITS];    // Active texture ids to be enabled on batch drawing (0 active by default)
        unsigned int defaultVShaderId;      // Default vertex shader id (used by default shader program)
        unsigned int defaultFShaderId;      // Default fragment shader id (used by default shader program)
        unsigned int defaultShaderId;       // Default shader program id, supports vertex color and diffuse texture
        int* defaultShaderLocs;             // Default shader locations pointer to be used on rendering
        unsigned int currentShaderId;       // Current shader id to be used on rendering (by default, defaultShaderId)
        int* currentShaderLocs;             // Current shader locations pointer to be used on rendering (by default, defaultShaderLocs)
        bool stereoRender;                  // Stereo rendering flag
        Matrix projectionStereo[2];         // VR stereo rendering eyes projection matrices
        Matrix viewOffsetStereo[2];         // VR stereo rendering eyes view offset matrices


        // Blending variables
        int currentBlendMode;               // Blending mode active
        int glBlendSrcFactor;               // Blending source factor
        int glBlendDstFactor;               // Blending destination factor
        int glBlendEquation;                // Blending equation
        int glBlendSrcFactorRGB;            // Blending source RGB factor
        int glBlendDestFactorRGB;           // Blending destination RGB factor
        int glBlendSrcFactorAlpha;          // Blending source alpha factor
        int glBlendDestFactorAlpha;         // Blending destination alpha factor
        int glBlendEquationRGB;             // Blending equation for RGB
        int glBlendEquationAlpha;           // Blending equation for alpha
        bool glCustomBlendModeModified;     // Custom blending factor and equation modification status

        int framebufferWidth;               // Current framebuffer width
        int framebufferHeight;              // Current framebuffer height
    }State;

    struct {
        bool vao;                           // VAO support (OpenGL ES2 could not support VAO extension) (GL_ARB_vertex_array_object)
        bool instancing;                    // Instancing supported (GL_ANGLE_instanced_arrays, GL_EXT_draw_instanced + GL_EXT_instanced_arrays)
        bool texNPOT;                       // NPOT textures full support (GL_ARB_texture_non_power_of_two, GL_OES_texture_npot)
        bool texDepth;                      // Depth textures supported (GL_ARB_depth_texture, GL_OES_depth_texture)
        bool texDepthWebGL;                 // Depth textures supported WebGL specific (GL_WEBGL_depth_texture)
        bool texFloat32;                    // float textures support (32 bit per channel) (GL_OES_texture_float)
        bool texFloat16;                    // half float textures support (16 bit per channel) (GL_OES_texture_half_float)
        bool texCompDXT;                    // DDS texture compression support (GL_EXT_texture_compression_s3tc, GL_WEBGL_compressed_texture_s3tc, GL_WEBKIT_WEBGL_compressed_texture_s3tc)
        bool texCompETC1;                   // ETC1 texture compression support (GL_OES_compressed_ETC1_RGB8_texture, GL_WEBGL_compressed_texture_etc1)
        bool texCompETC2;                   // ETC2/EAC texture compression support (GL_ARB_ES3_compatibility)
        bool texCompPVRT;                   // PVR texture compression support (GL_IMG_texture_compression_pvrtc)
        bool texCompASTC;                   // ASTC texture compression support (GL_KHR_texture_compression_astc_hdr, GL_KHR_texture_compression_astc_ldr)
        bool texMirrorClamp;                // Clamp mirror wrap mode supported (GL_EXT_texture_mirror_clamp)
        bool texAnisoFilter;                // Anisotropic texture filtering support (GL_EXT_texture_filter_anisotropic)
        bool computeShader;                 // Compute shaders support (GL_ARB_compute_shader)
        bool ssbo;                          // Shader storage buffer object support (GL_ARB_shader_storage_buffer_object)

        float maxAnisotropyLevel;           // Maximum anisotropy level supported (minimum is 2.0f)
        int maxDepthBits;                   // Maximum bits for depth component
    } ExtSupported;     // Extensions supported flags
};
extern  PurglData PLGL;

// OpenGL version
typedef enum {
    PL_OPENGL_11 = 1,           // OpenGL 1.1
    PL_OPENGL_21,               // OpenGL 2.1 (GLSL 120)
    PL_OPENGL_33,               // OpenGL 3.3 (GLSL 330)
    PL_OPENGL_43,               // OpenGL 4.3 (using GLSL 330)
    PL_OPENGL_ES_20,            // OpenGL ES 2.0 (GLSL 100)
    PL_OPENGL_ES_30             // OpenGL ES 3.0 (GLSL 300 es)
} plGlVersion;

// Get current OpenGL version
int plGetVersion(void);


// Load OpenGL extensions
// NOTE: External loader function must be provided
void LoadExtensions(void* loader);

//----------------------------------------------------------------------------------
// Module Functions Definition - PLGL functionality
//----------------------------------------------------------------------------------

// Initialize PLGL: OpenGL extensions, default buffers/shaders/textures, OpenGL states
void plglInit(int width, int height);



// Vertex Buffer Object deinitialization (memory free)
void plglClose(void);

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
void Normal3f(float x, float y, float z);

#endif


void Vertex3f(float x, float y, float z);

void Vertex2f(float x, float y);
// Define one vertex (texture coordinate)
// NOTE: Texture coordinates are limited to QUADS only
void TexCoord2f(float x, float y);

// Define one vertex (position)
void Vertex2i(int x, int y);

