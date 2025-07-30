#pragma once
#include "sys_file.h"
#include <section/graph/material_def.h>
using namespace Seek;
namespace System {

	void UnloadImage(Image image);

	Image LoadImageSource(const char* fileName);

	// Load animated image data
	//  - Image.data buffer includes all frames: [image#0][image#1][image#2][...]
	//  - Number of frames is returned through 'frames' parameter
	//  - All frames are returned in RGBA format
	//  - Frames delay data is discarded
	Image LoadImageAnim(const char* fileName, int* frames);

	// Load animated image data
	//  - Image.data buffer includes all frames: [image#0][image#1][image#2][...]
	//  - Number of frames is returned through 'frames' parameter
	//  - All frames are returned in RGBA format
	//  - Frames delay data is discarded
	Image LoadImageAnimFromMemory(const char* fileType, const unsigned char* fileData, int dataSize, int* frames);

	// Load image from memory buffer, fileType refers to extension: i.e. ".png"
	// WARNING: File extension must be provided in lower-case
	Image LoadImageFromMemory(const char* fileType, const unsigned char* fileData, int dataSize);
	
	// Load image from GPU texture data
	// NOTE: Compressed texture formats not supported
	//Image LoadImageFromTexture(Texture2D texture);
	//Image LoadImageFromTexture(Texture2D texture)
	// Check if an image is ready
	bool IsImageValid(Image image);

	// Export image data to file
	// NOTE: File format depends on fileName extension
	bool ExportImage(Image image, const char* fileName);

}