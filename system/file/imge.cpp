#include "image.h"
using namespace Seek;
#include <stb_image.h>
#include <logger/logger.h>
#include <section/enum/material_em.h>
namespace System {
	void UnloadImage(Image image) {
		FREE(image.data);
	}	

	Image LoadImageSource(const char* fileName) {
		Image image = { 0 };
#if defined(SUPPORT_FILEFORMAT_PNG) || \
    defined(SUPPORT_FILEFORMAT_BMP) || \
    defined(SUPPORT_FILEFORMAT_TGA) || \
    defined(SUPPORT_FILEFORMAT_JPG) || \
    defined(SUPPORT_FILEFORMAT_GIF) || \
    defined(SUPPORT_FILEFORMAT_PIC) || \
    defined(SUPPORT_FILEFORMAT_HDR) || \
    defined(SUPPORT_FILEFORMAT_PNM) || \
    defined(SUPPORT_FILEFORMAT_PSD)

#define STBI_REQUIRED
#endif
		// Loading file to memory
		int dataSize = 0;
		unsigned char* fileData = LoadFileData(fileName, &dataSize);
		// Loading image from memory data
		if (fileData != NULL)
		{
			image = LoadImageFromMemory(GetFileExtension(fileName), fileData, dataSize);

			UnloadFileData(fileData);
		}

		return image;
	}

	Image LoadImageAnim(const char* fileName, int* frames) {
		Image image = { 0 };
		int frameCount = 0;
#if defined(SUPPORT_FILEFORMAT_GIF)
		if (IsFileExtension(fileName, ".gif"))
		{
			int dataSize = 0;
			unsigned char* fileData = LoadFileData(fileName, &dataSize);

			if (fileData != NULL)
			{
				int comp = 0;
				int* delays = NULL;
				image.data = stbi_load_gif_from_memory(fileData, dataSize, &delays, &image.width, &image.height, &frameCount, &comp, 4);

				image.mipmaps = 1;
				image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

				UnloadFileData(fileData);
				FREE(delays);        // NOTE: Frames delays are discarded
			}
		}
#else
			if (false) {}
#endif
            else
            {
                image = LoadImageSource(fileName);
                frameCount = 1;
            }

            *frames = frameCount;
            return image;
	}

	Image LoadImageAnimFromMemory(const char* fileType, const unsigned char* fileData, int dataSize, int* frames) {
		Image image = { 0 };
		int frameCount = 0;

		// Security check for input data
		if ((fileType == NULL) || (fileData == NULL) || (dataSize == 0)) return image;

#if defined(SUPPORT_FILEFORMAT_GIF)
		if ((strcmp(fileType, ".gif") == 0) || (strcmp(fileType, ".GIF") == 0))
		{
			if (fileData != NULL)
			{
				int comp = 0;
				int* delays = NULL;
				image.data = stbi_load_gif_from_memory(fileData, dataSize, &delays, &image.width, &image.height, &frameCount, &comp, 4);

				image.mipmaps = 1;
				image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

				RL_FREE(delays);        // NOTE: Frames delays are discarded
			}
		}
#else
		if (false) {}
#endif
		else
		{
			image = LoadImageFromMemory(fileType, fileData, dataSize);
			frameCount = 1;
		}

		*frames = frameCount;
		return image;
	}

	Image LoadImageFromMemory(const char* fileType, const unsigned char* fileData, int dataSize) {
		Image image = { 0 };

		// Security check for input data
		if ((fileData == NULL) || (dataSize == 0))
		{
			TRACELOG(LOG_WARNING, "IMAGE: Invalid file data");
			return image;
		}
		if (fileType == NULL)
		{
			TRACELOG(LOG_WARNING, "IMAGE: Missing file extension");
			return image;
		}


        if ((false)
#if defined(SUPPORT_FILEFORMAT_PNG)
            || (strcmp(fileType, ".png") == 0) || (strcmp(fileType, ".PNG") == 0)
#endif
#if defined(SUPPORT_FILEFORMAT_BMP)
            || (strcmp(fileType, ".bmp") == 0) || (strcmp(fileType, ".BMP") == 0)
#endif
#if defined(SUPPORT_FILEFORMAT_TGA)
            || (strcmp(fileType, ".tga") == 0) || (strcmp(fileType, ".TGA") == 0)
#endif
#if defined(SUPPORT_FILEFORMAT_JPG)
            || (strcmp(fileType, ".jpg") == 0) || (strcmp(fileType, ".jpeg") == 0)
            || (strcmp(fileType, ".JPG") == 0) || (strcmp(fileType, ".JPEG") == 0)
#endif
#if defined(SUPPORT_FILEFORMAT_GIF)
            || (strcmp(fileType, ".gif") == 0) || (strcmp(fileType, ".GIF") == 0)
#endif
#if defined(SUPPORT_FILEFORMAT_PIC)
            || (strcmp(fileType, ".pic") == 0) || (strcmp(fileType, ".PIC") == 0)
#endif
#if defined(SUPPORT_FILEFORMAT_PNM)
            || (strcmp(fileType, ".ppm") == 0) || (strcmp(fileType, ".pgm") == 0)
            || (strcmp(fileType, ".PPM") == 0) || (strcmp(fileType, ".PGM") == 0)
#endif
#if defined(SUPPORT_FILEFORMAT_PSD)
            || (strcmp(fileType, ".psd") == 0) || (strcmp(fileType, ".PSD") == 0)
#endif
            )
        {
#if defined(STBI_REQUIRED)
            // NOTE: Using stb_image to load images (Supports multiple image formats)

            if (fileData != NULL)
            {
                int comp = 0;
                image.data = stbi_load_from_memory(fileData, dataSize, &image.width, &image.height, &comp, 0);

                if (image.data != NULL)
                {
                    image.mipmaps = 1;

                    if (comp == 1) image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
                    else if (comp == 2) image.format = PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA;
                    else if (comp == 3) image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
                    else if (comp == 4) image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
                }
            }
#endif
        }
#if defined(SUPPORT_FILEFORMAT_HDR)
        else if ((strcmp(fileType, ".hdr") == 0) || (strcmp(fileType, ".HDR") == 0))
        {
#if defined(STBI_REQUIRED)
            if (fileData != NULL)
            {
                int comp = 0;
                image.data = stbi_loadf_from_memory(fileData, dataSize, &image.width, &image.height, &comp, 0);

                image.mipmaps = 1;

                if (comp == 1) image.format = PIXELFORMAT_UNCOMPRESSED_R32;
                else if (comp == 3) image.format = PIXELFORMAT_UNCOMPRESSED_R32G32B32;
                else if (comp == 4) image.format = PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
                else
                {
                    TRACELOG(LOG_WARNING, "IMAGE: HDR file format not supported");
                    UnloadImage(image);
                }
            }
#endif
        }
#endif
#if defined(SUPPORT_FILEFORMAT_QOI)
        else if ((strcmp(fileType, ".qoi") == 0) || (strcmp(fileType, ".QOI") == 0))
        {
            if (fileData != NULL)
            {
                qoi_desc desc = { 0 };
                image.data = qoi_decode(fileData, dataSize, &desc, (int)fileData[12]);
                image.width = desc.width;
                image.height = desc.height;
                image.format = desc.channels == 4 ? PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 : PIXELFORMAT_UNCOMPRESSED_R8G8B8;
                image.mipmaps = 1;
            }
        }
#endif
#if defined(SUPPORT_FILEFORMAT_DDS)
        else if ((strcmp(fileType, ".dds") == 0) || (strcmp(fileType, ".DDS") == 0))
        {
            image.data = rl_load_dds_from_memory(fileData, dataSize, &image.width, &image.height, &image.format, &image.mipmaps);
        }
#endif
#if defined(SUPPORT_FILEFORMAT_PKM)
        else if ((strcmp(fileType, ".pkm") == 0) || (strcmp(fileType, ".PKM") == 0))
        {
            image.data = rl_load_pkm_from_memory(fileData, dataSize, &image.width, &image.height, &image.format, &image.mipmaps);
        }
#endif
#if defined(SUPPORT_FILEFORMAT_KTX)
        else if ((strcmp(fileType, ".ktx") == 0) || (strcmp(fileType, ".KTX") == 0))
        {
            image.data = rl_load_ktx_from_memory(fileData, dataSize, &image.width, &image.height, &image.format, &image.mipmaps);
        }
#endif
#if defined(SUPPORT_FILEFORMAT_PVR)
        else if ((strcmp(fileType, ".pvr") == 0) || (strcmp(fileType, ".PVR") == 0))
        {
            image.data = rl_load_pvr_from_memory(fileData, dataSize, &image.width, &image.height, &image.format, &image.mipmaps);
        }
#endif
#if defined(SUPPORT_FILEFORMAT_ASTC)
        else if ((strcmp(fileType, ".astc") == 0) || (strcmp(fileType, ".ASTC") == 0))
        {
            image.data = rl_load_astc_from_memory(fileData, dataSize, &image.width, &image.height, &image.format, &image.mipmaps);
        }
#endif
        else TRACELOG(LOG_WARNING, "IMAGE: Data format not supported");

        if (image.data != NULL) TRACELOG(LOG_INFO, "IMAGE: Data loaded successfully (%ix%i | %s | %i mipmaps)", image.width, image.height, glGetPixelFormatName(image.format), image.mipmaps);
        else TRACELOG(LOG_WARNING, "IMAGE: Failed to load image data");

        return image;
	}

	bool IsImageValid(Image image) {
        bool result = false;

        if ((image.data != NULL) &&     // Validate pixel data available
            (image.width > 0) &&        // Validate image width
            (image.height > 0) &&       // Validate image height
            (image.format > 0) &&       // Validate image format
            (image.mipmaps > 0)) result = true; // Validate image mipmaps (at least 1 for basic mipmap level)

        return result;
	}

	bool ExportImage(Image image, const char* fileName) {
        int result = 0;

        // Security check for input data
        if ((image.width == 0) || (image.height == 0) || (image.data == NULL)) return result;

#if defined(SUPPORT_IMAGE_EXPORT)
        int channels = 4;
        bool allocatedData = false;
        unsigned char* imgData = (unsigned char*)image.data;

        if (image.format == PIXELFORMAT_UNCOMPRESSED_GRAYSCALE) channels = 1;
        else if (image.format == PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA) channels = 2;
        else if (image.format == PIXELFORMAT_UNCOMPRESSED_R8G8B8) channels = 3;
        else if (image.format == PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) channels = 4;
        else
        {
            // NOTE: Getting Color array as RGBA unsigned char values
            imgData = (unsigned char*)LoadImageColors(image);
            allocatedData = true;
        }

#if defined(SUPPORT_FILEFORMAT_PNG)
        if (IsFileExtension(fileName, ".png"))
        {
            int dataSize = 0;
            unsigned char* fileData = stbi_write_png_to_mem((const unsigned char*)imgData, image.width * channels, image.width, image.height, channels, &dataSize);
            result = SaveFileData(fileName, fileData, dataSize);
            RL_FREE(fileData);
        }
#else
        if (false) {}
#endif
#if defined(SUPPORT_FILEFORMAT_BMP)
        else if (IsFileExtension(fileName, ".bmp")) result = stbi_write_bmp(fileName, image.width, image.height, channels, imgData);
#endif
#if defined(SUPPORT_FILEFORMAT_TGA)
        else if (IsFileExtension(fileName, ".tga")) result = stbi_write_tga(fileName, image.width, image.height, channels, imgData);
#endif
#if defined(SUPPORT_FILEFORMAT_JPG)
        else if (IsFileExtension(fileName, ".jpg") ||
            IsFileExtension(fileName, ".jpeg")) result = stbi_write_jpg(fileName, image.width, image.height, channels, imgData, 90);  // JPG quality: between 1 and 100
#endif
#if defined(SUPPORT_FILEFORMAT_QOI)
        else if (IsFileExtension(fileName, ".qoi"))
        {
            channels = 0;
            if (image.format == PIXELFORMAT_UNCOMPRESSED_R8G8B8) channels = 3;
            else if (image.format == PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) channels = 4;
            else TRACELOG(LOG_WARNING, "IMAGE: Image pixel format must be R8G8B8 or R8G8B8A8");

            if ((channels == 3) || (channels == 4))
            {
                qoi_desc desc = { 0 };
                desc.width = image.width;
                desc.height = image.height;
                desc.channels = channels;
                desc.colorspace = QOI_SRGB;

                result = qoi_write(fileName, imgData, &desc);
            }
        }
#endif
#if defined(SUPPORT_FILEFORMAT_KTX)
        else if (IsFileExtension(fileName, ".ktx"))
        {
            result = rl_save_ktx(fileName, image.data, image.width, image.height, image.format, image.mipmaps);
        }
#endif
        else if (IsFileExtension(fileName, ".raw"))
        {
            // Export raw pixel data (without header)
            // NOTE: It's up to the user to track image parameters
            result = SaveFileData(fileName, image.data, GetPixelDataSize(image.width, image.height, image.format));
        }

        if (allocatedData) RL_FREE(imgData);
#endif      // SUPPORT_IMAGE_EXPORT

        if (result != 0) TRACELOG(LOG_INFO, "FILEIO: [%s] Image exported successfully", fileName);
        else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to export image", fileName);

        return result;
	}
}