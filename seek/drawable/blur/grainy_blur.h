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
	class GrainyBlur {
	public:
		GrainyBlur() {}
		~GrainyBlur() {}

		void LoadSceneAndTexture(string img, int width = 1920, int height = 1080) {
			screenHeight = height;
			screenWidth = width;
			srcImage = LoadTexture(img.c_str());
			srcImage.width = screenWidth;
			srcImage.height = screenHeight;
			bokehShader = LoadShader(0, "resource/shader/blur/grainy_blur.fs");

			u_resolution_loc = GetShaderLocation(bokehShader, "u_resolution");
			u_mouse_loc = GetShaderLocation(bokehShader, "u_mouse");
			u_time_loc = GetShaderLocation(bokehShader, "u_time");
			resolution[0] = screenWidth;
			resolution[1] = screenHeight;
			SetShaderValue(bokehShader, u_resolution_loc, resolution, SHADER_UNIFORM_VEC2);
		}

		void Tick() {
			float deltaTime = GetFrameTime();
			u_time += deltaTime;
			SetShaderValue(bokehShader, u_time_loc, &u_time, SHADER_UNIFORM_FLOAT);
			//CORE.Input.Mouse.currentPosition
			SetShaderValue(bokehShader, u_mouse_loc, &CORE.Input.Mouse.currentPosition, SHADER_UNIFORM_VEC2);
			// Check if screen is resized
			if (IsWindowResized())
			{
				resolution[0] = (float)GetScreenWidth();
				resolution[1] = (float)GetScreenHeight();
				SetShaderValue(bokehShader, u_resolution_loc, resolution, SHADER_UNIFORM_VEC2);
			}
		}

		void Render() {
			BeginShaderMode(bokehShader);
			//	Texture2D tex = { rboVector[0]->texture,rboVector[0]->m_width,rboVector[0]->m_height ,1,PIXELFORMAT_UNCOMPRESSED_R16G16B16 };
			DrawTextureEx(srcImage, { 0.0,0.0 }, 0.0f, 1.0, DARKGRAY);
			EndShaderMode();
		}

		int u_resolution_loc;
		int u_mouse_loc;
		int u_time_loc;

		float resolution[2];
		float u_time;
		//CORE.Input.Mouse.currentPosition
		Texture2D srcImage;
		Vector2 u_mouse;
		Shader bokehShader;
		int screenWidth = 1920;
		int screenHeight = 1080;
	};
}