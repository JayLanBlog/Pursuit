#include "gl_pipline.h"
#include <logger/logger.h>

namespace PiplineState {

    // Set blend mode
    void SetBlendMode(int mode)
    {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
        if ((PLGL.State.currentBlendMode != mode) || ((mode == PL_BLEND_CUSTOM || mode == PL_BLEND_CUSTOM_SEPARATE) && PLGL.State.glCustomBlendModeModified))
        {
            DrawRenderBatch(PLGL.currentBatch);

            switch (mode)
            {
            case PL_BLEND_ALPHA: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD); break;
            case PL_BLEND_ADDITIVE: glBlendFunc(GL_SRC_ALPHA, GL_ONE); glBlendEquation(GL_FUNC_ADD); break;
            case PL_BLEND_MULTIPLIED: glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD); break;
            case PL_BLEND_ADD_COLORS: glBlendFunc(GL_ONE, GL_ONE); glBlendEquation(GL_FUNC_ADD); break;
            case PL_BLEND_SUBTRACT_COLORS: glBlendFunc(GL_ONE, GL_ONE); glBlendEquation(GL_FUNC_SUBTRACT); break;
            case PL_BLEND_ALPHA_PREMULTIPLY: glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD); break;
            case PL_BLEND_CUSTOM:
            {
                // NOTE: Using GL blend src/dst factors and GL equation configured with rlSetBlendFactors()
                glBlendFunc(PLGL.State.glBlendSrcFactor, PLGL.State.glBlendDstFactor); glBlendEquation(PLGL.State.glBlendEquation);
            } break;
            case PL_BLEND_CUSTOM_SEPARATE:
            {
                // NOTE: Using GL blend src/dst factors and GL equation configured with rlSetBlendFactorsSeparate()
                glBlendFuncSeparate(PLGL.State.glBlendSrcFactorRGB, PLGL.State.glBlendDestFactorRGB, PLGL.State.glBlendSrcFactorAlpha, PLGL.State.glBlendDestFactorAlpha);
                glBlendEquationSeparate(PLGL.State.glBlendEquationRGB, PLGL.State.glBlendEquationAlpha);
            } break;
            default: break;
            }

            PLGL.State.currentBlendMode = mode;
            PLGL.State.glCustomBlendModeModified = false;
        }
#endif
    }

    void CheckErrors(void)
    {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
        int check = 1;
        while (check)
        {
            const GLenum err = glGetError();
            switch (err)
            {
            case GL_NO_ERROR: check = 0; break;
            case 0x0500: TRACELOG(LOG_WARNING, "GL: Error detected: GL_INVALID_ENUM"); break;
            case 0x0501: TRACELOG(LOG_WARNING, "GL: Error detected: GL_INVALID_VALUE"); break;
            case 0x0502: TRACELOG(LOG_WARNING, "GL: Error detected: GL_INVALID_OPERATION"); break;
            case 0x0503: TRACELOG(LOG_WARNING, "GL: Error detected: GL_STACK_OVERFLOW"); break;
            case 0x0504: TRACELOG(LOG_WARNING, "GL: Error detected: GL_STACK_UNDERFLOW"); break;
            case 0x0505: TRACELOG(LOG_WARNING, "GL: Error detected: GL_OUT_OF_MEMORY"); break;
            case 0x0506: TRACELOG(LOG_WARNING, "GL: Error detected: GL_INVALID_FRAMEBUFFER_OPERATION"); break;
            default: TRACELOG(LOG_WARNING, "GL: Error detected: Unknown error code: %x", err); break;
            }
        }
#endif
    }

    // Clear used screen buffers (color and depth)
    void ClearScreenBuffers(void)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear used buffers: Color and Depth (Depth is used for 3D)
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);     // Stencil buffer not used...
    }

    // Clear color buffer with color
    void ClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        // Color values clamp to 0.0f(0) and 1.0f(255)
        float cr = (float)r / 255;
        float cg = (float)g / 255;
        float cb = (float)b / 255;
        float ca = (float)a / 255;

        glClearColor(cr, cg, cb, ca);
    }

    // Enable line aliasing
    void EnableSmoothLines(void)
    {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_11)
        glEnable(GL_LINE_SMOOTH);
#endif
    }

    // Disable line aliasing
    void DisableSmoothLines(void)
    {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_11)
        glDisable(GL_LINE_SMOOTH);
#endif
    }
    

    // Enable color blending
    void EnableColorBlend(void) { glEnable(GL_BLEND); }

    // Disable color blending
    void DisableColorBlend(void) { glDisable(GL_BLEND); }

    // Enable depth test
    void EnableDepthTest(void) { glEnable(GL_DEPTH_TEST); }

    // Disable depth test
    void DisableDepthTest(void) { glDisable(GL_DEPTH_TEST); }

    // Enable depth write
    void EnableDepthMask(void) { glDepthMask(GL_TRUE); }

    // Disable depth write
    void DisableDepthMask(void) { glDepthMask(GL_FALSE); }

    // Enable backface culling
    void EnableBackfaceCulling(void) { glEnable(GL_CULL_FACE); }

    // Disable backface culling
    void DisableBackfaceCulling(void) { glDisable(GL_CULL_FACE); }

    // Set color mask active for screen read/draw
    void ColorMask(bool r, bool g, bool b, bool a) { glColorMask(r, g, b, a); }

    // Set face culling mode
    void SetCullFace(int mode)
    {
        switch (mode)
        {
        case PL_CULL_FACE_BACK: glCullFace(GL_BACK); break;
        case PL_CULL_FACE_FRONT: glCullFace(GL_FRONT); break;
        default: break;
        }
    }

    // Enable scissor test
    void EnableScissorTest(void) { glEnable(GL_SCISSOR_TEST); }

    // Disable scissor test
    void DisableScissorTest(void) { glDisable(GL_SCISSOR_TEST); }

    // Scissor test
    void Scissor(int x, int y, int width, int height) { glScissor(x, y, width, height); }

    // Enable wire mode
    void EnableWireMode(void)
    {
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
        // NOTE: glPolygonMode() not available on OpenGL ES
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
    }

    // Disable wire mode
    void DisableWireMode(void)
    {
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
        // NOTE: glPolygonMode() not available on OpenGL ES
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    }

    // Enable point mode
    void EnablePointMode(void)
    {
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
        // NOTE: glPolygonMode() not available on OpenGL ES
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glEnable(GL_PROGRAM_POINT_SIZE);
#endif
    }

    // Disable point mode
    void DisablePointMode(void)
    {
#if defined(GRAPHICS_API_OPENGL_11) || defined(GRAPHICS_API_OPENGL_33)
        // NOTE: glPolygonMode() not available on OpenGL ES
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    }

}
