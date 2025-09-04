#pragma once
#include <drawable/benchmark/sky_box.h>
#include <drawable/benchmark/custom_model.h>
using namespace Acr;

namespace Scene {
    struct DIsolveFBO
    {
        unsigned int  frameBuffer;
        unsigned int depthBuffer;
        unsigned int texture;
        int m_width, m_height;
        DIsolveFBO(int width, int height) {
            m_width = width;
            m_height = height;
            frameBuffer = LoadFramebuffer();
            if (!frameBuffer)
            {
                TraceLog(LOG_WARNING, "Failed to create framebuffer");
                exit(1);
            }
            EnableFramebuffer(frameBuffer);
            texture = LoadTexture(nullptr, m_width, m_height, PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
            ActiveDrawBuffers(1);
            // Now we attach our textures to the framebuffer.
            FramebufferAttach(frameBuffer, texture, PL_ATTACHMENT_COLOR_CHANNEL0, PL_ATTACHMENT_TEXTURE2D, 0);
            depthBuffer = LoadTextureDepth(m_width, m_height, true);
            FramebufferAttach(frameBuffer, depthBuffer, PL_ATTACHMENT_DEPTH, PL_ATTACHMENT_RENDERBUFFER, 0);
            // to rlDisableFramebuffer() here.
            if (!FramebufferComplete(frameBuffer))
            {
                TraceLog(LOG_WARNING, "Framebuffer is not complete");
            }
        }

    };
	class DisolveScene {
	public:

		CustM cust;
		SkyBox box;

        DIsolveFBO* disFbo;

		void LoaderScene(int width, int height) {
            disFbo = new DIsolveFBO(width,height);
			cust.LoadScene(width,height);
			box.LoadScene(width, height);
		}

		void Tick() {
			cust.Tick();
			box.Tick();
		}

		void Render() {
           
            cust.Render();
            box.Render();

            /*EnableFramebuffer(disFbo->frameBuffer);
            ClearScreenBuffers();
            DisableColorBlend();
			box.Render();
            DisableFramebuffer();
            EnableColorBlend();
			cust.RenderWithTexture(disFbo->texture);
			*/
		}
	};

}