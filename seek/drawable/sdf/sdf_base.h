#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <core/pass/draw_pass2d.h>


using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace Loader;


namespace SDF {

	class SDFBase {
	public:
		
		SDFBase() {}

		~SDFBase(){}
		
		void LoadSdfFile(string url,int width = 1920,int height =1080) {
			camera.position = { 2.5f, 2.5f, 3.0f };    // Camera position
			camera.target ={ 0.0f, 0.0f, 0.7f };      // Camera looking at point
			camera.up ={ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 65.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

			screenHeight = height;
			screenWidth = width;
			sdfShader = LoadShader(0, url.c_str());
			u_resolution_loc = GetShaderLocation(sdfShader, "u_resolution");
			u_mouse_loc = GetShaderLocation(sdfShader, "u_mouse");
			u_time_loc = GetShaderLocation(sdfShader, "u_time");
			resolution[0] = screenWidth;
			resolution[1] = screenHeight;
			SetShaderValue(sdfShader, u_resolution_loc, resolution, SHADER_UNIFORM_VEC2);
		//	DisableCursor();                    // Limit cursor to relative movement inside the window
		}
		

		void Tick(){
			//----------------------------------------------------------------------------------
			UpdateCamera(&camera, CAMERA_FIRST_PERSON);
			float deltaTime = GetFrameTime();
			u_time += deltaTime;
			SetShaderValue(sdfShader, u_time_loc, &u_time, SHADER_UNIFORM_FLOAT);
			//CORE.Input.Mouse.currentPosition
			SetShaderValue(sdfShader, u_mouse_loc,&CORE.Input.Mouse.currentPosition, SHADER_UNIFORM_VEC2);
			// Check if screen is resized
			if (IsWindowResized())
			{
				resolution[0] = (float)GetScreenWidth();
				resolution[1] = (float)GetScreenHeight();
				SetShaderValue(sdfShader, u_resolution_loc, resolution, SHADER_UNIFORM_VEC2);
			}
		}

		void Draw() {
			BeginShaderMode(sdfShader);
			DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
			EndShaderMode();
		}
		
		float resolution[2];
		int u_resolution_loc;
		int u_mouse_loc;
		int u_time_loc;
		//
		// Initialization
		//--------------------------------------------------------------------------------------
		int screenWidth = 1920;
		int screenHeight = 1080;
		Camera camera = { 0 };
		float u_time;
		//CORE.Input.Mouse.currentPosition
		Vector2 u_mouse;
		Shader sdfShader;
	};



};