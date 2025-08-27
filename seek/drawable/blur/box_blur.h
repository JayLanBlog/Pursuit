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
	class BoxBlur {
	public:
		BoxBlur() {}
		~BoxBlur() {}
		void LoadSceneAndTexture(string uri, string img, int width = 1920, int height = 1080) {
			screenHeight = height;
			screenWidth = width;
			srcImage = LoadTexture(img.c_str());
			texelWidthOffset = 5.0 / srcImage.width;
			texelHeightOffset = 5.0 / srcImage.height;
			//DRAW::GL::LoadTexture(img);
			shader = LoadShader(0, uri.c_str());
			texelWidthOffsetLoc = GetShaderLocation(shader, "texelWidthOffset");
			texelHeightOffsetLoc = GetShaderLocation(shader, "texelHeightOffset");
			BlurKernelSizeLoc = GetShaderLocation(shader, "BlurKernelSize"); 
			BlurDirectionLoc = GetShaderLocation(shader, "BlurDirection");
			
			SetShaderValue(shader, texelWidthOffsetLoc, &texelWidthOffset, SHADER_ATTRIB_FLOAT);
			SetShaderValue(shader, texelHeightOffsetLoc, &texelHeightOffset, SHADER_ATTRIB_FLOAT);
			SetShaderValue(shader, BlurKernelSizeLoc, &blurKernelSize, SHADER_ATTRIB_FLOAT);
			SetShaderValue(shader, BlurDirectionLoc, &blurDirect, SHADER_ATTRIB_FLOAT);
		}

		void Render() {

			BeginShaderMode(shader);
			//DrawRectanglePro (rect, { 0.0f,0.0f }, 0.0f, RED);
			//DrawTextureEx
			//DrawTextureEx(srcImage, { (float)((screenWidth - srcImage.width) / 40), (float)((screenHeight - srcImage.height) / 4.0 )}, 0.0f, 1.5f, DARKGRAY);
			DrawTextureEx(srcImage, { (float)((screenWidth - srcImage.width) * 0.1),(float)((screenWidth - srcImage.height) * 0.1) }, 0.0f, 1.3, DARKGRAY);
			//DrawTexture(srcImage, ((screenWidth - srcImage.width) /2.0), ( (screenHeight - srcImage.height)/2.0), DARKGRAY);
			EndShaderMode();
		}

		int BlurDirectionLoc;
		int BlurKernelSizeLoc;
		int texelWidthOffsetLoc;
		int texelHeightOffsetLoc;
		Shader shader;
		float blurKernelSize = 3.0;
		float texelWidthOffset;
		float texelHeightOffset;
		float blurDirect = 0.0;
		int screenWidth = 1920;
		int screenHeight = 1080;
		Texture2D srcImage;
	};
}