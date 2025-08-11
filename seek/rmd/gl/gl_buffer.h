#pragma once
#include <section/enum/attachment_em.h>

namespace DRAW {
	namespace GL {

		// Attach color buffer texture to an fbo (unloads previous attachment)
		// NOTE: Attach type: 0-Color, 1-Depth renderbuffer, 2-Depth texture
		void FramebufferAttach(unsigned int fboId, unsigned int texId, int attachType, int texType, int mipLevel);
		// Verify render texture is complete
		bool FramebufferComplete(unsigned int id);
		// Unload framebuffer from GPU memory
		// NOTE: All attached textures/cubemaps/renderbuffers are also deleted
		void UnloadFramebuffer(unsigned int id);

		// Enable rendering to texture (fbo)
		void EnableFramebuffer(unsigned int id);

		// return the active render texture (fbo)
		unsigned int GetActiveFramebuffer(void);

		// Disable rendering to texture
		void DisableFramebuffer(void);

		// Bind framebuffer object (fbo)
		void BindFramebuffer(unsigned int target, unsigned int framebuffer);

		// Blit active framebuffer to main framebuffer
		void BlitFramebuffer(int srcX, int srcY, int srcWidth, int srcHeight, int dstX, int dstY, int dstWidth, int dstHeight, int bufferMask);
		unsigned int LoadFramebuffer(void);
		// Set current framebuffer height
		void SetFramebufferHeight(int height);
		// Set current framebuffer width
		void SetFramebufferWidth(int width);
		// Get default framebuffer width
		int GetFramebufferWidth(void);
		// Get default framebuffer height
		int GetFramebufferHeight(void);

		void ActiveDrawBuffers(int count);
	}
}