#include "gl_global.h"
#include "gl_default.h"
#include <logger/logger.h>
#include <helper/mem_help.h>
#include <section/enum/material_em.h>
using namespace DRAW::GL;
 static void LoadShaderDefault(void) {
	PLGL.State.defaultShaderLocs = (int*)CALLOC(PL_MAX_SHADER_LOCATIONS, sizeof(int));
	// NOTE: All locations must be reseted to -1 (no location)
	for (int i = 0; i < PL_MAX_SHADER_LOCATIONS; i++) PLGL.State.defaultShaderLocs[i] = -1;
	const char* defaultVShaderCode =
#if defined(GRAPHICS_API_OPENGL_21)
		"#version 120                       \n"
		"attribute vec3 vertexPosition;     \n"
		"attribute vec2 vertexTexCoord;     \n"
		"attribute vec4 vertexColor;        \n"
		"varying vec2 fragTexCoord;         \n"
		"varying vec4 fragColor;            \n"
#elif defined(GRAPHICS_API_OPENGL_33)
		"#version 330                       \n"
		"in vec3 vertexPosition;            \n"
		"in vec2 vertexTexCoord;            \n"
		"in vec4 vertexColor;               \n"
		"out vec2 fragTexCoord;             \n"
		"out vec4 fragColor;                \n"
#endif

#if defined(GRAPHICS_API_OPENGL_ES3)
		"#version 300 es                    \n"
		"precision mediump float;           \n"     // Precision required for OpenGL ES3 (WebGL 2) (on some browsers)
		"in vec3 vertexPosition;            \n"
		"in vec2 vertexTexCoord;            \n"
		"in vec4 vertexColor;               \n"
		"out vec2 fragTexCoord;             \n"
		"out vec4 fragColor;                \n"
#elif defined(GRAPHICS_API_OPENGL_ES2)
		"#version 100                       \n"
		"precision mediump float;           \n"     // Precision required for OpenGL ES2 (WebGL) (on some browsers)
		"attribute vec3 vertexPosition;     \n"
		"attribute vec2 vertexTexCoord;     \n"
		"attribute vec4 vertexColor;        \n"
		"varying vec2 fragTexCoord;         \n"
		"varying vec4 fragColor;            \n"
#endif

		"uniform mat4 mvp;                  \n"
		"void main()                        \n"
		"{                                  \n"
		"    fragTexCoord = vertexTexCoord; \n"
		"    fragColor = vertexColor;       \n"
		"    gl_Position = mvp*vec4(vertexPosition, 1.0); \n"
		"}                                  \n";
	// Fragment shader directly defined, no external file required
	const char* defaultFShaderCode =
#if defined(GRAPHICS_API_OPENGL_21)
		"#version 120                       \n"
		"varying vec2 fragTexCoord;         \n"
		"varying vec4 fragColor;            \n"
		"uniform sampler2D texture0;        \n"
		"uniform vec4 colDiffuse;           \n"
		"void main()                        \n"
		"{                                  \n"
		"    vec4 texelColor = texture2D(texture0, fragTexCoord); \n"
		"    gl_FragColor = texelColor*colDiffuse*fragColor;      \n"
		"}                                  \n";
#elif defined(GRAPHICS_API_OPENGL_33)
		"#version 330       \n"
		"in vec2 fragTexCoord;              \n"
		"in vec4 fragColor;                 \n"
		"out vec4 finalColor;               \n"
		"uniform sampler2D texture0;        \n"
		"uniform vec4 colDiffuse;           \n"
		"void main()                        \n"
		"{                                  \n"
		"    vec4 texelColor = texture(texture0, fragTexCoord);   \n"
		"    finalColor = texelColor*colDiffuse*fragColor;        \n"
		"}                                  \n";
#endif
#if defined(GRAPHICS_API_OPENGL_ES3)
	"#version 300 es                    \n"
		"precision mediump float;           \n"     // Precision required for OpenGL ES3 (WebGL 2)
		"in vec2 fragTexCoord;              \n"
		"in vec4 fragColor;                 \n"
		"out vec4 finalColor;               \n"
		"uniform sampler2D texture0;        \n"
		"uniform vec4 colDiffuse;           \n"
		"void main()                        \n"
		"{                                  \n"
		"    vec4 texelColor = texture(texture0, fragTexCoord);   \n"
		"    finalColor = texelColor*colDiffuse*fragColor;        \n"
		"}                                  \n";
#elif defined(GRAPHICS_API_OPENGL_ES2)
	"#version 100                       \n"
		"precision mediump float;           \n"     // Precision required for OpenGL ES2 (WebGL)
		"varying vec2 fragTexCoord;         \n"
		"varying vec4 fragColor;            \n"
		"uniform sampler2D texture0;        \n"
		"uniform vec4 colDiffuse;           \n"
		"void main()                        \n"
		"{                                  \n"
		"    vec4 texelColor = texture2D(texture0, fragTexCoord); \n"
		"    gl_FragColor = texelColor*colDiffuse*fragColor;      \n"
		"}                                  \n";
#endif
	// NOTE: Compiled vertex/fragment shaders are not deleted,
	// they are kept for re-use as default shaders in case some shader loading fails
	PLGL.State.defaultVShaderId = CompileShader(defaultVShaderCode, GL_VERTEX_SHADER);     // Compile default vertex shader
	PLGL.State.defaultFShaderId = CompileShader(defaultFShaderCode, GL_FRAGMENT_SHADER);   // Compile default fragment shader
	PLGL.State.defaultShaderId = LoadShaderProgram(PLGL.State.defaultVShaderId, PLGL.State.defaultFShaderId);
	if (PLGL.State.defaultShaderId > 0)
	{
		TRACELOG(LOG_INFO, "SHADER: [ID %i] Default shader loaded successfully", PLGL.State.defaultShaderId);

		// Set default shader locations: attributes locations
		PLGL.State.defaultShaderLocs[SHADER_LOC_VERTEX_POSITION] = glGetAttribLocation(PLGL.State.defaultShaderId, PL_DEFAULT_SHADER_ATTRIB_NAME_POSITION);
		PLGL.State.defaultShaderLocs[SHADER_LOC_VERTEX_TEXCOORD01] = glGetAttribLocation(PLGL.State.defaultShaderId, PL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
		PLGL.State.defaultShaderLocs[SHADER_LOC_VERTEX_COLOR] = glGetAttribLocation(PLGL.State.defaultShaderId, PL_DEFAULT_SHADER_ATTRIB_NAME_COLOR);

		// Set default shader locations: uniform locations
		PLGL.State.defaultShaderLocs[SHADER_LOC_MATRIX_MVP] = glGetUniformLocation(PLGL.State.defaultShaderId, PL_DEFAULT_SHADER_UNIFORM_NAME_MVP);
		PLGL.State.defaultShaderLocs[SHADER_LOC_COLOR_DIFFUSE] = glGetUniformLocation(PLGL.State.defaultShaderId, PL_DEFAULT_SHADER_UNIFORM_NAME_COLOR);
		PLGL.State.defaultShaderLocs[SHADER_LOC_MAP_DIFFUSE] = glGetUniformLocation(PLGL.State.defaultShaderId, PL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0);
	}
	else TRACELOG(LOG_WARNING, "SHADER: [ID %i] Failed to load default shader", PLGL.State.defaultShaderId);
}


static void UnloadShaderDefault(void) {
	glDetachShader(PLGL.State.defaultShaderId, PLGL.State.defaultVShaderId);
	glDetachShader(PLGL.State.defaultShaderId, PLGL.State.defaultFShaderId);
	glDeleteShader(PLGL.State.defaultVShaderId);
	glDeleteShader(PLGL.State.defaultFShaderId);
	glDeleteProgram(PLGL.State.defaultShaderId);
	FREE(PLGL.State.defaultShaderLocs);
	TRACELOG(LOG_INFO, "SHADER: [ID %i] Default shader unloaded successfully", PLGL.State.defaultShaderId);
}

void LoadDefaultShader() {
	LoadShaderDefault();
}

void UnloadDefautlShader() {
	UnloadShaderDefault();
}

extern  PurglData PLGL;