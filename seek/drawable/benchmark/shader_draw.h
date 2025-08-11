#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <section/enum/config_em.h>
#include <file/sys_text.h>
#include <core/pass/draw_pass.h>
#include <core/pass/draw_pass2d.h>
#include <core/text/draw_text.h>
#include <math/pr_math.h>
#include <core/text/draw_shape.h>
using namespace PMath;

using namespace PiplineState;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace Loader;
using namespace Text;
typedef struct {
	unsigned int camPos, camDir, screenCenter;
}RayLocs;

namespace MarkView {
	class ShaderHibrid {
	public:
		ShaderHibrid() {}
		~ShaderHibrid() {}

		RenderTexture2D LoadRenderTextureDepthTex(int wd, int ht) {
			RenderTexture2D target = { 0 };
			target.id = LoadFramebuffer(); // Load an empty framebuffer
			if (target.id > 0)
			{
				EnableFramebuffer(target.id);
				width = wd;
				height = ht;
				// Create color texture (default to RGBA)
				target.texture.id = LoadTexture(0, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
				target.texture.width = width;
				target.texture.height = height;
				target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
				target.texture.mipmaps = 1;

				// Create depth texture buffer (instead of raylib default renderbuffer)
				target.depth.id = LoadTextureDepth(width, height, false);
				target.depth.width = width;
				target.depth.height = height;
				target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
				target.depth.mipmaps = 1;
				// Attach color texture and depth texture to FBO
				FramebufferAttach(target.id, target.texture.id, PL_ATTACHMENT_COLOR_CHANNEL0, PL_ATTACHMENT_TEXTURE2D, 0);
				FramebufferAttach(target.id, target.depth.id, PL_ATTACHMENT_DEPTH, PL_ATTACHMENT_TEXTURE2D, 0);
				// Check if fbo is complete with attachments (valid)
				if (FramebufferComplete(target.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);
				DisableFramebuffer();
			}
			else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

			return target;
		}

		void Tick() {
			// Update
			//----------------------------------------------------------------------------------
			UpdateCamera(&camera, CAMERA_ORBITAL);
			// Update Camera Postion in the ray march shader.
			SetShaderValue(shdrRaymarch, marchLocs.camPos, &(camera.position), PL_SHADER_UNIFORM_VEC3);
			// Update Camera Looking Vector. Vector length determines FOV.
			Vector3 camDir = Vector3Scale(Vector3Normalize(Vector3Subtract(camera.target, camera.position)), camDist);
			SetShaderValue(shdrRaymarch, marchLocs.camDir, &(camDir), PL_SHADER_UNIFORM_VEC3);
			//----------------------------------------------------------------------------------
		}

		void TextureModelRender() {
			BeginTextureMode(target);
			  ClearBackground(WHITE);
			  // Raymarch Scene
			  EnableDepthTest(); //Manually enable Depth Test to handle multiple rendering methods.
			  BeginShaderMode(shdrRaymarch);
			  DrawRectangleRec( { 0, 0, (float)width, (float)height }, WHITE);
			  EndShaderMode();
			  // Rasterize Scene
			  BeginMode3D(camera);
			  BeginShaderMode(shdrRaster);
			  DrawCubeWiresV( { 0.0f, 0.5f, 1.0f }, { 1.0f, 1.0f, 1.0f }, RED);
			  DrawCubeV( { 0.0f, 0.5f, 1.0f }, { 1.0f, 1.0f, 1.0f }, PURPLE);
			  DrawCubeWiresV({ 0.0f, 0.5f, -1.0f }, { 1.0f, 1.0f, 1.0f }, DARKGREEN);
			  DrawCubeV({ 0.0f, 0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f }, YELLOW);
			  DrawGrid(10, 1.0f);
			  EndShaderMode();
			  EndMode3D();
			EndTextureMode();
		}

		void Render() {
			DrawTextureRec(target.texture, { 0, 0, (float)width, (float)-height },  { 0, 0 }, WHITE);
		}

		void LoaderScene(const int screenWidth, const int screenHeight) {
			shdrRaymarch = LoadShader(0, "resource/res/pr/shaders/hybrid_raymarch.fs");
			shdrRaster = LoadShader(0, "resource/res/pr/shaders/hybrid_raster.fs");
			// Fill the struct with shader locs.
			marchLocs.camPos = GetShaderLocation(shdrRaymarch, "camPos");
			marchLocs.camDir = GetShaderLocation(shdrRaymarch, "camDir");
			marchLocs.screenCenter = GetShaderLocation(shdrRaymarch, "screenCenter");
			// Transfer screenCenter position to shader. Which is used to calculate ray direction. 
			Vector2 screenCenter = {  screenWidth / 2.0f, screenHeight / 2.0f };
			SetShaderValue(shdrRaymarch, marchLocs.screenCenter, &screenCenter, SHADER_UNIFORM_VEC2);
			target = LoadRenderTextureDepthTex(screenWidth, screenHeight);
			camera = { { 0.5f, 1.0f, 1.5f },   // Camera position
					   { 0.0f, 0.5f, 0.0f },   // Camera looking at point
					   { 0.0f, 1.0f, 0.0f },   // Camera up vector (rotation towards target)
							45.0f,             // Camera field-of-view Y
						CAMERA_PERSPECTIVE     // Camera projection type
			};
			// Camera FOV is pre-calculated in the camera Distance.
			camDist = 1.0f / (tanf(camera.fovy * 0.5f * DEG2RAD));	
		}


		float width, height;
		float camDist;
		Camera camera;
		RayLocs marchLocs = { 0 };
		RenderTexture2D target;
		Shader shdrRaymarch;
		Shader shdrRaster;
	};
}