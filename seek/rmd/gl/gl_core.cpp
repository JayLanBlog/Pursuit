#include "gl_core.h"
#include <math/pr_math.h>

using namespace PMath;

namespace RHICore {

	// Get internal modelview matrix
	Matrix GetMatrixModelview(void) {
		Matrix matrix = MatrixIdentity();
#if defined(GRAPHICS_API_OPENGL_11)
		float mat[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		matrix.m0 = mat[0];
		matrix.m1 = mat[1];
		matrix.m2 = mat[2];
		matrix.m3 = mat[3];
		matrix.m4 = mat[4];
		matrix.m5 = mat[5];
		matrix.m6 = mat[6];
		matrix.m7 = mat[7];
		matrix.m8 = mat[8];
		matrix.m9 = mat[9];
		matrix.m10 = mat[10];
		matrix.m11 = mat[11];
		matrix.m12 = mat[12];
		matrix.m13 = mat[13];
		matrix.m14 = mat[14];
		matrix.m15 = mat[15];
#else
		matrix = PLGL.State.modelview;
#endif
		return matrix;

	}

	// Get internal projection matrix
	Matrix GetMatrixProjection(void) {
#if defined(GRAPHICS_API_OPENGL_11)
		float mat[16];
		glGetFloatv(GL_PROJECTION_MATRIX, mat);
		Matrix m;
		m.m0 = mat[0];
		m.m1 = mat[1];
		m.m2 = mat[2];
		m.m3 = mat[3];
		m.m4 = mat[4];
		m.m5 = mat[5];
		m.m6 = mat[6];
		m.m7 = mat[7];
		m.m8 = mat[8];
		m.m9 = mat[9];
		m.m10 = mat[10];
		m.m11 = mat[11];
		m.m12 = mat[12];
		m.m13 = mat[13];
		m.m14 = mat[14];
		m.m15 = mat[15];
		return m;
#else
		return PLGL.State.projection;
#endif
	}

	// Get internal accumulated transform matrix
	Matrix GetMatrixTransform(void) {
		Matrix mat = MatrixIdentity();
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		// TODO: Consider possible transform matrices in the PLGL.State.stack
		// Is this the right order? or should we start with the first stored matrix instead of the last one?
		//Matrix matStackTransform = rlMatrixIdentity();
		//for (int i = PLGL.State.stackCounter; i > 0; i--) matStackTransform = rlMatrixMultiply(PLGL.State.stack[i], matStackTransform);
		mat = PLGL.State.transform;
#endif
		return mat;
	}

	// Get internal projection matrix for stereo render (selected eye)
	Matrix GetMatrixProjectionStereo(int eye) {
		Matrix mat = MatrixIdentity();
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		mat = PLGL.State.projectionStereo[eye];
#endif
		return mat;
	}

	// Set a custom modelview matrix (replaces internal modelview matrix)
	void SetMatrixModelview(Matrix view) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		PLGL.State.modelview = view;
#endif
	}

	// Set a custom projection matrix (replaces internal projection matrix)
	void SetMatrixProjection(Matrix projection) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		PLGL.State.projection = projection;
#endif
	}

	// Set eyes projection matrices for stereo rendering
	void SetMatrixProjectionStereo(Matrix right, Matrix left) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		PLGL.State.projectionStereo[0] = right;
		PLGL.State.projectionStereo[1] = left;
#endif
	}

	// Set eyes view offsets matrices for stereo rendering
	void SetMatrixViewOffsetStereo(Matrix right, Matrix left) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		PLGL.State.viewOffsetStereo[0] = right;
		PLGL.State.viewOffsetStereo[1] = left;
#endif
	}

	// Check if stereo render is enabled
	bool IsStereoRenderEnabled(void) {
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2))
		return PLGL.State.stereoRender;
#else
		return false;
#endif
	}

	// Disable stereo rendering
	void DisableStereoRender(void){

#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2))
		PLGL.State.stereoRender = false;
#endif
	}

	// Enable stereo rendering
	void EnableStereoRender(void) {
#if (defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2))
		PLGL.State.stereoRender = true;
#endif
	}


	// Set blending mode factor and equation
	void SetBlendFactors(int glSrcFactor, int glDstFactor, int glEquation) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		if ((PLGL.State.glBlendSrcFactor != glSrcFactor) ||
			(PLGL.State.glBlendDstFactor != glDstFactor) ||
			(PLGL.State.glBlendEquation != glEquation))
		{
			PLGL.State.glBlendSrcFactor = glSrcFactor;
			PLGL.State.glBlendDstFactor = glDstFactor;
			PLGL.State.glBlendEquation = glEquation;
			PLGL.State.glCustomBlendModeModified = true;
		}
#endif
	}

	// Set blending mode factor and equation separately for RGB and alpha
	void SetBlendFactorsSeparate(int glSrcRGB, int glDstRGB, int glSrcAlpha, int glDstAlpha, int glEqRGB, int glEqAlpha) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		if ((PLGL.State.glBlendSrcFactorRGB != glSrcRGB) ||
			(PLGL.State.glBlendDestFactorRGB != glDstRGB) ||
			(PLGL.State.glBlendSrcFactorAlpha != glSrcAlpha) ||
			(PLGL.State.glBlendDestFactorAlpha != glDstAlpha) ||
			(PLGL.State.glBlendEquationRGB != glEqRGB) ||
			(PLGL.State.glBlendEquationAlpha != glEqAlpha))
		{
			PLGL.State.glBlendSrcFactorRGB = glSrcRGB;
			PLGL.State.glBlendDestFactorRGB = glDstRGB;
			PLGL.State.glBlendSrcFactorAlpha = glSrcAlpha;
			PLGL.State.glBlendDestFactorAlpha = glDstAlpha;
			PLGL.State.glBlendEquationRGB = glEqRGB;
			PLGL.State.glBlendEquationAlpha = glEqAlpha;
			PLGL.State.glCustomBlendModeModified = true;
		}
#endif
	}



}