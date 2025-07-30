#pragma once
#include <section/martrix/cg_def.h>
#include <section/enum/uniform_em.h>
using namespace Seek;

namespace DRAW {

	namespace GL {

		void SetUniform(int locIndex, const void* value, int uniformType, int count);

		int GetLocationUniform(unsigned int shaderId, const char* uniformName);
		
		void SetUniformMatrix(int locIndex, Matrix mat);

		void SetUniformMatrices(int locIndex, const Matrix* matrices, int count);

		int GetLocationAttrib(unsigned int shaderId, const char* attribName);

		// Set shader value uniform sampler
		void SetUniformSampler(int locIndex, unsigned int textureId);
	}
}