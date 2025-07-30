#pragma once
// Shader uniform data type
typedef enum {
    PL_SHADER_UNIFORM_FLOAT = 0,        // Shader uniform type: float
    PL_SHADER_UNIFORM_VEC2,             // Shader uniform type: vec2 (2 float)
    PL_SHADER_UNIFORM_VEC3,             // Shader uniform type: vec3 (3 float)
    PL_SHADER_UNIFORM_VEC4,             // Shader uniform type: vec4 (4 float)
    PL_SHADER_UNIFORM_INT,              // Shader uniform type: int
    PL_SHADER_UNIFORM_IVEC2,            // Shader uniform type: ivec2 (2 int)
    PL_SHADER_UNIFORM_IVEC3,            // Shader uniform type: ivec3 (3 int)
    PL_SHADER_UNIFORM_IVEC4,            // Shader uniform type: ivec4 (4 int)
    PL_SHADER_UNIFORM_UINT,             // Shader uniform type: unsigned int
    PL_SHADER_UNIFORM_UIVEC2,           // Shader uniform type: uivec2 (2 unsigned int)
    PL_SHADER_UNIFORM_UIVEC3,           // Shader uniform type: uivec3 (3 unsigned int)
    PL_SHADER_UNIFORM_UIVEC4,           // Shader uniform type: uivec4 (4 unsigned int)
    PL_SHADER_UNIFORM_SAMPLER2D         // Shader uniform type: sampler2d
} plShaderUniformDataType;


// Shader uniform data type
typedef enum {
    SHADER_UNIFORM_FLOAT = 0,       // Shader uniform type: float
    SHADER_UNIFORM_VEC2,            // Shader uniform type: vec2 (2 float)
    SHADER_UNIFORM_VEC3,            // Shader uniform type: vec3 (3 float)
    SHADER_UNIFORM_VEC4,            // Shader uniform type: vec4 (4 float)
    SHADER_UNIFORM_INT,             // Shader uniform type: int
    SHADER_UNIFORM_IVEC2,           // Shader uniform type: ivec2 (2 int)
    SHADER_UNIFORM_IVEC3,           // Shader uniform type: ivec3 (3 int)
    SHADER_UNIFORM_IVEC4,           // Shader uniform type: ivec4 (4 int)
    SHADER_UNIFORM_UINT,            // Shader uniform type: unsigned int
    SHADER_UNIFORM_UIVEC2,          // Shader uniform type: uivec2 (2 unsigned int)
    SHADER_UNIFORM_UIVEC3,          // Shader uniform type: uivec3 (3 unsigned int)
    SHADER_UNIFORM_UIVEC4,          // Shader uniform type: uivec4 (4 unsigned int)
    SHADER_UNIFORM_SAMPLER2D        // Shader uniform type: sampler2d
} ShaderUniformDataType;