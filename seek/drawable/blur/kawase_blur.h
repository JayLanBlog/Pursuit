#pragma once
#include <rmd/gl/gl_include.h>
#include <core/core.h>
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <core/pass/draw_pass2d.h>
using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;

namespace BLUR {
    struct FBORender
    {
        unsigned int  frameBuffer;
        unsigned int depthBuffer;
        unsigned int texture;
        int m_width, m_height;
        FBORender(int width, int height) {
            m_width = width;
            m_height = height;
            frameBuffer = LoadFramebuffer();
            if (!frameBuffer)
            {
                TraceLog(LOG_WARNING, "Failed to create framebuffer");
                exit(1);
            }
            EnableFramebuffer(frameBuffer);
            texture = LoadTexture(nullptr,m_width,m_height, PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16,1);
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

    class KawaseBlur {
    public:
        KawaseBlur() {}

        ~KawaseBlur() {}

        void LoadSceneAndTexture( string img, int width = 1920, int height = 1080) {
            screenWidth = width;
            screenHeight = height;
            srcImage = LoadTexture(img.c_str());
            srcImage.width = screenWidth;
            srcImage.height = screenHeight;

            upScaleShader = LoadShader(0,"resource/shader/blur/kawase_upblur.fs");
            downScaleShader = LoadShader(0,"resource/shader/blur/kawase_downblur.fs");
            renderShader = LoadShader(0, "resource/shader/blur/kawase_blur.fs");

            upResLoc = GetShaderLocation(upScaleShader, "iResolution");
            upOffsetLoc = GetShaderLocation(upScaleShader, "offset");
            upHalfLoc = GetShaderLocation(upScaleShader, "halfpixel");
            downResLoc = GetShaderLocation(downScaleShader, "iResolution");
            downOffsetLoc = GetShaderLocation(downScaleShader, "offset");
            downHalfLoc = GetShaderLocation(downScaleShader, "halfpixel");
            SetShaderParams();
            FBORender* render = new FBORender(srcImage.width, srcImage.height);
            rboVector.push_back(render);
            for (int i = 1; i <= iterations; i++) {
                FBORender* render = new FBORender(srcImage.width / pow(2, i), srcImage.height / pow(2, i));
                rboVector.push_back(render);
            }
        }

        void SetShaderParams() {
            iResolution = { (float)srcImage.width, (float)srcImage.height };
            halfPiexl = { (float)0.5,(float)0.5  };
            float ratio = 0.5;
            float offsetWRatio = srcImage.width * ratio * 0.01;
            float offsetHRatio = srcImage.height * ratio * 0.01;
            offset = {(float)offsetWRatio / srcImage.width,(float)offsetHRatio / srcImage.height };
            SetShaderValue(upScaleShader, upResLoc, &iResolution, PL_SHADER_ATTRIB_VEC2);
            SetShaderValue(upScaleShader, upOffsetLoc, &offset, PL_SHADER_ATTRIB_VEC2);
            SetShaderValue(upScaleShader, upHalfLoc, &halfPiexl, PL_SHADER_ATTRIB_VEC2);
            
            SetShaderValue(downScaleShader, upResLoc, &iResolution, PL_SHADER_ATTRIB_VEC2);
            SetShaderValue(downScaleShader, upOffsetLoc, &offset, PL_SHADER_ATTRIB_VEC2);
            SetShaderValue(downScaleShader, upHalfLoc, &halfPiexl, PL_SHADER_ATTRIB_VEC2);
        }


        void Draw() {
            EnableFramebuffer(rboVector[0]->frameBuffer);
            BeginShaderMode(downScaleShader);
           //DrawTextureFx(srcImage, { 0.0,(float)(screenHeight -0.74 * rboVector[0]->m_height)}, 0.0f, 1., DARKGRAY);
            DrawTextureFx(srcImage, { 0.0,GetHorPos(rboVector[0]->m_height)}, 0.0f, 1., DARKGRAY);
            EndShaderMode();
            DisableFramebuffer();

            BeginShaderMode(upScaleShader);
            Texture2D tex = {rboVector[0]->texture,rboVector[0]->m_width,rboVector[0]->m_height ,1,PIXELFORMAT_UNCOMPRESSED_R16G16B16 };
            DrawTextureEx(tex, { 0.0,0.0}, 0.0f, 1.0, DARKGRAY);
            EndShaderMode();
        }
        

        void BlurDraw() {

            EnableFramebuffer(rboVector[0]->frameBuffer);
            BeginShaderMode(downScaleShader);
            DrawTextureFx(srcImage, { 0.0,GetHorPos(srcImage.height) }, 0.0f, 1., DARKGRAY);
            EndShaderMode();
            DisableFramebuffer();

            for (int i = 0; i < iterations; i++) {
                EnableFramebuffer(rboVector[i+1]->frameBuffer);
                Texture2D tex = { rboVector[i]->texture,rboVector[i+1]->m_width,rboVector[i+1]->m_height ,1,PIXELFORMAT_UNCOMPRESSED_R16G16B16 };
                  BeginShaderMode(downScaleShader);
                  DrawTextureFx(tex, { 0.0,GetHorPos(tex.height) }, 0.0f, 1., DARKGRAY);
                  EndShaderMode();
                DisableFramebuffer();
            }

            if (!enableUpscaleBlur){
                for (int i = iterations; i > 0; i--) {
                    EnableFramebuffer(rboVector[i - 1]->frameBuffer);
                    Texture2D tex = { rboVector[i]->texture,rboVector[i - 1]->m_width,rboVector[i - 1]->m_height ,1,PIXELFORMAT_UNCOMPRESSED_R16G16B16 };
                    BeginShaderMode(upScaleShader);
                    DrawTextureFx(tex, { 0.0,GetHorPos(tex.height) }, 0.0f, 1., DARKGRAY);
                    EndShaderMode();
                    DisableFramebuffer();
                }
            }
            
            BeginShaderMode(renderShader);
            Texture2D tex = { rboVector[0]->texture,rboVector[0]->m_width,rboVector[0]->m_height ,1,PIXELFORMAT_UNCOMPRESSED_R16G16B16 };
            DrawTextureEx(tex, { 0.0,0.0 }, 0.0f, 1.0, DARKGRAY);
            EndShaderMode();
        }
        
        float GetHorPos(float hegiht) {
            return screenHeight - hegiht;
        }

        bool enableUpscaleBlur = true;
        Vector2 iResolution;
        Vector2 offset;
        Vector2 halfPiexl;

        int upResLoc;
        int downResLoc;
        int upOffsetLoc;
        int downOffsetLoc;
        int upHalfLoc;
        int downHalfLoc;
        int iterations= 3;
        vector<FBORender*> rboVector;

        int m_iterations = 0;
        Shader upScaleShader;
        Shader downScaleShader;
        Shader renderShader;
        Texture2D srcImage;
        int screenWidth = 1920;
        int screenHeight = 1080;
    };
}