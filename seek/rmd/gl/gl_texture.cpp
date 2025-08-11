#include "gl_global.h"
#include "gl_texture.h"
#include <helper/mem_help.h>
#include <section/enum/material_em.h>
#include <logger/logger.h>
#include <file/image.h>
using namespace System;
namespace DRAW {
	namespace GL {

		// Get color with alpha applied, alpha goes from 0.0f to 1.0f
		Color ColorAlpha(Color color, float alpha) {
			Color result = color;

			if (alpha < 0.0f) alpha = 0.0f;
			else if (alpha > 1.0f) alpha = 1.0f;

			result.a = (unsigned char)(255.0f * alpha);

			return result;
		}

		// Get color normalized as float [0..1]
		Vector4 ColorNormalize(Color color) {
			Vector4 result;

			result.x = (float)color.r / 255.0f;
			result.y = (float)color.g / 255.0f;
			result.z = (float)color.b / 255.0f;
			result.w = (float)color.a / 255.0f;

			return result;
		}
		// Load a texture from image data
		// NOTE: image is not unloaded, it must be done manually
		Texture2D LoadTextureFromImage(Image image) {
			Texture2D texture = { 0 };

			if ((image.width != 0) && (image.height != 0))
			{
				texture.id = LoadTexture(image.data, image.width, image.height, image.format, image.mipmaps);
			}
			else TRACELOG(LOG_WARNING, "IMAGE: Data is not valid to load texture");

			texture.width = image.width;
			texture.height = image.height;
			texture.mipmaps = image.mipmaps;
			texture.format = image.format;
			return texture;
		}

		Texture2D LoadTexture(const char* fileName) {
			Texture2D texture = { 0 };

			Image image = LoadImageSource(fileName);

			if (image.data != NULL)
			{
				texture = LoadTextureFromImage(image);
				UnloadImage(image);
			}

			return texture;
		}

		unsigned int GetTextureIdDefault(void) {
			unsigned int id = 0;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			id = PLGL.State.defaultTextureId;
#endif
			return id;
		}
		void UnloadTexture(unsigned int id) {
			glDeleteTextures(1, &id);
		}
		// Generate mipmap data for selected texture
		// NOTE: Only supports GPU mipmap generation
		void GenTextureMipmaps(unsigned int id, int width, int height, int format, int* mipmaps) {
			glBindTexture(GL_TEXTURE_2D, id);
			// Check if texture is power-of-two (POT)
			bool texIsPOT = false;
			if (((width > 0) && ((width & (width - 1)) == 0)) &&
				((height > 0) && ((height & (height - 1)) == 0))) texIsPOT = true;
			if ((texIsPOT) || (PLGL.ExtSupported.texNPOT))
			{
				//glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);   // Hint for mipmaps generation algorithm: GL_FASTEST, GL_NICEST, GL_DONT_CARE
				glGenerateMipmap(GL_TEXTURE_2D);    // Generate mipmaps automatically
#define MIN(a,b) (((a)<(b))? (a):(b))
#define MAX(a,b) (((a)>(b))? (a):(b))
				* mipmaps = 1 + (int)floor(log(MAX(width, height)) / log(2));
				TRACELOG(LOG_INFO, "TEXTURE: [ID %i] Mipmaps generated automatically, total: %i", id, *mipmaps);
			}
			else TRACELOG(LOG_INFO, "TEXTURE: [ID %i] Failed to generate mipmaps", id);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Read texture pixel data
		void* ReadTexturePixels(unsigned int id, int width, int height, int format) {
			void* pixels = NULL;
			glBindTexture(GL_TEXTURE_2D, id);
			// NOTE: Using texture id, we can retrieve some texture info (but not on OpenGL ES 2.0)
			 // Possible texture info: GL_TEXTURE_RED_SIZE, GL_TEXTURE_GREEN_SIZE, GL_TEXTURE_BLUE_SIZE, GL_TEXTURE_ALPHA_SIZE
			 //int width, height, format;
			 //glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
			 //glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
			 //glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);

			 // NOTE: Each row written to or read from by OpenGL pixel operations like glGetTexImage are aligned to a 4 byte boundary by default, which may add some padding
			 // Use glPixelStorei to modify padding with the GL_[UN]PACK_ALIGNMENT setting
			 // GL_PACK_ALIGNMENT affects operations that read from OpenGL memory (glReadPixels, glGetTexImage, etc.)
			 // GL_UNPACK_ALIGNMENT affects operations that write to OpenGL memory (glTexImage, etc.)
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			unsigned int glInternalFormat, glFormat, glType;
			GetGlTextureFormats(format, &glInternalFormat, &glFormat, &glType);
			unsigned int size = GetPixelDataSize(width, height, format);

			if ((glInternalFormat != 0) && (format < PL_PIXELFORMAT_COMPRESSED_DXT1_RGB))
			{
				pixels = MALLOC(size);
				glGetTexImage(GL_TEXTURE_2D, 0, glFormat, glType, pixels);
			}
			else TRACELOG(LOG_WARNING, "TEXTURE: [ID %i] Data retrieval not suported for pixel format (%i)", id, format);

			glBindTexture(GL_TEXTURE_2D, 0);
			return pixels;
		}

		// Read screen pixel data (color buffer)
		unsigned char* ReadScreenPixels(int width, int height) {
			unsigned char* imgData = (unsigned char*)CALLOC(width * height * 4, sizeof(unsigned char));
			// NOTE 1: glReadPixels returns image flipped vertically -> (0,0) is the bottom left corner of the framebuffer
			// NOTE 2: We are getting alpha channel! Be careful, it can be transparent if not cleared properly!
			glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
			// Flip image vertically!
	// NOTE: Alpha value has already been applied to RGB in framebuffer, we don't need it!
			for (int y = height - 1; y >= height / 2; y--)
			{
				for (int x = 0; x < (width * 4); x += 4)
				{
					unsigned int s = ((height - 1) - y) * width * 4 + x;
					unsigned int e = y * width * 4 + x;

					unsigned char r = imgData[s];
					unsigned char g = imgData[s + 1];
					unsigned char b = imgData[s + 2];

					imgData[s] = imgData[e];
					imgData[s + 1] = imgData[e + 1];
					imgData[s + 2] = imgData[e + 2];
					imgData[s + 3] = 255; // Set alpha component value to 255 (no trasparent image retrieval)

					imgData[e] = r;
					imgData[e + 1] = g;
					imgData[e + 2] = b;
					imgData[e + 3] = 255; // Ditto
				}
			}
			return imgData;     // NOTE: image data should be freed
		}


		unsigned int LoadTextureDepth(int width, int height, bool useRenderBuffer) {
			unsigned int id = 0;
			// In case depth textures not supported, we force renderbuffer usage
			if (!PLGL.ExtSupported.texDepth) useRenderBuffer = true;
			// NOTE: We let the implementation to choose the best bit-depth
			// Possible formats: GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32 and GL_DEPTH_COMPONENT32F
			unsigned int glInternalFormat = GL_DEPTH_COMPONENT;
			if (!useRenderBuffer && PLGL.ExtSupported.texDepth) {
				glGenTextures(1, &id);
				glBindTexture(GL_TEXTURE_2D, id);
				glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glBindTexture(GL_TEXTURE_2D, 0);

				TRACELOG(LOG_INFO, "TEXTURE: Depth texture loaded successfully");
			}
			else {
				// Create the renderbuffer that will serve as the depth attachment for the framebuffer
				// NOTE: A renderbuffer is simpler than a texture and could offer better performance on embedded devices
				glGenRenderbuffers(1, &id);
				glBindRenderbuffer(GL_RENDERBUFFER, id);
				glRenderbufferStorage(GL_RENDERBUFFER, glInternalFormat, width, height);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
				TRACELOG(LOG_INFO, "TEXTURE: [ID %i] Depth renderbuffer loaded successfully (%i bits)", id, (PLGL.ExtSupported.maxDepthBits >= 24) ? PLGL.ExtSupported.maxDepthBits : 16);

			}
			return id;
		}

		void UpdateTexture(unsigned int id, int offsetX, int offsetY, int width, int height, int format, const void* data) {
			glBindTexture(GL_TEXTURE_2D, id);
			unsigned int glInternalFormat, glFormat, glType;
			GetGlTextureFormats(format, &glInternalFormat, &glFormat, &glType);

			if ((glInternalFormat != 0) && (format < PL_PIXELFORMAT_COMPRESSED_DXT1_RGB))
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, width, height, glFormat, glType, data);
			}
			else TRACELOG(LOG_WARNING, "TEXTURE: [ID %i] Failed to update for current texture format (%i)", id, format);
		}


		unsigned int LoadTextureCubemap(const void* data, int size, int format, int mipmapCount) {
			unsigned int id = 0;
			int mipSize = size;
			// NOTE: Added pointer math separately from function to avoid UBSAN complaining
			unsigned char* dataPtr = NULL;
			if (data != NULL) dataPtr = (unsigned char*)data;
			unsigned int dataSize = GetPixelDataSize(size, size, format);
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_CUBE_MAP, id);
			unsigned int glInternalFormat, glFormat, glType;
			GetGlTextureFormats(format, &glInternalFormat, &glFormat, &glType);
			if (glInternalFormat != 0)
			{
				for (int i = 0; i < 6 * mipmapCount; i++) {
					int mipmapLevel = i / 6;
					int face = i % 6;
					if (data == NULL) {
						if (format < PL_PIXELFORMAT_COMPRESSED_DXT1_RGB)
						{
							if ((format == PL_PIXELFORMAT_UNCOMPRESSED_R32) ||
								(format == PL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32) ||
								(format == PL_PIXELFORMAT_UNCOMPRESSED_R16) ||
								(format == PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16)) TRACELOG(LOG_WARNING, "TEXTURES: Cubemap requested format not supported");
							else glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mipmapLevel, glInternalFormat, mipSize, mipSize, 0, glFormat, glType, NULL);
						}
						else TRACELOG(LOG_WARNING, "TEXTURES: Empty cubemap creation does not support compressed format");
					}
					else
					{
						if (format < PL_PIXELFORMAT_COMPRESSED_DXT1_RGB) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mipmapLevel, glInternalFormat, mipSize, mipSize, 0, glFormat, glType, (unsigned char*)dataPtr + face * dataSize);
						else glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mipmapLevel, glInternalFormat, mipSize, mipSize, 0, dataSize, (unsigned char*)dataPtr + face * dataSize);
					}

					if (format == PL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE)
					{
						GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
						glTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
					}
					else if (format == PL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA) {
						GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
						glTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
					}
					if (face == 5)
					{
						mipSize /= 2;
						if (data != NULL) dataPtr += dataSize * 6; // Increment data pointer to next mipmap

						// Security check for NPOT textures
						if (mipSize < 1) mipSize = 1;

						dataSize = GetPixelDataSize(mipSize, mipSize, format);
					}
				}
			}
			// Set cubemap texture sampling parameters
			if (mipmapCount > 1) glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			else glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  // Flag not supported on OpenGL ES 2.0
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			if (id > 0) TRACELOG(LOG_INFO, "TEXTURE: [ID %i] Cubemap texture loaded successfully (%ix%i)", id, size, size);
			else TRACELOG(LOG_WARNING, "TEXTURE: Failed to load cubemap texture");
			return id;
		}

		// Create an image from another image piece
		Image ImageFromImage(Image image, Rectangle rec) {
			Image result = { 0 };

			int bytesPerPixel = GetPixelDataSize(1, 1, image.format);

			result.width = (int)rec.width;
			result.height = (int)rec.height;
			result.data = CALLOC((int)rec.width * (int)rec.height * bytesPerPixel, 1);
			result.format = image.format;
			result.mipmaps = 1;

			for (int y = 0; y < (int)rec.height; y++)
			{
				memcpy(((unsigned char*)result.data) + y * (int)rec.width * bytesPerPixel, ((unsigned char*)image.data) + ((y + (int)rec.y) * image.width + (int)rec.x) * bytesPerPixel, (int)rec.width * bytesPerPixel);
			}

			return result;
		}

		static int GetPixelDataSize(int width, int height, int format) {
			int dataSize = 0;       // Size in bytes
			int bpp = 0;            // Bits per pixel

			switch (format)
			{
			case PL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: bpp = 8; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
			case PL_PIXELFORMAT_UNCOMPRESSED_R5G6B5:
			case PL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
			case PL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: bpp = 16; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: bpp = 32; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R8G8B8: bpp = 24; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R32: bpp = 32; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R32G32B32: bpp = 32 * 3; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: bpp = 32 * 4; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R16: bpp = 16; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16: bpp = 16 * 3; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16: bpp = 16 * 4; break;
			case PL_PIXELFORMAT_COMPRESSED_DXT1_RGB:
			case PL_PIXELFORMAT_COMPRESSED_DXT1_RGBA:
			case PL_PIXELFORMAT_COMPRESSED_ETC1_RGB:
			case PL_PIXELFORMAT_COMPRESSED_ETC2_RGB:
			case PL_PIXELFORMAT_COMPRESSED_PVRT_RGB:
			case PL_PIXELFORMAT_COMPRESSED_PVRT_RGBA: bpp = 4; break;
			case PL_PIXELFORMAT_COMPRESSED_DXT3_RGBA:
			case PL_PIXELFORMAT_COMPRESSED_DXT5_RGBA:
			case PL_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA:
			case PL_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA: bpp = 8; break;
			case PL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA: bpp = 2; break;
			default: break;
			}
			double bytesPerPixel = (double)bpp / 8.0;
			dataSize = (int)(bytesPerPixel * width * height); // Total data size in bytes
			// Most compressed formats works on 4x4 blocks,
			// if texture is smaller, minimum dataSize is 8 or 16
			if ((width < 4) && (height < 4))
			{
				if ((format >= PL_PIXELFORMAT_COMPRESSED_DXT1_RGB) && (format < PL_PIXELFORMAT_COMPRESSED_DXT3_RGBA)) dataSize = 8;
				else if ((format >= PL_PIXELFORMAT_COMPRESSED_DXT3_RGBA) && (format < PL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA)) dataSize = 16;
			}

			return dataSize;
		}

		void GetGlTextureFormats(int format, unsigned int* glInternalFormat, unsigned int* glFormat, unsigned int* glType) {
			*glInternalFormat = 0;
			*glFormat = 0;
			*glType = 0;
			switch (format) {
			case PL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: *glInternalFormat = GL_R8; *glFormat = GL_RED; *glType = GL_UNSIGNED_BYTE; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: *glInternalFormat = GL_RG8; *glFormat = GL_RG; *glType = GL_UNSIGNED_BYTE; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R5G6B5: *glInternalFormat = GL_RGB; *glFormat = GL_RGB; *glType = GL_UNSIGNED_SHORT_5_6_5; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R8G8B8: *glInternalFormat = GL_RGB8; *glFormat = GL_RGB; *glType = GL_UNSIGNED_BYTE; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1: *glInternalFormat = GL_RGB5_A1; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_5_5_5_1; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: *glInternalFormat = GL_RGBA4; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_4_4_4_4; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: *glInternalFormat = GL_RGBA8; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_BYTE; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R32: if (PLGL.ExtSupported.texFloat32) *glInternalFormat = GL_R32F; *glFormat = GL_RED; *glType = GL_FLOAT; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R32G32B32: if (PLGL.ExtSupported.texFloat32) *glInternalFormat = GL_RGB32F; *glFormat = GL_RGB; *glType = GL_FLOAT; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: if (PLGL.ExtSupported.texFloat32) *glInternalFormat = GL_RGBA32F; *glFormat = GL_RGBA; *glType = GL_FLOAT; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R16: if (PLGL.ExtSupported.texFloat16) *glInternalFormat = GL_R16F; *glFormat = GL_RED; *glType = GL_HALF_FLOAT; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16: if (PLGL.ExtSupported.texFloat16) *glInternalFormat = GL_RGB16F; *glFormat = GL_RGB; *glType = GL_HALF_FLOAT; break;
			case PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16: if (PLGL.ExtSupported.texFloat16) *glInternalFormat = GL_RGBA16F; *glFormat = GL_RGBA; *glType = GL_HALF_FLOAT; break;
			default: TRACELOG(LOG_WARNING, "TEXTURE: Current format not supported (%i)", format); break;
			}
		}

		void BindImageTexture(unsigned int id, unsigned int index, int format, bool readonly) {
#if defined(GRAPHICS_API_OPENGL_43)
			unsigned int glInternalFormat = 0, glFormat = 0, glType = 0;

			rlGetGlTextureFormats(format, &glInternalFormat, &glFormat, &glType);
			glBindImageTexture(index, id, 0, 0, 0, readonly ? GL_READ_ONLY : GL_READ_WRITE, glInternalFormat);
#else
			TRACELOG(LOG_WARNING, "TEXTURE: Image texture binding not enabled. Define GRAPHICS_API_OPENGL_43");
#endif
		}

		unsigned int LoadTexture(const void* data, int width, int height, int format, int mipmapCount) {
			unsigned int id = 0;

			glBindTexture(GL_TEXTURE_2D, 0);    // Free any old binding

				// Check texture format support by OpenGL 1.1 (compressed textures not supported)
#if defined(GRAPHICS_API_OPENGL_11)
			if (format >= PL_PIXELFORMAT_COMPRESSED_DXT1_RGB)
			{
				TRACELOG(PL_LOG_WARNING, "GL: OpenGL 1.1 does not support GPU compressed texture formats");
				return id;
			}
#else
			if ((!PLGL.ExtSupported.texCompDXT) && ((format == PL_PIXELFORMAT_COMPRESSED_DXT1_RGB) || (format == PL_PIXELFORMAT_COMPRESSED_DXT1_RGBA) ||
				(format == PL_PIXELFORMAT_COMPRESSED_DXT3_RGBA) || (format == PL_PIXELFORMAT_COMPRESSED_DXT5_RGBA)))
			{
				TRACELOG(LOG_INFO, "GL: DXT compressed texture format not supported");
				return id;
			}

			if ((!PLGL.ExtSupported.texCompETC1) && (format == PL_PIXELFORMAT_COMPRESSED_ETC1_RGB))
			{
				TRACELOG(LOG_WARNING, "GL: ETC1 compressed texture format not supported");
				return id;
			}

			if ((!PLGL.ExtSupported.texCompETC2) && ((format == PL_PIXELFORMAT_COMPRESSED_ETC2_RGB) || (format == PL_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA)))
			{
				TRACELOG(LOG_WARNING, "GL: ETC2 compressed texture format not supported");
				return id;
			}

			if ((!PLGL.ExtSupported.texCompPVRT) && ((format == PL_PIXELFORMAT_COMPRESSED_PVRT_RGB) || (format == PL_PIXELFORMAT_COMPRESSED_PVRT_RGBA)))
			{
				TRACELOG(LOG_WARNING, "GL: PVRT compressed texture format not supported");
				return id;
			}

			if ((!PLGL.ExtSupported.texCompASTC) && ((format == PL_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA) || (format == PL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA)))
			{
				TRACELOG(LOG_WARNING, "GL: ASTC compressed texture format not supported");
				return id;
			}
			//rlLoadExtensions
			//void rlLoadExtensions(void* loader)

#endif  // GRAPHICS_API_OPENGL_11
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glGenTextures(1, &id);              // Generate texture id
			glBindTexture(GL_TEXTURE_2D, id);
			int mipWidth = width;
			int mipHeight = height;
			int mipOffset = 0;

			// NOTE: Added pointer math separately from function to avoid UBSAN complaining
			unsigned char* dataPtr = NULL;
			if (data != NULL) dataPtr = (unsigned char*)data;
			// Load the different mipmap levels
			for (int i = 0; i < mipmapCount; i++) {
				unsigned int mipSize = GetPixelDataSize(mipWidth, mipHeight, format);
				unsigned int glInternalFormat, glFormat, glType;
				GetGlTextureFormats(format, &glInternalFormat, &glFormat, &glType);
				TRACELOGD("TEXTURE: Load mipmap level %i (%i x %i), size: %i, offset: %i", i, mipWidth, mipHeight, mipSize, mipOffset);
				if (glInternalFormat != 0) {
					if (format < PL_PIXELFORMAT_COMPRESSED_DXT1_RGB) glTexImage2D(GL_TEXTURE_2D, i, glInternalFormat, mipWidth, mipHeight, 0, glFormat, glType, dataPtr);
					if (format == PL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE)
					{
						GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
						glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
					}
					else if (format == PL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA) {
						GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
						glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
					}
				}
				mipWidth /= 2;
				mipHeight /= 2;
				mipOffset += mipSize;
				if (data != NULL) dataPtr += mipSize;         // Increment data pointer to next mipmap
				// Security check for NPOT textures
				if (mipWidth < 1) mipWidth = 1;
				if (mipHeight < 1) mipHeight = 1;
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
			 // Magnification and minification filters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR

			if (mipmapCount > 1)
			{
				// Activate Trilinear filtering if mipmaps are available
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapCount); // Required for user-defined mip count
			}
			// Unbind current texture
			glBindTexture(GL_TEXTURE_2D, 0);
			if (id > 0) TRACELOG(LOG_WARNING, "TEXTURE: [ID %i] Texture loaded successfully (%ix%i | %s | %i mipmaps)", id, width, height, glGetPixelFormatName(format), mipmapCount);
			else TRACELOG(LOG_WARNING, "TEXTURE: Failed to load texture");
			return id;
		}
		float HalfToFloat(unsigned short x) {
			float result = 0.0f;

			union {
				float fm;
				unsigned int ui;
			} uni;

			const unsigned int e = (x & 0x7c00) >> 10; // Exponent
			const unsigned int m = (x & 0x03ff) << 13; // Mantissa
			uni.fm = (float)m;
			const unsigned int v = uni.ui >> 23; // Evil log2 bit hack to count leading zeros in denormalized format
			uni.ui = (x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007fe000)); // sign : normalized : denormalized

			result = uni.fm;

			return result;
		}

		// Get image pixel color at (x, y) position
		Color GetImageColor(Image image, int x, int y) {
			Color color = { 0 };

			if ((x >= 0) && (x < image.width) && (y >= 0) && (y < image.height))
			{
				switch (image.format)
				{
				case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
				{
					color.r = ((unsigned char*)image.data)[y * image.width + x];
					color.g = ((unsigned char*)image.data)[y * image.width + x];
					color.b = ((unsigned char*)image.data)[y * image.width + x];
					color.a = 255;

				} break;
				case PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
				{
					color.r = ((unsigned char*)image.data)[(y * image.width + x) * 2];
					color.g = ((unsigned char*)image.data)[(y * image.width + x) * 2];
					color.b = ((unsigned char*)image.data)[(y * image.width + x) * 2];
					color.a = ((unsigned char*)image.data)[(y * image.width + x) * 2 + 1];

				} break;
				case PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
				{
					unsigned short pixel = ((unsigned short*)image.data)[y * image.width + x];

					color.r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11) * (255 / 31));
					color.g = (unsigned char)((float)((pixel & 0b0000011111000000) >> 6) * (255 / 31));
					color.b = (unsigned char)((float)((pixel & 0b0000000000111110) >> 1) * (255 / 31));
					color.a = (unsigned char)((pixel & 0b0000000000000001) * 255);

				} break;
				case PIXELFORMAT_UNCOMPRESSED_R5G6B5:
				{
					unsigned short pixel = ((unsigned short*)image.data)[y * image.width + x];

					color.r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11) * (255 / 31));
					color.g = (unsigned char)((float)((pixel & 0b0000011111100000) >> 5) * (255 / 63));
					color.b = (unsigned char)((float)(pixel & 0b0000000000011111) * (255 / 31));
					color.a = 255;

				} break;
				case PIXELFORMAT_UNCOMPRESSED_R4G4B4A4:
				{
					unsigned short pixel = ((unsigned short*)image.data)[y * image.width + x];

					color.r = (unsigned char)((float)((pixel & 0b1111000000000000) >> 12) * (255 / 15));
					color.g = (unsigned char)((float)((pixel & 0b0000111100000000) >> 8) * (255 / 15));
					color.b = (unsigned char)((float)((pixel & 0b0000000011110000) >> 4) * (255 / 15));
					color.a = (unsigned char)((float)(pixel & 0b0000000000001111) * (255 / 15));

				} break;
				case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
				{
					color.r = ((unsigned char*)image.data)[(y * image.width + x) * 4];
					color.g = ((unsigned char*)image.data)[(y * image.width + x) * 4 + 1];
					color.b = ((unsigned char*)image.data)[(y * image.width + x) * 4 + 2];
					color.a = ((unsigned char*)image.data)[(y * image.width + x) * 4 + 3];

				} break;
				case PIXELFORMAT_UNCOMPRESSED_R8G8B8:
				{
					color.r = (unsigned char)((unsigned char*)image.data)[(y * image.width + x) * 3];
					color.g = (unsigned char)((unsigned char*)image.data)[(y * image.width + x) * 3 + 1];
					color.b = (unsigned char)((unsigned char*)image.data)[(y * image.width + x) * 3 + 2];
					color.a = 255;

				} break;
				case PIXELFORMAT_UNCOMPRESSED_R32:
				{
					color.r = (unsigned char)(((float*)image.data)[y * image.width + x] * 255.0f);
					color.g = 0;
					color.b = 0;
					color.a = 255;

				} break;
				case PIXELFORMAT_UNCOMPRESSED_R32G32B32:
				{
					color.r = (unsigned char)(((float*)image.data)[(y * image.width + x) * 3] * 255.0f);
					color.g = (unsigned char)(((float*)image.data)[(y * image.width + x) * 3 + 1] * 255.0f);
					color.b = (unsigned char)(((float*)image.data)[(y * image.width + x) * 3 + 2] * 255.0f);
					color.a = 255;

				} break;
				case PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
				{
					color.r = (unsigned char)(((float*)image.data)[(y * image.width + x) * 4] * 255.0f);
					color.g = (unsigned char)(((float*)image.data)[(y * image.width + x) * 4] * 255.0f);
					color.b = (unsigned char)(((float*)image.data)[(y * image.width + x) * 4] * 255.0f);
					color.a = (unsigned char)(((float*)image.data)[(y * image.width + x) * 4] * 255.0f);

				} break;
				case PIXELFORMAT_UNCOMPRESSED_R16:
				{
					color.r = (unsigned char)(HalfToFloat(((unsigned short*)image.data)[y * image.width + x]) * 255.0f);
					color.g = 0;
					color.b = 0;
					color.a = 255;

				} break;
				case PIXELFORMAT_UNCOMPRESSED_R16G16B16:
				{
					color.r = (unsigned char)(HalfToFloat(((unsigned short*)image.data)[(y * image.width + x) * 3]) * 255.0f);
					color.g = (unsigned char)(HalfToFloat(((unsigned short*)image.data)[(y * image.width + x) * 3 + 1]) * 255.0f);
					color.b = (unsigned char)(HalfToFloat(((unsigned short*)image.data)[(y * image.width + x) * 3 + 2]) * 255.0f);
					color.a = 255;

				} break;
				case PIXELFORMAT_UNCOMPRESSED_R16G16B16A16:
				{
					color.r = (unsigned char)(HalfToFloat(((unsigned short*)image.data)[(y * image.width + x) * 4]) * 255.0f);
					color.g = (unsigned char)(HalfToFloat(((unsigned short*)image.data)[(y * image.width + x) * 4]) * 255.0f);
					color.b = (unsigned char)(HalfToFloat(((unsigned short*)image.data)[(y * image.width + x) * 4]) * 255.0f);
					color.a = (unsigned char)(HalfToFloat(((unsigned short*)image.data)[(y * image.width + x) * 4]) * 255.0f);

				} break;
				default: TRACELOG(LOG_WARNING, "Compressed image format does not support color reading"); break;
				}
			}
			else TRACELOG(LOG_WARNING, "Requested image pixel (%i, %i) out of bounds", x, y);

			return color;
		}

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)

		// Define one vertex (color)
		void Color4ub(unsigned char x, unsigned char y, unsigned char z, unsigned char w) {
			PLGL.State.colorr = x;
			PLGL.State.colorg = y;
			PLGL.State.colorb = z;
			PLGL.State.colora = w;
		}

		// Define one vertex (color)
		void Color4f(float r, float g, float b, float a) {
			Color4ub((unsigned char)(r * 255), (unsigned char)(g * 255), (unsigned char)(b * 255), (unsigned char)(a * 255));
		}

		// Define one vertex (color)
		void Color3f(float x, float y, float z) {
			Color4ub((unsigned char)(x * 255), (unsigned char)(y * 255), (unsigned char)(z * 255), 255);
		}

#endif

		// Set current texture to use
		void SetTexture(unsigned int id) {
			if (id == 0)
			{
#if defined(GRAPHICS_API_OPENGL_11)
				rlDisableTexture();
#else     // NOTE: If quads batch limit is reached, we force a draw call and next batch starts
				if (PLGL.State.vertexCounter >=
					PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].elementCount * 4)
				{
					DrawRenderBatch(PLGL.currentBatch);
				}

#endif
			}
			else
			{
#if defined(GRAPHICS_API_OPENGL_11)
				rlEnableTexture(id);
#else
				if (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].textureId != id)
				{
					if (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount > 0)
					{
						// Make sure current PLGL.currentBatch->draws[i].vertexCount is aligned a multiple of 4,
			   // that way, following QUADS drawing will keep aligned with index processing
			   // It implies adding some extra alignment vertex at the end of the draw,
			   // those vertex are not processed but they are considered as an additional offset
			   // for the next set of vertex to be drawn
						if (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].mode == PL_LINES) PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexAlignment = ((PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount < 4) ? PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount : PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount % 4);
						else if (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].mode == PL_TRIANGLES) PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexAlignment = ((PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount < 4) ? 1 : (4 - (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount % 4)));
						else PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexAlignment = 0;

						if (!CheckRenderBatchLimit(PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexAlignment))
						{
							PLGL.State.vertexCounter += PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexAlignment;

							PLGL.currentBatch->drawCounter++;
						}
					}
					if (PLGL.currentBatch->drawCounter >= PL_DEFAULT_BATCH_DRAWCALLS) DrawRenderBatch(PLGL.currentBatch);

					PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].textureId = id;
					PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount = 0;
				}

#endif
			}
		}

		// Select and active a texture slot
		void ActiveTextureSlot(int slot) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glActiveTexture(GL_TEXTURE0 + slot);
#endif
		}

		// Enable texture
		void EnableTexture(unsigned int id) {
#if defined(GRAPHICS_API_OPENGL_11)
			glEnable(GL_TEXTURE_2D);
#endif
			glBindTexture(GL_TEXTURE_2D, id);
		}

		// Disable texture
		void DisableTexture(void) {
#if defined(GRAPHICS_API_OPENGL_11)
			glDisable(GL_TEXTURE_2D);
#endif
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Enable texture cubemap
		void EnableTextureCubemap(unsigned int id) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glBindTexture(GL_TEXTURE_CUBE_MAP, id);
#endif
		}

		// Disable texture cubemap
		void DisableTextureCubemap(void) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
#endif
		}

		// Set texture parameters (wrap mode/filter mode)
		void TextureParameters(unsigned int id, int param, int value) {
			glBindTexture(GL_TEXTURE_2D, id);

#if !defined(GRAPHICS_API_OPENGL_11)
			// Reset anisotropy filter, in case it was set
		//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
#endif

			switch (param)
			{
			case PL_TEXTURE_WRAP_S:
			case PL_TEXTURE_WRAP_T:
			{
				if (value == PL_TEXTURE_WRAP_MIRROR_CLAMP)
				{
#if !defined(GRAPHICS_API_OPENGL_11)
					if (PLGL.ExtSupported.texMirrorClamp) glTexParameteri(GL_TEXTURE_2D, param, value);
					else TRACELOG(LOG_WARNING, "GL: Clamp mirror wrap mode not supported (GL_MIRROR_CLAMP_EXT)");
#endif
				}
				else glTexParameteri(GL_TEXTURE_2D, param, value);
			} break;
			case PL_TEXTURE_MAG_FILTER:
			case PL_TEXTURE_MIN_FILTER: glTexParameteri(GL_TEXTURE_2D, param, value); break;
			case PL_TEXTURE_FILTER_ANISOTROPIC:
			{
#if !defined(GRAPHICS_API_OPENGL_11)
				if (value <= PLGL.ExtSupported.maxAnisotropyLevel) {}// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
				else if (PLGL.ExtSupported.maxAnisotropyLevel > 0.0f)
				{
					TRACELOG(LOG_WARNING, "GL: Maximum anisotropic filter level supported is %iX", id, (int)PLGL.ExtSupported.maxAnisotropyLevel);
					//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
				}
				else TRACELOG(LOG_WARNING, "GL: Anisotropic filtering not supported");
#endif
			} break;
#if defined(GRAPHICS_API_OPENGL_33)
			case PL_TEXTURE_MIPMAP_BIAS_RATIO: glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, value / 100.0f);
#endif
			default: break;
			}

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Set cubemap parameters (wrap mode/filter mode)
		void CubemapParameters(unsigned int id, int param, int value) {
#if !defined(GRAPHICS_API_OPENGL_11)
			glBindTexture(GL_TEXTURE_CUBE_MAP, id);

			// Reset anisotropy filter, in case it was set
			//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);

			switch (param)
			{
			case PL_TEXTURE_WRAP_S:
			case PL_TEXTURE_WRAP_T:
			{
				if (value == PL_TEXTURE_WRAP_MIRROR_CLAMP)
				{
					if (PLGL.ExtSupported.texMirrorClamp) glTexParameteri(GL_TEXTURE_CUBE_MAP, param, value);
					else TRACELOG(LOG_WARNING, "GL: Clamp mirror wrap mode not supported (GL_MIRROR_CLAMP_EXT)");
				}
				else glTexParameteri(GL_TEXTURE_CUBE_MAP, param, value);
			} break;
			case PL_TEXTURE_MAG_FILTER:
			case PL_TEXTURE_MIN_FILTER: glTexParameteri(GL_TEXTURE_CUBE_MAP, param, value); break;
			case PL_TEXTURE_FILTER_ANISOTROPIC:
			{
				if (value <= PLGL.ExtSupported.maxAnisotropyLevel) {}// glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
				else if (PLGL.ExtSupported.maxAnisotropyLevel > 0.0f)
				{
					TRACELOG(LOG_WARNING, "GL: Maximum anisotropic filter level supported is %iX", id, (int)PLGL.ExtSupported.maxAnisotropyLevel);
					//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)value);
				}
				else TRACELOG(LOG_WARNING, "GL: Anisotropic filtering not supported");
			} break;
#if defined(GRAPHICS_API_OPENGL_33)
			case PL_TEXTURE_MIPMAP_BIAS_RATIO: glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_LOD_BIAS, value / 100.0f);
#endif
			default: break;
			}
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
#endif
		}

	}


	
}

