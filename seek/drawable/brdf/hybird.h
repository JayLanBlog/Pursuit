#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <gemo/TorusKnotGeometry.h>
#include <math/pr_math.h>
#include <core/pass/draw_pass2d.h>
#include <core/text/draw_shape.h>

using namespace PMath;
using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace GEMO;

namespace BRDF {
	typedef struct {
		unsigned int camPos, camDir, screenCenter;
	}RayLocs;

	class HyBird {
	public:
		const string BasePath = "E:/gl/resource/res/water/";
		const string RootPath = "E:/gl/resource/";

		HyBird() {}
		~HyBird() {}

		void LoadScene(int width, int height) {
			screenWidth = width;
			screenHeight = height;
			
			string rfs = RootPath + "shader/brdf/hybrid_raymarch.fs";
			string rasterfs = RootPath + "shader/brdf/hybrid_raster.fs";

			shdrRaymarch = LoadShader(0,rfs.c_str());
			shdrRaster = LoadShader(0,rasterfs.c_str());
			
			// Fill the struct with shader locs.
			marchLocs.camPos = GetShaderLocation(shdrRaymarch, "camPos");
			marchLocs.camDir = GetShaderLocation(shdrRaymarch, "camDir");
			marchLocs.screenCenter = GetShaderLocation(shdrRaymarch, "screenCenter");

			screenCenter = {  screenWidth / 2.0f,screenHeight / 2.0f };
			SetShaderValue(shdrRaymarch, marchLocs.screenCenter, &screenCenter, SHADER_UNIFORM_VEC2);
			target = LoadRenderTextureDepthTex(screenWidth, screenHeight);
			
			// Define the camera to look into our 3d world
			camera = {
				{ 0.5f, 1.0f, 1.5f },    // Camera position
				{ 0.0f, 0.5f, 0.0f },      // Camera looking at point
				{ 0.0f, 1.0f, 0.0f },          // Camera up vector (rotation towards target)
				45.0f,                                // Camera field-of-view Y
				CAMERA_PERSPECTIVE              // Camera projection type
			};
			camDist = 1.0f / (tanf(camera.fovy * 0.5f * DEG2RAD));
		}

		void Tick() {
			// Update
			//----------------------------------------------------------------------------------
			UpdateCamera(&camera, CAMERA_ORBITAL);
			// Update Camera Postion in the ray march shader.
			SetShaderValue(shdrRaymarch, marchLocs.camPos, &(camera.position), SHADER_UNIFORM_VEC3);
			// Update Camera Looking Vector. Vector length determines FOV.
			Vector3 camDir = Vector3Scale(Vector3Normalize(Vector3Subtract(camera.target, camera.position)), camDist);
			SetShaderValue(shdrRaymarch, marchLocs.camDir, &(camDir), SHADER_UNIFORM_VEC3);

		}

		void PreRender() {
			// Draw
			//----------------------------------------------------------------------------------
			// Draw into our custom render texture (framebuffer)
			BeginTextureMode(target);
			ClearBackground(WHITE);
			// Raymarch Scene
			EnableDepthTest(); //Manually enable Depth Test to handle multiple rendering methods.
			BeginShaderMode(shdrRaymarch);
			DrawRectangleRec({ 0, 0, (float)screenWidth, (float)screenHeight }, WHITE);
			EndShaderMode();
			// Rasterize Scene
			BeginMode3D(camera);
			BeginShaderMode(shdrRaster);
			DrawCubeWiresV( { 0.0f, 0.5f, 1.0f }, { 1.0f, 1.0f, 1.0f }, RED);
			DrawCubeV( { 0.0f, 0.5f, 1.0f },{ 1.0f, 1.0f, 1.0f }, PURPLE);
			DrawCubeWiresV({ 0.0f, 0.5f, -1.0f },  { 1.0f, 1.0f, 1.0f }, DARKGREEN);
			DrawCubeV( { 0.0f, 0.5f, -1.0f },  { 1.0f, 1.0f, 1.0f }, YELLOW);
			DrawGrid(10, 1.0f);
			EndShaderMode();
			EndMode3D();
			EndTextureMode();
		}

		void Render() {
			DrawTextureRec(target.texture, { 0, 0, (float)screenWidth, (float)-screenHeight },  { 0, 0 }, WHITE);
		}

		Camera camera;
		float camDist;
		RenderTexture2D target;
		Vector2 screenCenter;
		int screenWidth;
		int screenHeight;
		Shader shdrRaymarch;
		Shader shdrRaster;
		RayLocs marchLocs = { 0 };
	};
}