#include "gl_global.h"
#include "gl_buffer.h"
#include <logger/logger.h>

namespace DRAW {
	namespace GL {

		unsigned int LoadFramebuffer(void) {
			unsigned int fboId = 0;
			glGenFramebuffers(1, &fboId);       // Create the framebuffer object
			glBindFramebuffer(GL_FRAMEBUFFER, 0);   // Unbind any framebuffer
			return fboId;
		}
		// Attach color buffer texture to an fbo (unloads previous attachment)
		// NOTE: Attach type: 0-Color, 1-Depth renderbuffer, 2-Depth texture
		void FramebufferAttach(unsigned int fboId, unsigned int texId, int attachType, int texType, int mipLevel) {
			glBindFramebuffer(GL_FRAMEBUFFER, fboId);

            switch (attachType)
            {
            case PL_ATTACHMENT_COLOR_CHANNEL0:
            case PL_ATTACHMENT_COLOR_CHANNEL1:
            case PL_ATTACHMENT_COLOR_CHANNEL2:
            case PL_ATTACHMENT_COLOR_CHANNEL3:
            case PL_ATTACHMENT_COLOR_CHANNEL4:
            case PL_ATTACHMENT_COLOR_CHANNEL5:
            case PL_ATTACHMENT_COLOR_CHANNEL6:
            case PL_ATTACHMENT_COLOR_CHANNEL7:
            {
                if (texType == PL_ATTACHMENT_TEXTURE2D) glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachType, GL_TEXTURE_2D, texId, mipLevel);
                else if (texType == PL_ATTACHMENT_RENDERBUFFER) glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachType, GL_RENDERBUFFER, texId);
                else if (texType >= PL_ATTACHMENT_CUBEMAP_POSITIVE_X) glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachType, GL_TEXTURE_CUBE_MAP_POSITIVE_X + texType, texId, mipLevel);
            } break;
            case PL_ATTACHMENT_DEPTH:
            {
                if (texType == PL_ATTACHMENT_TEXTURE2D) glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texId, mipLevel);
                else if (texType == PL_ATTACHMENT_RENDERBUFFER)  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, texId);
            } break;
            case PL_ATTACHMENT_STENCIL:
            {
                if (texType == PL_ATTACHMENT_TEXTURE2D) glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texId, mipLevel);
                else if (texType == PL_ATTACHMENT_RENDERBUFFER)  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, texId);
            } break;
            default: break;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		// Verify render texture is complete
		bool FramebufferComplete(unsigned int id) {
            bool result = false;
            glBindFramebuffer(GL_FRAMEBUFFER, id);

            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                switch (status)
                {
                case GL_FRAMEBUFFER_UNSUPPORTED: TRACELOG(LOG_WARNING, "FBO: [ID %i] Framebuffer is unsupported", id); break;
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: TRACELOG(LOG_WARNING, "FBO: [ID %i] Framebuffer has incomplete attachment", id); break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: TRACELOG(LOG_WARNING, "FBO: [ID %i] Framebuffer has a missing attachment", id); break;
                default: break;
                }
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            result = (status == GL_FRAMEBUFFER_COMPLETE);
            return result;
		
		}
		// Unload framebuffer from GPU memory
		// NOTE: All attached textures/cubemaps/renderbuffers are also deleted
		void UnloadFramebuffer(unsigned int id) {
            // Query depth attachment to automatically delete texture/renderbuffer
            int depthType = 0, depthId = 0;
            glBindFramebuffer(GL_FRAMEBUFFER, id);   // Bind framebuffer to query depth texture type
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &depthType);
            // TODO: Review warning retrieving object name in WebGL
            // WARNING: WebGL: INVALID_ENUM: getFramebufferAttachmentParameter: invalid parameter name
            // https://registry.khronos.org/webgl/specs/latest/1.0/
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &depthId);

            unsigned int depthIdU = (unsigned int)depthId;
            if (depthType == GL_RENDERBUFFER) glDeleteRenderbuffers(1, &depthIdU);
            else if (depthType == GL_TEXTURE) glDeleteTextures(1, &depthIdU);

            // NOTE: If a texture object is deleted while its image is attached to the *currently bound* framebuffer,
            // the texture image is automatically detached from the currently bound framebuffer

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &id);

            TRACELOG(LOG_INFO, "FBO: [ID %i] Unloaded framebuffer from VRAM (GPU)", id);
		}



        // Enable rendering to texture (fbo)
        void EnableFramebuffer(unsigned int id) {
            glBindFramebuffer(GL_FRAMEBUFFER, id);
        }

        // return the active render texture (fbo)
        unsigned int GetActiveFramebuffer(void) {
            GLint fboId = 0;
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fboId);
            return fboId;
        }

        // Disable rendering to texture
        void DisableFramebuffer(void) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Bind framebuffer object (fbo)
        void BindFramebuffer(unsigned int target, unsigned int framebuffer) {
            glBindFramebuffer(target, framebuffer);
        }

        // Blit active framebuffer to main framebuffer
        void BlitFramebuffer(int srcX, int srcY, int srcWidth, int srcHeight, int dstX, int dstY, int dstWidth, int dstHeight, int bufferMask) {
            glBlitFramebuffer(srcX, srcY, srcWidth, srcHeight, dstX, dstY, dstWidth, dstHeight, bufferMask, GL_NEAREST);
        }



        // Set current framebuffer width
        void SetFramebufferWidth(int width)
        {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
            PLGL.State.framebufferWidth = width;
#endif
        }

        // Set current framebuffer height
        void SetFramebufferHeight(int height)
        {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
            PLGL.State.framebufferHeight = height;
#endif
        }

        // Get default framebuffer width
        int GetFramebufferWidth(void)
        {
            int width = 0;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
            width = PLGL.State.framebufferWidth;
#endif
            return width;
        }

        // Get default framebuffer height
        int GetFramebufferHeight(void)
        {
            int height = 0;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
            height = PLGL.State.framebufferHeight;
#endif
            return height;
        }
	}
}

