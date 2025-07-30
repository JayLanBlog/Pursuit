#pragma once
// Material map index
typedef enum {
    MATERIAL_MAP_ALBEDO = 0,        // Albedo material (same as: MATERIAL_MAP_DIFFUSE)
    MATERIAL_MAP_METALNESS,         // Metalness material (same as: MATERIAL_MAP_SPECULAR)
    MATERIAL_MAP_NORMAL,            // Normal material
    MATERIAL_MAP_ROUGHNESS,         // Roughness material
    MATERIAL_MAP_OCCLUSION,         // Ambient occlusion material
    MATERIAL_MAP_EMISSION,          // Emission material
    MATERIAL_MAP_HEIGHT,            // Heightmap material
    MATERIAL_MAP_CUBEMAP,           // Cubemap material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    MATERIAL_MAP_IRRADIANCE,        // Irradiance material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    MATERIAL_MAP_PREFILTER,         // Prefilter material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    MATERIAL_MAP_BRDF               // Brdf material
} MaterialMapIndex;

#define MATERIAL_MAP_DIFFUSE      MATERIAL_MAP_ALBEDO
#define MATERIAL_MAP_SPECULAR     MATERIAL_MAP_METALNESS

// Shader location index
typedef enum {
    SHADER_LOC_VERTEX_POSITION = 0, // Shader location: vertex attribute: position
    SHADER_LOC_VERTEX_TEXCOORD01,   // Shader location: vertex attribute: texcoord01
    SHADER_LOC_VERTEX_TEXCOORD02,   // Shader location: vertex attribute: texcoord02
    SHADER_LOC_VERTEX_NORMAL,       // Shader location: vertex attribute: normal
    SHADER_LOC_VERTEX_TANGENT,      // Shader location: vertex attribute: tangent
    SHADER_LOC_VERTEX_COLOR,        // Shader location: vertex attribute: color
    SHADER_LOC_MATRIX_MVP,          // Shader location: matrix uniform: model-view-projection
    SHADER_LOC_MATRIX_VIEW,         // Shader location: matrix uniform: view (camera transform)
    SHADER_LOC_MATRIX_PROJECTION,   // Shader location: matrix uniform: projection
    SHADER_LOC_MATRIX_MODEL,        // Shader location: matrix uniform: model (transform)
    SHADER_LOC_MATRIX_NORMAL,       // Shader location: matrix uniform: normal
    SHADER_LOC_VECTOR_VIEW,         // Shader location: vector uniform: view
    SHADER_LOC_COLOR_DIFFUSE,       // Shader location: vector uniform: diffuse color
    SHADER_LOC_COLOR_SPECULAR,      // Shader location: vector uniform: specular color
    SHADER_LOC_COLOR_AMBIENT,       // Shader location: vector uniform: ambient color
    SHADER_LOC_MAP_ALBEDO,          // Shader location: sampler2d texture: albedo (same as: SHADER_LOC_MAP_DIFFUSE)
    SHADER_LOC_MAP_METALNESS,       // Shader location: sampler2d texture: metalness (same as: SHADER_LOC_MAP_SPECULAR)
    SHADER_LOC_MAP_NORMAL,          // Shader location: sampler2d texture: normal
    SHADER_LOC_MAP_ROUGHNESS,       // Shader location: sampler2d texture: roughness
    SHADER_LOC_MAP_OCCLUSION,       // Shader location: sampler2d texture: occlusion
    SHADER_LOC_MAP_EMISSION,        // Shader location: sampler2d texture: emission
    SHADER_LOC_MAP_HEIGHT,          // Shader location: sampler2d texture: height
    SHADER_LOC_MAP_CUBEMAP,         // Shader location: samplerCube texture: cubemap
    SHADER_LOC_MAP_IRRADIANCE,      // Shader location: samplerCube texture: irradiance
    SHADER_LOC_MAP_PREFILTER,       // Shader location: samplerCube texture: prefilter
    SHADER_LOC_MAP_BRDF,            // Shader location: sampler2d texture: brdf
    SHADER_LOC_VERTEX_BONEIDS,      // Shader location: vertex attribute: boneIds
    SHADER_LOC_VERTEX_BONEWEIGHTS,  // Shader location: vertex attribute: boneWeights
    SHADER_LOC_BONE_MATRICES,       // Shader location: array of matrices uniform: boneMatrices
    SHADER_LOC_VERTEX_INSTANCE_TX   // Shader location: vertex attribute: instanceTransform
} ShaderLocationIndex;


// Shader location point type
typedef enum {
    PL_SHADER_LOC_VERTEX_POSITION = 0,  // Shader location: vertex attribute: position
    PL_SHADER_LOC_VERTEX_TEXCOORD01,    // Shader location: vertex attribute: texcoord01
    PL_SHADER_LOC_VERTEX_TEXCOORD02,    // Shader location: vertex attribute: texcoord02
    PL_SHADER_LOC_VERTEX_NORMAL,        // Shader location: vertex attribute: normal
    PL_SHADER_LOC_VERTEX_TANGENT,       // Shader location: vertex attribute: tangent
    PL_SHADER_LOC_VERTEX_COLOR,         // Shader location: vertex attribute: color
    PL_SHADER_LOC_MATRIX_MVP,           // Shader location: matrix uniform: model-view-projection
    PL_SHADER_LOC_MATRIX_VIEW,          // Shader location: matrix uniform: view (camera transform)
    PL_SHADER_LOC_MATRIX_PROJECTION,    // Shader location: matrix uniform: projection
    PL_SHADER_LOC_MATRIX_MODEL,         // Shader location: matrix uniform: model (transform)
    PL_SHADER_LOC_MATRIX_NORMAL,        // Shader location: matrix uniform: normal
    PL_SHADER_LOC_VECTOR_VIEW,          // Shader location: vector uniform: view
    PL_SHADER_LOC_COLOR_DIFFUSE,        // Shader location: vector uniform: diffuse color
    PL_SHADER_LOC_COLOR_SPECULAR,       // Shader location: vector uniform: specular color
    PL_SHADER_LOC_COLOR_AMBIENT,        // Shader location: vector uniform: ambient color
    PL_SHADER_LOC_MAP_ALBEDO,           // Shader location: sampler2d texture: albedo (same as: PL_SHADER_LOC_MAP_DIFFUSE)
    PL_SHADER_LOC_MAP_METALNESS,        // Shader location: sampler2d texture: metalness (same as: PL_SHADER_LOC_MAP_SPECULAR)
    PL_SHADER_LOC_MAP_NORMAL,           // Shader location: sampler2d texture: normal
    PL_SHADER_LOC_MAP_ROUGHNESS,        // Shader location: sampler2d texture: roughness
    PL_SHADER_LOC_MAP_OCCLUSION,        // Shader location: sampler2d texture: occlusion
    PL_SHADER_LOC_MAP_EMISSION,         // Shader location: sampler2d texture: emission
    PL_SHADER_LOC_MAP_HEIGHT,           // Shader location: sampler2d texture: height
    PL_SHADER_LOC_MAP_CUBEMAP,          // Shader location: samplerCube texture: cubemap
    PL_SHADER_LOC_MAP_IRRADIANCE,       // Shader location: samplerCube texture: irradiance
    PL_SHADER_LOC_MAP_PREFILTER,        // Shader location: samplerCube texture: prefilter
    PL_SHADER_LOC_MAP_BRDF              // Shader location: sampler2d texture: brdf
} plShaderLocationIndex;




#define SHADER_LOC_MAP_DIFFUSE      SHADER_LOC_MAP_ALBEDO
#define SHADER_LOC_MAP_SPECULAR     SHADER_LOC_MAP_METALNESS

#define PL_SHADER_LOC_MAP_DIFFUSE       PL_SHADER_LOC_MAP_ALBEDO
#define PL_SHADER_LOC_MAP_SPECULAR      PL_SHADER_LOC_MAP_METALNESS

// Pixel formats
// NOTE: Support depends on OpenGL version and platform
typedef enum {
    PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1, // 8 bit per pixel (no alpha)
    PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,    // 8*2 bpp (2 channels)
    PIXELFORMAT_UNCOMPRESSED_R5G6B5,        // 16 bpp
    PIXELFORMAT_UNCOMPRESSED_R8G8B8,        // 24 bpp
    PIXELFORMAT_UNCOMPRESSED_R5G5B5A1,      // 16 bpp (1 bit alpha)
    PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,      // 16 bpp (4 bit alpha)
    PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,      // 32 bpp
    PIXELFORMAT_UNCOMPRESSED_R32,           // 32 bpp (1 channel - float)
    PIXELFORMAT_UNCOMPRESSED_R32G32B32,     // 32*3 bpp (3 channels - float)
    PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,  // 32*4 bpp (4 channels - float)
    PIXELFORMAT_UNCOMPRESSED_R16,           // 16 bpp (1 channel - half float)
    PIXELFORMAT_UNCOMPRESSED_R16G16B16,     // 16*3 bpp (3 channels - half float)
    PIXELFORMAT_UNCOMPRESSED_R16G16B16A16,  // 16*4 bpp (4 channels - half float)
    PIXELFORMAT_COMPRESSED_DXT1_RGB,        // 4 bpp (no alpha)
    PIXELFORMAT_COMPRESSED_DXT1_RGBA,       // 4 bpp (1 bit alpha)
    PIXELFORMAT_COMPRESSED_DXT3_RGBA,       // 8 bpp
    PIXELFORMAT_COMPRESSED_DXT5_RGBA,       // 8 bpp
    PIXELFORMAT_COMPRESSED_ETC1_RGB,        // 4 bpp
    PIXELFORMAT_COMPRESSED_ETC2_RGB,        // 4 bpp
    PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA,   // 8 bpp
    PIXELFORMAT_COMPRESSED_PVRT_RGB,        // 4 bpp
    PIXELFORMAT_COMPRESSED_PVRT_RGBA,       // 4 bpp
    PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,   // 8 bpp
    PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA    // 2 bpp
} PixelFormat;

typedef enum {
    PL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1,     // 8 bit per pixel (no alpha)
    PL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,        // 8*2 bpp (2 channels)
    PL_PIXELFORMAT_UNCOMPRESSED_R5G6B5,            // 16 bpp
    PL_PIXELFORMAT_UNCOMPRESSED_R8G8B8,            // 24 bpp
    PL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1,          // 16 bpp (1 bit alpha)
    PL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,          // 16 bpp (4 bit alpha)
    PL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,          // 32 bpp
    PL_PIXELFORMAT_UNCOMPRESSED_R32,               // 32 bpp (1 channel - float)
    PL_PIXELFORMAT_UNCOMPRESSED_R32G32B32,         // 32*3 bpp (3 channels - float)
    PL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,      // 32*4 bpp (4 channels - float)
    PL_PIXELFORMAT_UNCOMPRESSED_R16,               // 16 bpp (1 channel - half float)
    PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16,         // 16*3 bpp (3 channels - half float)
    PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16,      // 16*4 bpp (4 channels - half float)
    PL_PIXELFORMAT_COMPRESSED_DXT1_RGB,            // 4 bpp (no alpha)
    PL_PIXELFORMAT_COMPRESSED_DXT1_RGBA,           // 4 bpp (1 bit alpha)
    PL_PIXELFORMAT_COMPRESSED_DXT3_RGBA,           // 8 bpp
    PL_PIXELFORMAT_COMPRESSED_DXT5_RGBA,           // 8 bpp
    PL_PIXELFORMAT_COMPRESSED_ETC1_RGB,            // 4 bpp
    PL_PIXELFORMAT_COMPRESSED_ETC2_RGB,            // 4 bpp
    PL_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA,       // 8 bpp
    PL_PIXELFORMAT_COMPRESSED_PVRT_RGB,            // 4 bpp
    PL_PIXELFORMAT_COMPRESSED_PVRT_RGBA,           // 4 bpp
    PL_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,       // 8 bpp
    PL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA        // 2 bpp
} glPixelFormat;

// Shader attribute data types
typedef enum {
    PL_SHADER_ATTRIB_FLOAT = 0,         // Shader attribute type: float
    PL_SHADER_ATTRIB_VEC2,              // Shader attribute type: vec2 (2 float)
    PL_SHADER_ATTRIB_VEC3,              // Shader attribute type: vec3 (3 float)
    PL_SHADER_ATTRIB_VEC4               // Shader attribute type: vec4 (4 float)
} plShaderAttributeDataType;
// Shader attribute data types
typedef enum {
    SHADER_ATTRIB_FLOAT = 0,        // Shader attribute type: float
    SHADER_ATTRIB_VEC2,             // Shader attribute type: vec2 (2 float)
    SHADER_ATTRIB_VEC3,             // Shader attribute type: vec3 (3 float)
    SHADER_ATTRIB_VEC4              // Shader attribute type: vec4 (4 float)
} ShaderAttributeDataType;

const char* glGetPixelFormatName(unsigned int format);

