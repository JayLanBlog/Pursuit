#include "gl_global.h"
#include <logger/logger.h>
#include <rmd/gl/gl_texture.h>
#include <section/enum/material_em.h>
#include <rmd/gl/gl_default.h>
#include <math/pr_math.h>

using namespace DRAW::GL;
using namespace PMath;

PurglData PLGL ={0};


// Get current OpenGL version
int plGetVersion(void)
{
    int glVersion = 0;
#if defined(GRAPHICS_API_OPENGL_11)
    glVersion = PL_OPENGL_11;
#endif
#if defined(GRAPHICS_API_OPENGL_21)
    glVersion = PL_OPENGL_21;
#elif defined(GRAPHICS_API_OPENGL_43)
    glVersion = PL_OPENGL_43;
#elif defined(GRAPHICS_API_OPENGL_33)
    glVersion = PL_OPENGL_33;
#endif
#if defined(GRAPHICS_API_OPENGL_ES3)
    glVersion = PL_OPENGL_ES_30;
#elif defined(GRAPHICS_API_OPENGL_ES2)
    glVersion = PL_OPENGL_ES_20;
#endif

    return glVersion;
}

// Load OpenGL extensions
// NOTE: External loader function must be provided
void LoadExtensions(void* loader)
{
#if defined(GRAPHICS_API_OPENGL_33)     // Also defined for GRAPHICS_API_OPENGL_21
    // NOTE: glad is generated and contains only required OpenGL 3.3 Core extensions (and lower versions)
    if (gladLoadGLLoader((GLADloadproc)loader) == 0) TRACELOG(LOG_WARNING, "GLAD: Cannot load OpenGL extensions");
    else TRACELOG(LOG_INFO, "GLAD: OpenGL extensions loaded successfully");

    // Get number of supported extensions
    GLint numExt = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
    TRACELOG(LOG_INFO, "GL: Supported extensions count: %i", numExt);

#if defined(PLGL_SHOW_GL_DETAILS_INFO)
    // Get supported extensions list
    // WARNING: glGetStringi() not available on OpenGL 2.1
    TRACELOG(LOG_INFO, "GL: OpenGL extensions:");
    for (int i = 0; i < numExt; i++) TRACELOG(LOG_INFO, "    %s", glGetStringi(GL_EXTENSIONS, i));
#endif

#if defined(GRAPHICS_API_OPENGL_21)
    // Register supported extensions flags
    // Optional OpenGL 2.1 extensions
    PLGL.ExtSupported.vao = GLAD_GL_ARB_vertex_array_object;
    PLGL.ExtSupported.instancing = (GLAD_GL_EXT_draw_instanced && GLAD_GL_ARB_instanced_arrays);
    PLGL.ExtSupported.texNPOT = GLAD_GL_ARB_texture_non_power_of_two;
    PLGL.ExtSupported.texFloat32 = GLAD_GL_ARB_texture_float;
    PLGL.ExtSupported.texFloat16 = GLAD_GL_ARB_texture_float;
    PLGL.ExtSupported.texDepth = GLAD_GL_ARB_depth_texture;
    PLGL.ExtSupported.maxDepthBits = 32;
    PLGL.ExtSupported.texAnisoFilter = GLAD_GL_EXT_texture_filter_anisotropic;
    PLGL.ExtSupported.texMirrorClamp = GLAD_GL_EXT_texture_mirror_clamp;
#else
    // Register supported extensions flags
    // OpenGL 3.3 extensions supported by default (core)

    PLGL.ExtSupported.vao = true;
    PLGL.ExtSupported.instancing = true;
    PLGL.ExtSupported.texNPOT = true;
    PLGL.ExtSupported.texFloat32 = true;
    PLGL.ExtSupported.texFloat16 = true;
    PLGL.ExtSupported.texDepth = true;
    PLGL.ExtSupported.maxDepthBits = 32;
    PLGL.ExtSupported.texAnisoFilter = true;
    PLGL.ExtSupported.texMirrorClamp = true;
#endif
   // GLAD_GL_KHR_texture_compression_astc_ldr;
    // Optional OpenGL 3.3 extensions
   // PLGL.ExtSupported.texCompASTC = GLAD_GL_KHR_texture_compression_astc_hdr && GLAD_GL_KHR_texture_compression_astc_ldr;
   // PLGL.ExtSupported.texCompDXT = GLAD_GL_EXT_texture_compression_s3tc;  // Texture compression: DXT
   // PLGL.ExtSupported.texCompETC2 = GLAD_GL_ARB_ES3_compatibility;        // Texture compression: ETC2/EAC
#if defined(GRAPHICS_API_OPENGL_43)
    PLGL.ExtSupported.computeShader = GLAD_GL_ARB_compute_shader;
    PLGL.ExtSupported.ssbo = GLAD_GL_ARB_shader_storage_buffer_object;
#endif

#endif  // GRAPHICS_API_OPENGL_33

#if defined(GRAPHICS_API_OPENGL_ES3)
    // Register supported extensions flags
    // OpenGL ES 3.0 extensions supported by default (or it should be)
    PLGL.ExtSupported.vao = true;
    PLGL.ExtSupported.instancing = true;
    PLGL.ExtSupported.texNPOT = true;
    PLGL.ExtSupported.texFloat32 = true;
    PLGL.ExtSupported.texFloat16 = true;
    PLGL.ExtSupported.texDepth = true;
    PLGL.ExtSupported.texDepthWebGL = true;
    PLGL.ExtSupported.maxDepthBits = 24;
    PLGL.ExtSupported.texAnisoFilter = true;
    PLGL.ExtSupported.texMirrorClamp = true;
    // TODO: Check for additional OpenGL ES 3.0 supported extensions:
    //PLGL.ExtSupported.texCompDXT = true;
    //PLGL.ExtSupported.texCompETC1 = true;
    //PLGL.ExtSupported.texCompETC2 = true;
    //PLGL.ExtSupported.texCompPVRT = true;
    //PLGL.ExtSupported.texCompASTC = true;
    //PLGL.ExtSupported.maxAnisotropyLevel = true;
    //PLGL.ExtSupported.computeShader = true;
    //PLGL.ExtSupported.ssbo = true;

#elif defined(GRAPHICS_API_OPENGL_ES2)

#if defined(PLATFORM_DESKTOP_GLFW) || defined(PLATFORM_DESKTOP_SDL)
    // TODO: Support GLAD loader for OpenGL ES 3.0
    if (gladLoadGLES2((GLADloadfunc)loader) == 0) TRACELOG(LOG_WARNING, "GLAD: Cannot load OpenGL ES2.0 functions");
    else TRACELOG(LOG_INFO, "GLAD: OpenGL ES 2.0 loaded successfully");
#endif

    // Get supported extensions list
    GLint numExt = 0;
    const char** extList = (const char**)PL_MALLOC(512 * sizeof(const char*)); // Allocate 512 strings pointers (2 KB)
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);  // One big const string

    // NOTE: We have to duplicate string because glGetString() returns a const string
    int size = strlen(extensions) + 1;      // Get extensions string size in bytes
    char* extensionsDup = (char*)PL_CALLOC(size, sizeof(char));
    strcpy(extensionsDup, extensions);
    extList[numExt] = extensionsDup;

    for (int i = 0; i < size; i++)
    {
        if (extensionsDup[i] == ' ')
        {
            extensionsDup[i] = '\0';
            numExt++;
            extList[numExt] = &extensionsDup[i + 1];
        }
    }

    TRACELOG(LOG_INFO, "GL: Supported extensions count: %i", numExt);

#if defined(PLGL_SHOW_GL_DETAILS_INFO)
    TRACELOG(LOG_INFO, "GL: OpenGL extensions:");
    for (int i = 0; i < numExt; i++) TRACELOG(LOG_INFO, "    %s", extList[i]);
#endif

    // Check required extensions
    for (int i = 0; i < numExt; i++)
    {
        // Check VAO support
        // NOTE: Only check on OpenGL ES, OpenGL 3.3 has VAO support as core feature
        if (strcmp(extList[i], (const char*)"GL_OES_vertex_array_object") == 0)
        {
            // The extension is supported by our hardware and driver, try to get related functions pointers
            // NOTE: emscripten does not support VAOs natively, it uses emulation and it reduces overall performance...
            glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)((PLGLLoadProc)loader)("glGenVertexArraysOES");
            glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)((PLGLLoadProc)loader)("glBindVertexArrayOES");
            glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC)((PLGLLoadProc)loader)("glDeleteVertexArraysOES");
            //glIsVertexArray = (PFNGLISVERTEXARRAYOESPROC)loader("glIsVertexArrayOES");     // NOTE: Fails in WebGL, omitted

            if ((glGenVertexArrays != NULL) && (glBindVertexArray != NULL) && (glDeleteVertexArrays != NULL)) PLGL.ExtSupported.vao = true;
        }

        // Check instanced rendering support
        if (strstr(extList[i], (const char*)"instanced_arrays") != NULL)   // Broad check for instanced_arrays
        {
            // Specific check
            if (strcmp(extList[i], (const char*)"GL_ANGLE_instanced_arrays") == 0)      // ANGLE
            {
                glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)((PLGLLoadProc)loader)("glDrawArraysInstancedANGLE");
                glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)((PLGLLoadProc)loader)("glDrawElementsInstancedANGLE");
                glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISOREXTPROC)((PLGLLoadProc)loader)("glVertexAttribDivisorANGLE");
            }
            else if (strcmp(extList[i], (const char*)"GL_EXT_instanced_arrays") == 0)   // EXT
            {
                glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)((PLGLLoadProc)loader)("glDrawArraysInstancedEXT");
                glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)((PLGLLoadProc)loader)("glDrawElementsInstancedEXT");
                glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISOREXTPROC)((PLGLLoadProc)loader)("glVertexAttribDivisorEXT");
            }
            else if (strcmp(extList[i], (const char*)"GL_NV_instanced_arrays") == 0)    // NVIDIA GLES
            {
                glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)((PLGLLoadProc)loader)("glDrawArraysInstancedNV");
                glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)((PLGLLoadProc)loader)("glDrawElementsInstancedNV");
                glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISOREXTPROC)((PLGLLoadProc)loader)("glVertexAttribDivisorNV");
            }

            // The feature will only be marked as supported if the elements from GL_XXX_instanced_arrays are present
            if ((glDrawArraysInstanced != NULL) && (glDrawElementsInstanced != NULL) && (glVertexAttribDivisor != NULL)) PLGL.ExtSupported.instancing = true;
        }
        else if (strstr(extList[i], (const char*)"draw_instanced") != NULL)
        {
            // GL_ANGLE_draw_instanced doesn't exist
            if (strcmp(extList[i], (const char*)"GL_EXT_draw_instanced") == 0)
            {
                glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)((PLGLLoadProc)loader)("glDrawArraysInstancedEXT");
                glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)((PLGLLoadProc)loader)("glDrawElementsInstancedEXT");
            }
            else if (strcmp(extList[i], (const char*)"GL_NV_draw_instanced") == 0)
            {
                glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)((PLGLLoadProc)loader)("glDrawArraysInstancedNV");
                glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)((PLGLLoadProc)loader)("glDrawElementsInstancedNV");
            }

            // But the functions will at least be loaded if only GL_XX_EXT_draw_instanced exist
            if ((glDrawArraysInstanced != NULL) && (glDrawElementsInstanced != NULL) && (glVertexAttribDivisor != NULL)) PLGL.ExtSupported.instancing = true;
        }

        // Check NPOT textures support
        // NOTE: Only check on OpenGL ES, OpenGL 3.3 has NPOT textures full support as core feature
        if (strcmp(extList[i], (const char*)"GL_OES_texture_npot") == 0) PLGL.ExtSupported.texNPOT = true;

        // Check texture float support
        if (strcmp(extList[i], (const char*)"GL_OES_texture_float") == 0) PLGL.ExtSupported.texFloat32 = true;
        if (strcmp(extList[i], (const char*)"GL_OES_texture_half_float") == 0) PLGL.ExtSupported.texFloat16 = true;

        // Check depth texture support
        if (strcmp(extList[i], (const char*)"GL_OES_depth_texture") == 0) PLGL.ExtSupported.texDepth = true;
        if (strcmp(extList[i], (const char*)"GL_WEBGL_depth_texture") == 0) PLGL.ExtSupported.texDepthWebGL = true; // WebGL requires unsized internal format
        if (PLGL.ExtSupported.texDepthWebGL) PLGL.ExtSupported.texDepth = true;

        if (strcmp(extList[i], (const char*)"GL_OES_depth24") == 0) PLGL.ExtSupported.maxDepthBits = 24; // Not available on WebGL
        if (strcmp(extList[i], (const char*)"GL_OES_depth32") == 0) PLGL.ExtSupported.maxDepthBits = 32; // Not available on WebGL

        // Check texture compression support: DXT
        if ((strcmp(extList[i], (const char*)"GL_EXT_texture_compression_s3tc") == 0) ||
            (strcmp(extList[i], (const char*)"GL_WEBGL_compressed_texture_s3tc") == 0) ||
            (strcmp(extList[i], (const char*)"GL_WEBKIT_WEBGL_compressed_texture_s3tc") == 0)) PLGL.ExtSupported.texCompDXT = true;

        // Check texture compression support: ETC1
        if ((strcmp(extList[i], (const char*)"GL_OES_compressed_ETC1_RGB8_texture") == 0) ||
            (strcmp(extList[i], (const char*)"GL_WEBGL_compressed_texture_etc1") == 0)) PLGL.ExtSupported.texCompETC1 = true;

        // Check texture compression support: ETC2/EAC
        if (strcmp(extList[i], (const char*)"GL_ARB_ES3_compatibility") == 0) PLGL.ExtSupported.texCompETC2 = true;

        // Check texture compression support: PVR
        if (strcmp(extList[i], (const char*)"GL_IMG_texture_compression_pvrtc") == 0) PLGL.ExtSupported.texCompPVRT = true;

        // Check texture compression support: ASTC
        if (strcmp(extList[i], (const char*)"GL_KHR_texture_compression_astc_hdr") == 0) PLGL.ExtSupported.texCompASTC = true;

        // Check anisotropic texture filter support
        if (strcmp(extList[i], (const char*)"GL_EXT_texture_filter_anisotropic") == 0) PLGL.ExtSupported.texAnisoFilter = true;

        // Check clamp mirror wrap mode support
        if (strcmp(extList[i], (const char*)"GL_EXT_texture_mirror_clamp") == 0) PLGL.ExtSupported.texMirrorClamp = true;
    }

    // Free extensions pointers
    PL_FREE(extList);
    PL_FREE(extensionsDup);    // Duplicated string must be deallocated
#endif  // GRAPHICS_API_OPENGL_ES2

    // Check OpenGL information and capabilities
    //------------------------------------------------------------------------------
    // Show current OpenGL and GLSL version
    TRACELOG(LOG_INFO, "GL: OpenGL device information:");
    TRACELOG(LOG_INFO, "    > Vendor:   %s", glGetString(GL_VENDOR));
    TRACELOG(LOG_INFO, "    > Renderer: %s", glGetString(GL_RENDERER));
    TRACELOG(LOG_INFO, "    > Version:  %s", glGetString(GL_VERSION));
    TRACELOG(LOG_INFO, "    > GLSL:     %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    // NOTE: Anisotropy levels capability is an extension
#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &PLGL.ExtSupported.maxAnisotropyLevel);

#if defined(PLGL_SHOW_GL_DETAILS_INFO)
    // Show some OpenGL GPU capabilities
    TRACELOG(LOG_INFO, "GL: OpenGL capabilities:");
    GLint capability = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &capability);
    TRACELOG(LOG_INFO, "    GL_MAX_TEXTURE_SIZE: %i", capability);
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &capability);
    TRACELOG(LOG_INFO, "    GL_MAX_CUBE_MAP_TEXTURE_SIZE: %i", capability);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &capability);
    TRACELOG(LOG_INFO, "    GL_MAX_TEXTURE_IMAGE_UNITS: %i", capability);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &capability);
    TRACELOG(LOG_INFO, "    GL_MAX_VERTEX_ATTRIBS: %i", capability);
#if !defined(GRAPHICS_API_OPENGL_ES2)
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &capability);
    TRACELOG(LOG_INFO, "    GL_MAX_UNIFORM_BLOCK_SIZE: %i", capability);
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &capability);
    TRACELOG(LOG_INFO, "    GL_MAX_DRAW_BUFFERS: %i", capability);
    if (PLGL.ExtSupported.texAnisoFilter) TRACELOG(LOG_INFO, "    GL_MAX_TEXTURE_MAX_ANISOTROPY: %.0f", PLGL.ExtSupported.maxAnisotropyLevel);
#endif
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &capability);
    TRACELOG(LOG_INFO, "    GL_NUM_COMPRESSED_TEXTURE_FORMATS: %i", capability);
    GLint* compFormats = (GLint*)PL_CALLOC(capability, sizeof(GLint));
    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, compFormats);
    for (int i = 0; i < capability; i++) TRACELOG(LOG_INFO, "        %s", rlGetCompressedFormatName(compFormats[i]));
    PL_FREE(compFormats);

#if defined(GRAPHICS_API_OPENGL_43)
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &capability);
    TRACELOG(LOG_INFO, "    GL_MAX_VERTEX_ATTRIB_BINDINGS: %i", capability);
    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &capability);
    TRACELOG(LOG_INFO, "    GL_MAX_UNIFORM_LOCATIONS: %i", capability);
#endif  // GRAPHICS_API_OPENGL_43
#else   // PLGL_SHOW_GL_DETAILS_INFO

    // Show some basic info about GL supported features
    if (PLGL.ExtSupported.vao) TRACELOG(LOG_INFO, "GL: VAO extension detected, VAO functions loaded successfully");
    else TRACELOG(LOG_WARNING, "GL: VAO extension not found, VAO not supported");
    if (PLGL.ExtSupported.texNPOT) TRACELOG(LOG_INFO, "GL: NPOT textures extension detected, full NPOT textures supported");
    else TRACELOG(LOG_WARNING, "GL: NPOT textures extension not found, limited NPOT support (no-mipmaps, no-repeat)");
    if (PLGL.ExtSupported.texCompDXT) TRACELOG(LOG_INFO, "GL: DXT compressed textures supported");
    if (PLGL.ExtSupported.texCompETC1) TRACELOG(LOG_INFO, "GL: ETC1 compressed textures supported");
    if (PLGL.ExtSupported.texCompETC2) TRACELOG(LOG_INFO, "GL: ETC2/EAC compressed textures supported");
    if (PLGL.ExtSupported.texCompPVRT) TRACELOG(LOG_INFO, "GL: PVRT compressed textures supported");
    if (PLGL.ExtSupported.texCompASTC) TRACELOG(LOG_INFO, "GL: ASTC compressed textures supported");
    if (PLGL.ExtSupported.computeShader) TRACELOG(LOG_INFO, "GL: Compute shaders supported");
    if (PLGL.ExtSupported.ssbo) TRACELOG(LOG_INFO, "GL: Shader storage buffer objects supported");
#endif  // PLGL_SHOW_GL_DETAILS_INFO

#endif  // GRAPHICS_API_OPENGL_33 || GRAPHICS_API_OPENGL_ES2
}


void plglInit(int width, int height) {
    // Enable OpenGL debug context if required
#if defined(PLGL_ENABLE_OPENGL_DEBUG_CONTEXT) && defined(GRAPHICS_API_OPENGL_43)
    if ((glDebugMessageCallback != NULL) && (glDebugMessageControl != NULL))
    {
        glDebugMessageCallback(rlDebugMessageCallback, 0);
        // glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, 0, GL_TRUE);

        // Debug context options:
        //  - GL_DEBUG_OUTPUT - Faster version but not useful for breakpoints
        //  - GL_DEBUG_OUTPUT_SYNCHRONUS - Callback is in sync with errors, so a breakpoint can be placed on the callback in order to get a stacktrace for the GL error
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
#endif

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    // Init default white texture
    unsigned char pixels[4] = { 255, 255, 255, 255 };   // 1 pixel RGBA (4 bytes)
    PLGL.State.defaultTextureId = LoadTexture(pixels, 1, 1,PL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);

    if (PLGL.State.defaultTextureId != 0) TRACELOG(LOG_INFO, "TEXTURE: [ID %i] Default texture loaded successfully", PLGL.State.defaultTextureId);
    else TRACELOG(LOG_WARNING, "TEXTURE: Failed to load default texture");

    // Init default Shader (customized for GL 3.3 and ES2)
    // Loaded: PLGL.State.defaultShaderId + PLGL.State.defaultShaderLocs
    LoadDefaultShader();
    PLGL.State.currentShaderId = PLGL.State.defaultShaderId;
    PLGL.State.currentShaderLocs = PLGL.State.defaultShaderLocs;

    // Init default vertex arrays buffers
    // Simulate that the default shader has the location PL_SHADER_LOC_VERTEX_NORMAL to bind the normal buffer for the default render batch
    PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_NORMAL] = PL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL;
    PLGL.defaultBatch = LoadRenderBatch(PL_DEFAULT_BATCH_BUFFERS, PL_DEFAULT_BATCH_BUFFER_ELEMENTS);
    PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_NORMAL] = -1;
    PLGL.currentBatch = &PLGL.defaultBatch;

    // Init stack matrices (emulating OpenGL 1.1)
    for (int i = 0; i < PL_MAX_MATRIX_STACK_SIZE; i++) PLGL.State.stack[i] = MatrixIdentity();

    // Init internal matrices
    PLGL.State.transform = MatrixIdentity();
    PLGL.State.projection = MatrixIdentity();
    PLGL.State.modelview = MatrixIdentity();
    PLGL.State.currentMatrix = &PLGL.State.modelview;
#endif  // GRAPHICS_API_OPENGL_33 || GRAPHICS_API_OPENGL_ES2


    // Initialize OpenGL default states
   //----------------------------------------------------------
   // Init state: Depth test
    glDepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
    glDisable(GL_DEPTH_TEST);                               // Disable depth testing for 2D (only used for 3D)

    // Init state: Blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
    glEnable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)

    // Init state: Culling
    // NOTE: All shapes/models triangles are drawn CCW
    glCullFace(GL_BACK);                                    // Cull the back face (default)
    glFrontFace(GL_CCW);                                    // Front face are defined counter clockwise (default)
    glEnable(GL_CULL_FACE);                                 // Enable backface culling

    // Init state: Cubemap seamless
#if defined(GRAPHICS_API_OPENGL_33)
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);                 // Seamless cubemaps (not supported on OpenGL ES 2.0)
#endif
#if defined(GRAPHICS_API_OPENGL_11)
    // Init state: Color hints (deprecated in OpenGL 3.0+)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);      // Improve quality of color and texture coordinate interpolation
    glShadeModel(GL_SMOOTH);                                // Smooth shading between vertex (vertex colors interpolation)
#endif

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
// Store screen size into global variables
    PLGL.State.framebufferWidth = width;
    PLGL.State.framebufferHeight = height;

    TRACELOG(LOG_INFO, "PLGL: Default OpenGL state initialized successfully");
    //----------------------------------------------------------
#endif
     // Init state: Color/Depth buffers clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear color (black)
    glClearDepth(1.0f);                                     // Set clear depth value (default)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear color and depth buffers (depth buffer required for 3D)
}


// Vertex Buffer Object deinitialization (memory free)
void plglClose(void) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    UnloadRenderBatch(PLGL.defaultBatch);
    UnloadDefautlShader();          // Unload default shader
    glDeleteTextures(1, &PLGL.State.defaultTextureId); // Unload default texture
    TRACELOG(LOG_INFO, "TEXTURE: [ID %i] Default texture unloaded successfully", PLGL.State.defaultTextureId);
#endif
}


#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
// Define one vertex (normal)
// NOTE: Normals limited to TRIANGLES only?
void Normal3f(float x, float y, float z)
{
    float normalx = x;
    float normaly = y;
    float normalz = z;
    if (PLGL.State.transformRequired)
    {
        normalx = PLGL.State.transform.m0 * x + PLGL.State.transform.m4 * y + PLGL.State.transform.m8 * z;
        normaly = PLGL.State.transform.m1 * x + PLGL.State.transform.m5 * y + PLGL.State.transform.m9 * z;
        normalz = PLGL.State.transform.m2 * x + PLGL.State.transform.m6 * y + PLGL.State.transform.m10 * z;
    }
    float length = sqrtf(normalx * normalx + normaly * normaly + normalz * normalz);
    if (length != 0.0f)
    {
        float ilength = 1.0f / length;
        normalx *= ilength;
        normaly *= ilength;
        normalz *= ilength;
    }
    PLGL.State.normalx = normalx;
    PLGL.State.normaly = normaly;
    PLGL.State.normalz = normalz;
}

#endif

void Vertex3f(float x, float y, float z) {
    float tx = x;
    float ty = y;
    float tz = z;

    // Transform provided vector if required
    if (PLGL.State.transformRequired)
    {
        tx = PLGL.State.transform.m0 * x + PLGL.State.transform.m4 * y + PLGL.State.transform.m8 * z + PLGL.State.transform.m12;
        ty = PLGL.State.transform.m1 * x + PLGL.State.transform.m5 * y + PLGL.State.transform.m9 * z + PLGL.State.transform.m13;
        tz = PLGL.State.transform.m2 * x + PLGL.State.transform.m6 * y + PLGL.State.transform.m10 * z + PLGL.State.transform.m14;
    }

    // WARNING: We can't break primitives when launching a new batch
    // RL_LINES comes in pairs, RL_TRIANGLES come in groups of 3 vertices and RL_QUADS come in groups of 4 vertices
    // We must check current draw.mode when a new vertex is required and finish the batch only if the draw.mode draw.vertexCount is %2, %3 or %4
    if (PLGL.State.vertexCounter > (PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].elementCount * 4 - 4))
    {
        if ((PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].mode == PL_LINES) &&
            (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount % 2 == 0))
        {
            // Reached the maximum number of vertices for RL_LINES drawing
            // Launch a draw call but keep current state for next vertices comming
            // NOTE: We add +1 vertex to the check for security
            CheckRenderBatchLimit(2 + 1);
        }
        else if ((PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].mode == PL_TRIANGLES) &&
            (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount % 3 == 0))
        {
            CheckRenderBatchLimit(3 + 1);
        }
        else if ((PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].mode == PL_QUADS) &&
            (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount % 4 == 0))
        {
            CheckRenderBatchLimit(4 + 1);
        }
    }

    // Add vertices
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].vertices[3 * PLGL.State.vertexCounter] = tx;
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].vertices[3 * PLGL.State.vertexCounter + 1] = ty;
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].vertices[3 * PLGL.State.vertexCounter + 2] = tz;

    // Add current texcoord
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].texcoords[2 * PLGL.State.vertexCounter] = PLGL.State.texcoordx;
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].texcoords[2 * PLGL.State.vertexCounter + 1] = PLGL.State.texcoordy;

    // Add current normal
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].normals[3 * PLGL.State.vertexCounter] = PLGL.State.normalx;
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].normals[3 * PLGL.State.vertexCounter + 1] = PLGL.State.normaly;
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].normals[3 * PLGL.State.vertexCounter + 2] = PLGL.State.normalz;

    // Add current color
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].colors[4 * PLGL.State.vertexCounter] = PLGL.State.colorr;
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].colors[4 * PLGL.State.vertexCounter + 1] = PLGL.State.colorg;
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].colors[4 * PLGL.State.vertexCounter + 2] = PLGL.State.colorb;
    PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].colors[4 * PLGL.State.vertexCounter + 3] = PLGL.State.colora;

    PLGL.State.vertexCounter++;
    PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount++;
}

void Vertex2f(float x, float y) {
    Vertex3f(x, y, PLGL.currentBatch->currentDepth);
}
// Define one vertex (texture coordinate)
// NOTE: Texture coordinates are limited to QUADS only
void TexCoord2f(float x, float y) {
    PLGL.State.texcoordx = x;
    PLGL.State.texcoordy = y;
}

// Define one vertex (position)
void Vertex2i(int x, int y) {
    Vertex3f((float)x, (float)y, PLGL.currentBatch->currentDepth);
}