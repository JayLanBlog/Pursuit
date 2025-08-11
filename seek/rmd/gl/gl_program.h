#pragma once
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "gl_define.h"
#include <section/graph/material_def.h>
#include <section/martrix/cg_def.h>
#include <section/graph/shader_def.h>
using namespace Seek;
namespace DRAW {
	namespace GL {
		Shader LoadShader(const char* vsFileName, const char* fsFileName);
		// Load shader from code strings and bind default locations
		Shader LoadShaderFromMemory(const char* vsCode, const char* fsCode);
		unsigned int LoadShaderCode(const char* vsCode, const char* fsCode);
		// Check if a shader is valid (loaded on GPU)
		bool IsShaderValid(Shader shader);
		// Unload shader from GPU memory (VRAM)
		void UnloadShader(Shader shader);
		// Set shader uniform value
		void SetShaderValue(Shader shader, int locIndex, const void* value, int uniformType);
		// Set shader uniform value vector
		void SetShaderValueV(Shader shader, int locIndex, const void* value, int uniformType, int count);
		// Set shader uniform value (matrix 4x4)
		void SetShaderValueMatrix(Shader shader, int locIndex, Matrix mat);
		// Set shader uniform value for texture
		void SetShaderValueTexture(Shader shader, int locIndex, Texture2D texture);
		// Enable shader program
		void EnableShader(unsigned int id);
		// Disable shader program
		void DisableShader(void);
		unsigned int CompileShader(const char* shaderCode, int type);
		unsigned int LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId);
		//unsigned int rlCompileShader(const char* shaderCode, int type);
		// Get default shader id
		unsigned int GetShaderIdDefault(void);
		// Get default shader locs
		int* glGetShaderLocsDefault(void);
		unsigned int CompileShader(const char* shaderCode, int type);
		// Load custom shader strings and return program id
		unsigned int LoadShaderProgram(unsigned int vShaderId, unsigned int fShaderId);
		// Unload shader program
		void UnloadShaderProgram(unsigned int id);
		// Load compute shader program
		unsigned int LoadComputeShaderProgram(unsigned int shaderId);
		// Dispatch compute shader (equivalent to *draw* for graphics pilepine)
		void ComputeShaderDispatch(unsigned int groupX, unsigned int groupY, unsigned int groupZ);
		// Load shader storage buffer object (SSBO)
		 unsigned int LoadShaderBuffer(unsigned int size, const void* data, int usageHint);
		 int* GetShaderLocsDefault(void);

		 void SetShader(unsigned int id, int* locs);
	}
}