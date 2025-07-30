#pragma once
#include <section/graph/material_def.h>
#include <string>
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <file/image.h>

// Texture parameters (equivalent to OpenGL defines)
#define PL_TEXTURE_WRAP_S                       0x2802      // GL_TEXTURE_WRAP_S
#define PL_TEXTURE_WRAP_T                       0x2803      // GL_TEXTURE_WRAP_T
#define PL_TEXTURE_MAG_FILTER                   0x2800      // GL_TEXTURE_MAG_FILTER
#define PL_TEXTURE_MIN_FILTER                   0x2801      // GL_TEXTURE_MIN_FILTER

#define PL_TEXTURE_FILTER_NEAREST               0x2600      // GL_NEAREST
#define PL_TEXTURE_FILTER_LINEAR                0x2601      // GL_LINEAR
#define PL_TEXTURE_FILTER_MIP_NEAREST           0x2700      // GL_NEAREST_MIPMAP_NEAREST
#define PL_TEXTURE_FILTER_NEAREST_MIP_LINEAR    0x2702      // GL_NEAREST_MIPMAP_LINEAR
#define PL_TEXTURE_FILTER_LINEAR_MIP_NEAREST    0x2701      // GL_LINEAR_MIPMAP_NEAREST
#define PL_TEXTURE_FILTER_MIP_LINEAR            0x2703      // GL_LINEAR_MIPMAP_LINEAR
#define PL_TEXTURE_FILTER_ANISOTROPIC           0x3000      // Anisotropic filter (custom identifier)
#define PL_TEXTURE_MIPMAP_BIAS_RATIO            0x4000      // Texture mipmap bias, percentage ratio (custom identifier)

#define PL_TEXTURE_WRAP_REPEAT                  0x2901      // GL_REPEAT
#define PL_TEXTURE_WRAP_CLAMP                   0x812F      // GL_CLAMP_TO_EDGE
#define PL_TEXTURE_WRAP_MIRROR_REPEAT           0x8370      // GL_MIRRORED_REPEAT
#define PL_TEXTURE_WRAP_MIRROR_CLAMP            0x8742      // GL_MIRROR_CLAMP_EXT

using namespace Seek;

namespace DRAW {

	namespace GL {

		unsigned int GetTextureIdDefault(void);

		// Load a texture from image data
		// NOTE: image is not unloaded, it must be done manually
		Texture2D LoadTextureFromImage(Image image);

		Texture2D LoadTexture(const char* fileName);

		void UnloadTexture(unsigned int id);
		// Generate mipmap data for selected texture
		// NOTE: Only supports GPU mipmap generation
		void GenTextureMipmaps(unsigned int id, int width, int height, int format, int* mipmaps);
		// Read texture pixel data
		void* ReadTexturePixels(unsigned int id, int width, int height, int format);
		// Read screen pixel data (color buffer)
		unsigned char* ReadScreenPixels(int width, int height);

		unsigned int LoadTextureDepth(int width, int height, bool useRenderBuffer);

		void UpdateTexture(unsigned int id, int offsetX, int offsetY, int width, int height, int format, const void* data);

		unsigned int LoadTextureCubemap(const void* data, int size, int format, int mipmapCount);
		// Create an image from another image piece
		Image ImageFromImage(Image image, Rectangle rec);
		//void rlLoadExtensions(void* loader);
		void GetGlTextureFormats(int format, unsigned int* glInternalFormat, unsigned int* glFormat, unsigned int* glType);

		static int GetPixelDataSize(int width, int height, int format);

		unsigned int LoadTexture(const void* data, int width, int height, int format, int mipmapCount);

		void BindImageTexture(unsigned int id, unsigned int index, int format, bool readonly);

		float HalfToFloat(unsigned short x);
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		// Get image pixel color at (x, y) position
		Color GetImageColor(Image image, int x, int y);
	
		// Define one vertex (color)
		void Color4ub(unsigned char x, unsigned char y, unsigned char z, unsigned char w);

		// Define one vertex (color)
		void Color4f(float r, float g, float b, float a);

		// Define one vertex (color)
		void Color3f(float x, float y, float z);
#endif
		// Set current texture to use
		void SetTexture(unsigned int id);

		// Select and active a texture slot
		void ActiveTextureSlot(int slot);

		// Enable texture
		void EnableTexture(unsigned int id);

		// Disable texture
		void DisableTexture(void);

		// Enable texture cubemap
		void EnableTextureCubemap(unsigned int id);

		// Disable texture cubemap
		void DisableTextureCubemap(void);

		// Set texture parameters (wrap mode/filter mode)
		void TextureParameters(unsigned int id, int param, int value);

		// Set cubemap parameters (wrap mode/filter mode)
		void CubemapParameters(unsigned int id, int param, int value);
	}

}