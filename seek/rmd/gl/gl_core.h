#pragma once
#include <section/martrix/cg_def.h>
#include "gl_global.h"

using namespace Seek;
namespace RHICore {

	// Get internal modelview matrix
	Matrix GetMatrixModelview(void);

	// Get internal projection matrix
	Matrix GetMatrixProjection(void);

	// Get internal accumulated transform matrix
	Matrix GetMatrixTransform(void);

	// Get internal projection matrix for stereo render (selected eye)
	Matrix GetMatrixProjectionStereo(int eye);

	// Set a custom modelview matrix (replaces internal modelview matrix)
	void SetMatrixModelview(Matrix view);

	// Set a custom projection matrix (replaces internal projection matrix)
	void SetMatrixProjection(Matrix projection);

	// Set eyes projection matrices for stereo rendering
	void SetMatrixProjectionStereo(Matrix right, Matrix left);

	// Set eyes view offsets matrices for stereo rendering
	void SetMatrixViewOffsetStereo(Matrix right, Matrix left);

	// Check if stereo render is enabled
	bool IsStereoRenderEnabled(void);

	// Disable stereo rendering
	void DisableStereoRender(void);

	// Enable stereo rendering
	void EnableStereoRender(void);


	// Set blending mode factor and equation
	void SetBlendFactors(int glSrcFactor, int glDstFactor, int glEquation);

	// Set blending mode factor and equation separately for RGB and alpha
	void SetBlendFactorsSeparate(int glSrcRGB, int glDstRGB, int glSrcAlpha, int glDstAlpha, int glEqRGB, int glEqAlpha);


}