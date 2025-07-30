#pragma once
#include "gl_program.h"

void LoadDefaultShader();

void UnloadDefautlShader();

static void LoadShaderDefault(void);
// Unload default shader
		// NOTE: Unloads: PLGL.State.defaultShaderId, PLGL.State.defaultShaderLocs
static void UnloadShaderDefault(void);


