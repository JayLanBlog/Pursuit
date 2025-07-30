#include "gl_global.h"
#include "gl_program.h"
#include "gl_uniform.h"
#include <file/sys_file.h>
#include <logger/logger.h>
#include <section/enum/material_em.h>
using namespace System;

namespace DRAW {
	namespace GL {

		unsigned int LoadShaderCode(const char* vsCode, const char* fsCode) {
			unsigned int id = 0;
			unsigned int vertexShaderId = 0;
			unsigned int fragmentShaderId = 0;

			// Compile vertex shader (if provided)
			// NOTE: If not vertex shader is provided, use default one
			if (vsCode != NULL) vertexShaderId = CompileShader(vsCode, GL_VERTEX_SHADER);
			else vertexShaderId = PLGL.State.defaultVShaderId;
			// Compile fragment shader (if provided)
			// NOTE: If not vertex shader is provided, use default one
			if (fsCode != NULL) fragmentShaderId = CompileShader(fsCode, GL_FRAGMENT_SHADER);
			 else fragmentShaderId = PLGL.State.defaultFShaderId;

			// In case vertex and fragment shader are the default ones, no need to recompile, we can just assign the default shader program id
			if ((vertexShaderId == PLGL.State.defaultVShaderId) && (fragmentShaderId == PLGL.State.defaultFShaderId)) id = PLGL.State.defaultShaderId;
			else if ((vertexShaderId > 0) && (fragmentShaderId > 0)) {
				// One of or both shader are new, we need to compile a new shader program
				id = LoadShaderProgram(vertexShaderId, fragmentShaderId);
				// WARNING: Shader program linkage could fail and returned id is 0

		// We can detach and delete vertex/fragment shaders (if not default ones)
		// NOTE: We detach shader before deletion to make sure memory is freed
				if (vertexShaderId != PLGL.State.defaultVShaderId)
				{
					// WARNING: Shader program linkage could fail and returned id is 0
					if (id > 0) glDetachShader(id, vertexShaderId);
					glDeleteShader(vertexShaderId);
				}
				if (fragmentShaderId != PLGL.State.defaultFShaderId)
				{
					// WARNING: Shader program linkage could fail and returned id is 0
					if (id > 0) glDetachShader(id, fragmentShaderId);
					glDeleteShader(fragmentShaderId);
				}

				if (id == 0)
				{
					// In case shader loading fails, we return the default shader
					TRACELOG(LOG_WARNING, "SHADER: Failed to load custom shader code, using default shader");
					id = PLGL.State.defaultShaderId;
				}
			}
			return id;
		}
		Shader LoadShader(const char* vsFileName, const char* fsFileName) {
			Shader shader = { 0 };
			char* vShaderStr = NULL;
			char* fShaderStr = NULL;
			if (vsFileName != NULL) vShaderStr = LoadFileText(vsFileName);
			if (fsFileName != NULL) fShaderStr = LoadFileText(fsFileName);
			if ((vShaderStr == NULL) && (fShaderStr == NULL))TRACELOG(LOG_WARNING, "SHADER: Shader files provided are not valid, using default shader");
			shader = LoadShaderFromMemory(vShaderStr, fShaderStr);
			UnloadFileText(vShaderStr);
			UnloadFileText(fShaderStr);
			return shader;
		}

		// Get default shader locs
		int* GetShaderLocsDefault() {

			int* locs = NULL;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			locs = PLGL.State.defaultShaderLocs;
#endif
			return locs;
		}

		// Load shader from code strings and bind default locations
		Shader LoadShaderFromMemory(const char* vsCode, const char* fsCode) {
			Shader shader = { 0 };
			shader.id = LoadShaderCode(vsCode, fsCode);
			//shader.id = LoadShaderCode(vsCode, fsCode);

			if (shader.id == GetShaderIdDefault()) shader.locs = GetShaderLocsDefault();
			else if (shader.id > 0)
			{
				// After custom shader loading, we TRY to set default location names
				// Default shader attribute locations have been binded before linking:
				//          vertex position location    = 0
				//          vertex texcoord location    = 1
				//          vertex normal location      = 2
				//          vertex color location       = 3
				//          vertex tangent location     = 4
				//          vertex texcoord2 location   = 5
				//          vertex boneIds location     = 6
				//          vertex boneWeights location = 7

				// NOTE: If any location is not found, loc point becomes -1

				shader.locs = (int*)CALLOC(PL_MAX_SHADER_LOCATIONS, sizeof(int));

				// All locations reset to -1 (no location)
				for (int i = 0; i < PL_MAX_SHADER_LOCATIONS; i++) shader.locs[i] = -1;

				// Get handles to GLSL input attribute locations
				shader.locs[SHADER_LOC_VERTEX_POSITION] = GetLocationAttrib(shader.id, PL_DEFAULT_SHADER_ATTRIB_NAME_POSITION);
				shader.locs[SHADER_LOC_VERTEX_TEXCOORD01] = GetLocationAttrib(shader.id, PL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
				shader.locs[SHADER_LOC_VERTEX_TEXCOORD02] = GetLocationAttrib(shader.id, PL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2);
				shader.locs[SHADER_LOC_VERTEX_NORMAL] = GetLocationAttrib(shader.id, PL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL);
				shader.locs[SHADER_LOC_VERTEX_TANGENT] = GetLocationAttrib(shader.id, PL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT);
				shader.locs[SHADER_LOC_VERTEX_COLOR] = GetLocationAttrib(shader.id, PL_DEFAULT_SHADER_ATTRIB_NAME_COLOR);
				shader.locs[SHADER_LOC_VERTEX_BONEIDS] = GetLocationAttrib(shader.id, PL_DEFAULT_SHADER_ATTRIB_NAME_BONEIDS);
				shader.locs[SHADER_LOC_VERTEX_BONEWEIGHTS] = GetLocationAttrib(shader.id, PL_DEFAULT_SHADER_ATTRIB_NAME_BONEWEIGHTS);
				shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX] = GetLocationAttrib(shader.id, PL_DEFAULT_SHADER_ATTRIB_NAME_INSTANCE_TX);

				// Get handles to GLSL uniform locations (vertex shader)
				shader.locs[SHADER_LOC_MATRIX_MVP] = GetLocationUniform(shader.id, PL_DEFAULT_SHADER_UNIFORM_NAME_MVP);
				shader.locs[SHADER_LOC_MATRIX_VIEW] = GetLocationUniform(shader.id, PL_DEFAULT_SHADER_UNIFORM_NAME_VIEW);
				shader.locs[SHADER_LOC_MATRIX_PROJECTION] = GetLocationUniform(shader.id, PL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION);
				shader.locs[SHADER_LOC_MATRIX_MODEL] = GetLocationUniform(shader.id, PL_DEFAULT_SHADER_UNIFORM_NAME_MODEL);
				shader.locs[SHADER_LOC_MATRIX_NORMAL] = GetLocationUniform(shader.id, PL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL);
				shader.locs[SHADER_LOC_BONE_MATRICES] = GetLocationUniform(shader.id, PL_DEFAULT_SHADER_UNIFORM_NAME_BONE_MATRICES);

				// Get handles to GLSL uniform locations (fragment shader)
				shader.locs[SHADER_LOC_COLOR_DIFFUSE] = GetLocationUniform(shader.id, PL_DEFAULT_SHADER_UNIFORM_NAME_COLOR);
				shader.locs[SHADER_LOC_MAP_DIFFUSE] = GetLocationUniform(shader.id, PL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0);  // SHADER_LOC_MAP_ALBEDO
				shader.locs[SHADER_LOC_MAP_SPECULAR] = GetLocationUniform(shader.id, PL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1); // SHADER_LOC_MAP_METALNESS
				shader.locs[SHADER_LOC_MAP_NORMAL] = GetLocationUniform(shader.id, PL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2);
			}

			return shader;
		}

		// Check if a shader is valid (loaded on GPU)
		bool IsShaderValid(Shader shader) {
			return ((shader.id > 0) &&          // Validate shader id (GPU loaded successfully)
				(shader.locs != NULL));     // Validate memory has been allocated for default shader locations

		}

		// Unload shader from GPU memory (VRAM)
		void UnloadShader(Shader shader) {
			if (shader.id != GetShaderIdDefault())
			{
				UnloadShaderProgram(shader.id);

				// NOTE: If shader loading failed, it should be 0
				FREE(shader.locs);
			}
		}


		// Set shader uniform value
		void SetShaderValue(Shader shader, int locIndex, const void* value, int uniformType) {
			SetShaderValueV(shader, locIndex, value, uniformType, 1);
		}

		// Set shader uniform value vector
		void SetShaderValueV(Shader shader, int locIndex, const void* value, int uniformType, int count) {
			if (locIndex > -1)
			{
				EnableShader(shader.id);
				SetUniform(locIndex, value, uniformType, count);
				DisableShader();      // Avoid resetting current shader program, in case other uniforms are set
			}
		}

		// Set shader uniform value (matrix 4x4)
		void SetShaderValueMatrix(Shader shader, int locIndex, Matrix mat) {
			if (locIndex > -1)
			{
				EnableShader(shader.id);
				SetUniformMatrix(locIndex, mat);
				DisableShader();
			}
		}

		// Set shader uniform value for texture
		void SetShaderValueTexture(Shader shader, int locIndex, Texture2D texture) {
			if (locIndex > -1)
			{
				EnableShader(shader.id);
				SetUniformSampler(locIndex, texture.id);
				DisableShader();
			}
		}

		// Enable shader program
		void EnableShader(unsigned int id) {
			glUseProgram(id);
		}

		// Disable shader program
		void DisableShader(void) {
			glUseProgram(0);
		}

		unsigned int CompileShader(const char* shaderCode, int type) {
			unsigned int shader = 0;
			shader = glCreateShader(type);
			glShaderSource(shader, 1, &shaderCode, NULL);

			GLint success = 0;
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

			if (success == GL_FALSE) {
				switch (type)
				{
				case GL_VERTEX_SHADER: TRACELOG(LOG_WARNING, "SHADER: [ID %i] Failed to compile vertex shader code", shader); break;
				case GL_FRAGMENT_SHADER: TRACELOG(LOG_WARNING, "SHADER: [ID %i] Failed to compile fragment shader code", shader); break;
					//case GL_GEOMETRY_SHADER:
			//	case GL_COMPILE: TRACELOG(LOG_WARNING, "SHADER: Compute shaders not enabled. Define GRAPHICS_API_OPENGL_43", shader); break;

				default: break;
				}
			}

			int maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
			if (maxLength > 0)
			{
				int length = 0;
				char* log = (char*)CALLOC(maxLength, sizeof(char));
				glGetShaderInfoLog(shader, maxLength, &length, log);
				TRACELOG(LOG_WARNING, "SHADER: [ID %i] Compile error: %s", shader, log);
				FREE(log);
				// Unload object allocated by glCreateShader(),
	// despite failing in the compilation process
				glDeleteShader(shader);
				shader = 0;
			}
			else
			{
				switch (type)
				{
				case GL_VERTEX_SHADER: TRACELOG(LOG_INFO, "SHADER: [ID %i] Vertex shader compiled successfully", shader); break;
				case GL_FRAGMENT_SHADER: TRACELOG(LOG_INFO, "SHADER: [ID %i] Fragment shader compiled successfully", shader); break;
					//case GL_GEOMETRY_SHADER:
#if defined(GRAPHICS_API_OPENGL_43)
				case GL_COMPUTE_SHADER: TRACELOG(PL_LOG_INFO, "SHADER: [ID %i] Compute shader compiled successfully", shader); break;
#elif defined(GRAPHICS_API_OPENGL_33)
				//case GL_COMPUTE_SHADER	: TRACELOG(LOG_INFO, "SHADER: Compute shaders not enabled. Define GRAPHICS_API_OPENGL_43", shader); break;
#endif
				default: break;
				}
			}
			return shader;
		}

		unsigned int LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId) {
			unsigned int program = 0;
			GLint success = 0;
			program = glCreateProgram();
			glAttachShader(program, vShaderId);
			glAttachShader(program, fShaderId);
			// NOTE: Default attribute shader locations must be Bound before linking
			glBindAttribLocation(program, PL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION, PL_DEFAULT_SHADER_ATTRIB_NAME_POSITION);
			glBindAttribLocation(program, PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD, PL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
			glBindAttribLocation(program, PL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL, PL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL);
			glBindAttribLocation(program, PL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR, PL_DEFAULT_SHADER_ATTRIB_NAME_COLOR);
			glBindAttribLocation(program, PL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT, PL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT);
			glBindAttribLocation(program, PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2, PL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2);
			glBindAttribLocation(program, PL_DEFAULT_SHADER_ATTRIB_LOCATION_INSTANCE_TX, PL_DEFAULT_SHADER_ATTRIB_NAME_INSTANCE_TX);
#ifdef PL_SUPPORT_MESH_GPU_SKINNING
			glBindAttribLocation(program, PL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEIDS, PL_DEFAULT_SHADER_ATTRIB_NAME_BONEIDS);
			glBindAttribLocation(program, PL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS, PL_DEFAULT_SHADER_ATTRIB_NAME_BONEWEIGHTS);
#endif
			// NOTE: If some attrib name is no found on the shader, it locations becomes -1
			glLinkProgram(program);
			// NOTE: All uniform variables are intitialised to 0 when a program links
			glGetProgramiv(program, GL_LINK_STATUS, &success);
			if (success == GL_FALSE)
			{
				TRACELOG(LOG_WARNING, "SHADER: [ID %i] Failed to link shader program", program);

				int maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				if (maxLength > 0)
				{
					int length = 0;
					char* log = (char*)CALLOC(maxLength, sizeof(char));
					glGetProgramInfoLog(program, maxLength, &length, log);
					TRACELOG(LOG_WARNING, "SHADER: [ID %i] Link error: %s", program, log);
					FREE(log);
				}
				else
				{
					// Get the size of compiled shader program (not available on OpenGL ES 2.0)
					// NOTE: If GL_LINK_STATUS is GL_FALSE, program binary length is zero
					//GLint binarySize = 0;
					//glGetProgramiv(id, GL_PROGRAM_BINARY_LENGTH, &binarySize);

					TRACELOG(LOG_INFO, "SHADER: [ID %i] Program shader loaded successfully", program);
				}

				glDeleteProgram(program);

				program = 0;
			}
			return program;
		}
		//unsigned int rlCompileShader(const char* shaderCode, int type);

		// Get default shader id
		unsigned int GetShaderIdDefault(void) {
			unsigned int id = 0;
			id = PLGL.State.defaultShaderId;
			return id;
		}

		// Get default shader locs
		int* glGetShaderLocsDefault(void) {
			int* locs = NULL;
			locs = PLGL.State.defaultShaderLocs;
			return locs;
		}

		// Unload shader program
		void UnloadShaderProgram(unsigned int id) {
			glDeleteProgram(id);

			TRACELOG(LOG_INFO, "SHADER: [ID %i] Unloaded shader program data from VRAM (GPU)", id);
		}

		// Load compute shader program
		unsigned int LoadComputeShaderProgram(unsigned int shaderId) {
			unsigned int program = 0;

#if defined(GRAPHICS_API_OPENGL_43)
			GLint success = 0;
			program = glCreateProgram();
			glAttachShader(program, shaderId);
			glLinkProgram(program);

			// NOTE: All uniform variables are intitialised to 0 when a program links

			glGetProgramiv(program, GL_LINK_STATUS, &success);

			if (success == GL_FALSE)
			{
				TRACELOG(LOG_WARNING, "SHADER: [ID %i] Failed to link compute shader program", program);

				int maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				if (maxLength > 0)
				{
					int length = 0;
					char* log = (char*)CALLOC(maxLength, sizeof(char));
					glGetProgramInfoLog(program, maxLength, &length, log);
					TRACELOG(WARNING, "SHADER: [ID %i] Link error: %s", program, log);
					FREE(log);
				}

				glDeleteProgram(program);

				program = 0;
			}
			else
			{
				// Get the size of compiled shader program (not available on OpenGL ES 2.0)
				// NOTE: If GL_LINK_STATUS is GL_FALSE, program binary length is zero
				//GLint binarySize = 0;
				//glGetProgramiv(id, GL_PROGRAM_BINARY_LENGTH, &binarySize);

				TRACELOG(LOG_INFO, "SHADER: [ID %i] Compute shader program loaded successfully", program);
			}
#else
			TRACELOG(LOG_WARNING, "SHADER: Compute shaders not enabled. Define GRAPHICS_API_OPENGL_43");
#endif

			return program;
		}

		// Dispatch compute shader (equivalent to *draw* for graphics pilepine)
		void ComputeShaderDispatch(unsigned int groupX, unsigned int groupY, unsigned int groupZ) {
#if defined(GRAPHICS_API_OPENGL_43)
			glDispatchCompute(groupX, groupY, groupZ);
#endif
		}

		// Load shader storage buffer object (SSBO)
		unsigned int LoadShaderBuffer(unsigned int size, const void* data, int usageHint) {
			unsigned int ssbo = 0;

#if defined(GRAPHICS_API_OPENGL_43)
			glGenBuffers(1, &ssbo);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
			glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usageHint ? usageHint : PL_STREAM_COPY);
			if (data == NULL) glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);    // Clear buffer data to 0
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#else
			TRACELOG(LOG_WARNING, "SSBO: SSBO not enabled. Define GRAPHICS_API_OPENGL_43");
#endif

			return ssbo;
		}

		

	}
}