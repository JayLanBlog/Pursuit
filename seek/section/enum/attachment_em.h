#pragma once
// Framebuffer attachment type
// NOTE: By default up to 8 color channels defined, but it can be more
typedef enum {
    PL_ATTACHMENT_COLOR_CHANNEL0 = 0,       // Framebuffer attachment type: color 0
    PL_ATTACHMENT_COLOR_CHANNEL1 = 1,       // Framebuffer attachment type: color 1
    PL_ATTACHMENT_COLOR_CHANNEL2 = 2,       // Framebuffer attachment type: color 2
    PL_ATTACHMENT_COLOR_CHANNEL3 = 3,       // Framebuffer attachment type: color 3
    PL_ATTACHMENT_COLOR_CHANNEL4 = 4,       // Framebuffer attachment type: color 4
    PL_ATTACHMENT_COLOR_CHANNEL5 = 5,       // Framebuffer attachment type: color 5
    PL_ATTACHMENT_COLOR_CHANNEL6 = 6,       // Framebuffer attachment type: color 6
    PL_ATTACHMENT_COLOR_CHANNEL7 = 7,       // Framebuffer attachment type: color 7
    PL_ATTACHMENT_DEPTH = 100,              // Framebuffer attachment type: depth
    PL_ATTACHMENT_STENCIL = 200,            // Framebuffer attachment type: stencil
} glFramebufferAttachType;


// Framebuffer texture attachment type
typedef enum {
    PL_ATTACHMENT_CUBEMAP_POSITIVE_X = 0,   // Framebuffer texture attachment type: cubemap, +X side
    PL_ATTACHMENT_CUBEMAP_NEGATIVE_X = 1,   // Framebuffer texture attachment type: cubemap, -X side
    PL_ATTACHMENT_CUBEMAP_POSITIVE_Y = 2,   // Framebuffer texture attachment type: cubemap, +Y side
    PL_ATTACHMENT_CUBEMAP_NEGATIVE_Y = 3,   // Framebuffer texture attachment type: cubemap, -Y side
    PL_ATTACHMENT_CUBEMAP_POSITIVE_Z = 4,   // Framebuffer texture attachment type: cubemap, +Z side
    PL_ATTACHMENT_CUBEMAP_NEGATIVE_Z = 5,   // Framebuffer texture attachment type: cubemap, -Z side
    PL_ATTACHMENT_TEXTURE2D = 100,          // Framebuffer texture attachment type: texture2d
    PL_ATTACHMENT_RENDERBUFFER = 200,       // Framebuffer texture attachment type: renderbuffer
} glFramebufferAttachTextureType;