#include "gl_global.h"
#include "gl_uniform.h"
#include <section/enum/uniform_em.h>
#include <logger/logger.h>


namespace DRAW {

	namespace GL {

		void SetUniform(int locIndex, const void* value, int uniformType, int count) {
			switch (uniformType)
			{
			case PL_SHADER_UNIFORM_FLOAT: glUniform1fv(locIndex, count, (float*)value); break;
			case PL_SHADER_UNIFORM_VEC2: glUniform2fv(locIndex, count, (float*)value); break;
			case PL_SHADER_UNIFORM_VEC3: glUniform3fv(locIndex, count, (float*)value); break;
			case PL_SHADER_UNIFORM_VEC4: glUniform4fv(locIndex, count, (float*)value); break;
			case PL_SHADER_UNIFORM_INT: glUniform1iv(locIndex, count, (int*)value); break;
			case PL_SHADER_UNIFORM_IVEC2: glUniform2iv(locIndex, count, (int*)value); break;
			case PL_SHADER_UNIFORM_IVEC3: glUniform3iv(locIndex, count, (int*)value); break;
			case PL_SHADER_UNIFORM_IVEC4: glUniform4iv(locIndex, count, (int*)value); break;
#if !defined(GRAPHICS_API_OPENGL_ES2)
			case PL_SHADER_UNIFORM_UINT: glUniform1uiv(locIndex, count, (unsigned int*)value); break;
			case PL_SHADER_UNIFORM_UIVEC2: glUniform2uiv(locIndex, count, (unsigned int*)value); break;
			case PL_SHADER_UNIFORM_UIVEC3: glUniform3uiv(locIndex, count, (unsigned int*)value); break;
			case PL_SHADER_UNIFORM_UIVEC4: glUniform4uiv(locIndex, count, (unsigned int*)value); break;
#endif
			case PL_SHADER_UNIFORM_SAMPLER2D: glUniform1iv(locIndex, count, (int*)value); break;
			default: TRACELOG(LOG_WARNING, "SHADER: Failed to set uniform value, data type not recognized");
				// TODO: Support glUniform1uiv(), glUniform2uiv(), glUniform3uiv(), glUniform4uiv()
			}
		}

		int GetLocationAttrib(unsigned int shaderId, const char* attribName) {
			int location = -1;
			location = glGetAttribLocation(shaderId, attribName);
			return location;
		}

		int GetLocationUniform(unsigned int shaderId, const char* uniformName) {
			int location = -1;
			location = glGetUniformLocation(shaderId, uniformName);
			return location;
		}

		void SetUniformMatrix(int locIndex, Matrix mat) {
			float matfloat[16] = {
				mat.m0, mat.m1, mat.m2, mat.m3,
				mat.m4, mat.m5, mat.m6, mat.m7,
				mat.m8, mat.m9, mat.m10, mat.m11,
				mat.m12, mat.m13, mat.m14, mat.m15
			};
			glUniformMatrix4fv(locIndex, 1, false, matfloat);
		}

		void SetUniformMatrices(int locIndex, const Matrix* matrices, int count) {
			glUniformMatrix4fv(locIndex, count, true, (const float*)matrices);
		}

		// Set shader value uniform sampler
		void SetUniformSampler(int locIndex, unsigned int textureId) {
			// Check if texture is already active
			for (int i = 0; i < PL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++)
			{
				if (PLGL.State.activeTextureId[i] == textureId)
				{
					glUniform1i(locIndex, 1 + i);
					return;
				}

				// NOTE: Default texture is always activated as GL_TEXTURE0
				for (int i = 0; i < PL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++)
				{
					if (PLGL.State.activeTextureId[i] == 0)
					{
						glUniform1i(locIndex, 1 + i);              // Activate new texture unit
						PLGL.State.activeTextureId[i] = textureId; // Save texture id for binding on drawing
						break;
					}
				}
			}
		}
	}
}


