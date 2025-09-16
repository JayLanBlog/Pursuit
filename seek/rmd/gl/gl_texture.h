#pragma once
#include <section/graph/material_def.h>
#include <string>
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <file/image.h>
#include <section/martrix/cg_def.h>

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


//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#ifndef PIXELFORMAT_UNCOMPRESSED_R5G5B5A1_ALPHA_THRESHOLD
#define PIXELFORMAT_UNCOMPRESSED_R5G5B5A1_ALPHA_THRESHOLD  50    // Threshold over 255 to set alpha as 0
#endif


using namespace Seek;

namespace DRAW {

	namespace GL {
		// Generate image: checked
		Image GenImageChecked(int width, int height, int checksX, int checksY, Color col1, Color col2);

		// Get color normalized as float [0..1]
		Vector4 ColorNormalize(Color color);
		unsigned int GetTextureIdDefault(void);

		// Load a texture from image data
		// NOTE: image is not unloaded, it must be done manually
		Texture2D LoadTextureFromImage(Image image);

		Texture2D LoadTexture(const char* fileName);

		Texture2D LoadCubeTexture(std::vector<Image> datas, int mipmapLevel);

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

		unsigned int LoadTextureCubemap(std::vector<Image> datas, int mipmapLevel);

		unsigned int LoadTextureCubemap(const void* data, int size, int format, int mipmapCount);
		// Create an image from another image piece
		Image ImageFromImage(Image image, Rectangle rec);
		//void rlLoadExtensions(void* loader);
		void GetGlTextureFormats(int format, unsigned int* glInternalFormat, unsigned int* glFormat, unsigned int* glType);

		int GetPixelDataSize(int width, int height, int format);

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

		// Get color with alpha applied, alpha goes from 0.0f to 1.0f
		Color ColorAlpha(Color color, float alpha);

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


		Color* LoadImageColors(Image image);

		Color Fade(Color color, float alpha);

		Color GetColor(unsigned int hexValue);

		void SetTextureFilter(Texture2D texture, int filter);

		// Set pixel color formatted into destination pointer
		void SetPixelColor(void* dstPtr, Color color, int format);

		void ImageResizeCanvas(Image* image, int newWidth, int newHeight, int offsetX, int offsetY, Color fill);

		//------------------------------------------------------------------------------------
		// Image generation functions
		//------------------------------------------------------------------------------------
		// Generate image: plain color
		Image GenImageColor(int width, int height, Color color);

		// Unload color data loaded with LoadImageColors()
		void UnloadImageColors(Color* colors);

		// Get src alpha-blended into dst color with tint
		Color ColorAlphaBlend(Color dst, Color src, Color tint);

		// Convert image data to desired format
		void ImageFormat(Image* image, int newFormat);

		// Get color from a pixel from certain format
		Color GetPixelColor(void* srcPtr, int format);

		Vector4* LoadImageDataNormalized(Image image);

		unsigned short FloatToHalf(float x);
		// Resize and image to new size
		// NOTE: Uses stb default scaling filters (both bicubic):
		// STBIR_DEFAULT_FILTER_UPSAMPLE    STBIR_FILTER_CATMULLROM
		// STBIR_DEFAULT_FILTER_DOWNSAMPLE  STBIR_FILTER_MITCHELL   (high-quality Catmull-Rom)
		void ImageResize(Image* image, int newWidth, int newHeight);
	}

}