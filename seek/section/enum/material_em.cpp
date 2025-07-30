#include "material_em.h"

const char* glGetPixelFormatName(unsigned int format) {
    switch (format)
    {
    case PL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: return "GRAYSCALE"; break;         // 8 bit per pixel (no alpha)
    case PL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: return "GRAY_ALPHA"; break;       // 8*2 bpp (2 channels)
    case PL_PIXELFORMAT_UNCOMPRESSED_R5G6B5: return "R5G6B5"; break;               // 16 bpp
    case PL_PIXELFORMAT_UNCOMPRESSED_R8G8B8: return "R8G8B8"; break;               // 24 bpp
    case PL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1: return "R5G5B5A1"; break;           // 16 bpp (1 bit alpha)
    case PL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: return "R4G4B4A4"; break;           // 16 bpp (4 bit alpha)
    case PL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: return "R8G8B8A8"; break;           // 32 bpp
    case PL_PIXELFORMAT_UNCOMPRESSED_R32: return "R32"; break;                     // 32 bpp (1 channel - float)
    case PL_PIXELFORMAT_UNCOMPRESSED_R32G32B32: return "R32G32B32"; break;         // 32*3 bpp (3 channels - float)
    case PL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: return "R32G32B32A32"; break;   // 32*4 bpp (4 channels - float)
    case PL_PIXELFORMAT_UNCOMPRESSED_R16: return "R16"; break;                     // 16 bpp (1 channel - half float)
    case PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16: return "R16G16B16"; break;         // 16*3 bpp (3 channels - half float)
    case PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16: return "R16G16B16A16"; break;   // 16*4 bpp (4 channels - half float)
    case PL_PIXELFORMAT_COMPRESSED_DXT1_RGB: return "DXT1_RGB"; break;             // 4 bpp (no alpha)
    case PL_PIXELFORMAT_COMPRESSED_DXT1_RGBA: return "DXT1_RGBA"; break;           // 4 bpp (1 bit alpha)
    case PL_PIXELFORMAT_COMPRESSED_DXT3_RGBA: return "DXT3_RGBA"; break;           // 8 bpp
    case PL_PIXELFORMAT_COMPRESSED_DXT5_RGBA: return "DXT5_RGBA"; break;           // 8 bpp
    case PL_PIXELFORMAT_COMPRESSED_ETC1_RGB: return "ETC1_RGB"; break;             // 4 bpp
    case PL_PIXELFORMAT_COMPRESSED_ETC2_RGB: return "ETC2_RGB"; break;             // 4 bpp
    case PL_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA: return "ETC2_RGBA"; break;       // 8 bpp
    case PL_PIXELFORMAT_COMPRESSED_PVRT_RGB: return "PVRT_RGB"; break;             // 4 bpp
    case PL_PIXELFORMAT_COMPRESSED_PVRT_RGBA: return "PVRT_RGBA"; break;           // 4 bpp
    case PL_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA: return "ASTC_4x4_RGBA"; break;   // 8 bpp
    case PL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA: return "ASTC_8x8_RGBA"; break;   // 2 bpp
    default: return "UNKNOWN"; break;
    }
}