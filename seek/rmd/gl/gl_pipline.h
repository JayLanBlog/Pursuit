#pragma once
//Used to calculate various states of the pipeline

#include "gl_global.h"
// Face culling mode
typedef enum {
	PL_CULL_FACE_FRONT = 0,
	PL_CULL_FACE_BACK
} plCullMode;


// Color blending modes (pre-defined)
typedef enum {
	PL_BLEND_ALPHA = 0,                 // Blend textures considering alpha (default)
	PL_BLEND_ADDITIVE,                  // Blend textures adding colors
	PL_BLEND_MULTIPLIED,                // Blend textures multiplying colors
	PL_BLEND_ADD_COLORS,                // Blend textures adding colors (alternative)
	PL_BLEND_SUBTRACT_COLORS,           // Blend textures subtracting colors (alternative)
	PL_BLEND_ALPHA_PREMULTIPLY,         // Blend premultiplied textures considering alpha
	PL_BLEND_CUSTOM,                    // Blend textures using custom src/dst factors (use rlSetBlendFactors())
	PL_BLEND_CUSTOM_SEPARATE            // Blend textures using custom src/dst factors (use rlSetBlendFactorsSeparate())
} plBlendMode;

namespace PiplineState{
	// Disable line aliasing
	void DisableSmoothLines(void);
	// Enable line aliasing
	void EnableSmoothLines(void);
	// Clear color buffer with color
	void ClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	// Clear used screen buffers (color and depth)
	void ClearScreenBuffers(void);
	// Check and log OpenGL error codes
	void CheckErrors(void);
	// Set blend mode
	void SetBlendMode(int mode);
	// Disable point mode
	void DisablePointMode(void);
	// Enable point mode
	void EnablePointMode(void);
	// Disable wire mode
	void DisableWireMode(void);
	// Enable wire mode
	void EnableWireMode(void);
	// Scissor test
	void Scissor(int x, int y, int width, int height);
	// Disable scissor test
	void DisableScissorTest(void);
	// Enable scissor test
	void EnableScissorTest(void);
	// Set face culling mode
	void SetCullFace(int mode);
	// Set color mask active for screen read/draw
	void ColorMask(bool r, bool g, bool b, bool a);
	// Disable backface culling
	void DisableBackfaceCulling(void);
	// Enable backface culling
	void EnableBackfaceCulling(void);
	// Disable depth write
	void DisableDepthMask(void);
	// Enable depth write
	void EnableDepthMask(void);
	// Disable depth test
	void DisableDepthTest(void);
	// Enable depth test
	void EnableDepthTest(void);
	// Enable depth test
	void EnableDepthTest(void);
	// Disable color blending
	void DisableColorBlend(void);
	// Enable color blending
	void EnableColorBlend(void);
}