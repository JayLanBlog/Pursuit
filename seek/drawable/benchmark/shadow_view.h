#pragma once

#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <section/enum/config_em.h>
#include <file/sys_text.h>
#include <math/pr_math.h>

using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace Loader;
using namespace Text;
using namespace PMath;
using namespace RHICore;
#define SHADOWMAP_RESOLUTION 1024
namespace MarkView {
	class ShadowView {
	public:

		ShadowView() {}
		~ShadowView() {}

		void EnablePiplineState() {
			SetConfigFlags(FLAG_MSAA_4X_HINT);
		}

		void LoaderShaderIn() {
			shadowShader = LoadShader("resource/res/pr/shaders/shadowmap.vs","resource/res/pr/shaders/shadowmap.fs");
			shadowShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shadowShader, "viewPos");
			lightDir = Vector3Normalize({ 0.35f, -1.0f, -0.35f });
			Color lightColor = WHITE;
			lightColorNormalized = ColorNormalize(lightColor);
			lightDirLoc = GetShaderLocation(shadowShader, "lightDir");
			int lightColLoc = GetShaderLocation(shadowShader, "lightColor");
			SetShaderValue(shadowShader, lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);
			SetShaderValue(shadowShader, lightColLoc, &lightColorNormalized, SHADER_UNIFORM_VEC4);
			int ambientLoc = GetShaderLocation(shadowShader, "ambient");
			float ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			SetShaderValue(shadowShader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);
			lightVPLoc = GetShaderLocation(shadowShader, "lightVP");
			 shadowMapLoc = GetShaderLocation(shadowShader, "shadowMap");
			int shadowMapResolution = SHADOWMAP_RESOLUTION;
			SetShaderValue(shadowShader, GetShaderLocation(shadowShader, "shadowMapResolution"), &shadowMapResolution, SHADER_UNIFORM_INT);
		}

		void ModelLoader() {
			cube = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
			cube.materials[0].shader = shadowShader;
			robot = LoadModel("resource/res/pr/robot.glb");
			for (int i = 0; i < robot.materialCount; i++)
			{
				robot.materials[i].shader = shadowShader;
			}
			robotAnimations = LoadModelAnimations("resource/res/pr/robot.glb", &animCount);
			shadowMap = LoadShadowmapRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
			lightCam.position = Vector3Scale(lightDir, -15.0f);
			lightCam.target = Vector3Zero();
			// Use an orthographic projection for directional lights
			lightCam.projection = CAMERA_ORTHOGRAPHIC;
			lightCam.up = { 0.0f, 1.0f, 0.0f };
			lightCam.fovy = 20.0f;
		}

		void Tick() {
			 dt = GetFrameTime();
			Vector3 cameraPos = cam.position;
			SetShaderValue(shadowShader, shadowShader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos, SHADER_UNIFORM_VEC3);
			UpdateCamera(&cam, CAMERA_ORBITAL);

			fc++;
			fc %= (robotAnimations[0].frameCount);
			UpdateModelAnimation(robot, robotAnimations[0], fc);
			ProcessKeyEvent();

			lightDir = Vector3Normalize(lightDir);
			lightCam.position = Vector3Scale(lightDir, -15.0f);
			SetShaderValue(shadowShader, lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);
		}

		void Draw() {
			Matrix lightView;
			Matrix lightProj;
			BeginTextureMode(shadowMap);
			ClearBackground(WHITE);
			BeginMode3D(lightCam);
			lightView = GetMatrixModelview();
			lightProj = GetMatrixProjection();
			DrawScene(cube, robot);
			EndMode3D();
			EndTextureMode();
			Matrix lightViewProj = MatrixMultiply(lightView, lightProj);

			ClearBackground(RAYWHITE);

			SetShaderValueMatrix(shadowShader, lightVPLoc, lightViewProj);

			EnableShader(shadowShader.id);
			int slot = 10; // Can be anything 0 to 15, but 0 will probably be taken up
			ActiveTextureSlot(10);
			EnableTexture(shadowMap.depth.id);
			SetUniform(shadowMapLoc, &slot, SHADER_UNIFORM_INT, 1);

			BeginMode3D(cam);

			// Draw the same exact things as we drew in the shadowmap!
			DrawScene(cube, robot);

			EndMode3D();

		}
		void DrawScene(Model cube, Model robot)
		{
			DrawModelEx(cube, Vector3Zero(), { 0.0f, 1.0f, 0.0f }, 0.0f,  { 10.0f, 1.0f, 10.0f }, BLUE);
			DrawModelEx(cube,{ 1.5f, 1.0f, -1.5f },{ 0.0f, 1.0f, 0.0f }, 0.0f, Vector3One(), WHITE);
			DrawModelEx(robot, { 0.0f, 0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f }, 0.0f, { 1.0f, 1.0f, 1.0f }, RED);
		}

		void ProcessKeyEvent() {
			if (IsKeyDown(KEY_LEFT))
			{
				if (lightDir.x < 0.6f)
					lightDir.x += cameraSpeed * 60.0f * dt;
			}
			if (IsKeyDown(KEY_RIGHT))
			{
				if (lightDir.x > -0.6f)
					lightDir.x -= cameraSpeed * 60.0f * dt;
			}
			if (IsKeyDown(KEY_UP))
			{
				if (lightDir.z < 0.6f)
					lightDir.z += cameraSpeed * 60.0f * dt;
			}
			if (IsKeyDown(KEY_DOWN))
			{
				if (lightDir.z > -0.6f)
					lightDir.z -= cameraSpeed * 60.0f * dt;
			}
		}
		
		RenderTexture2D LoadShadowmapRenderTexture(int width, int height) {
			RenderTexture2D target = { 0 };
			target.id = LoadFramebuffer(); // Load an empty framebuffer
			target.texture.width = width;
			target.texture.height = height;
			if (target.id > 0)
			{
				EnableFramebuffer(target.id);
				// Create depth texture
				// We don't need a color texture for the shadowmap
				target.depth.id = LoadTextureDepth(width, height, false);
				target.depth.width = width;
				target.depth.height = height;
				target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
				target.depth.mipmaps = 1;
				// Attach depth texture to FBO
				FramebufferAttach(target.id, target.depth.id, PL_ATTACHMENT_DEPTH, PL_ATTACHMENT_TEXTURE2D, 0);
				// Check if fbo is complete with attachments (valid)
				if (FramebufferComplete(target.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);
				DisableFramebuffer();
			}
			else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

			return target;
		}

		void CreateCamera() {
			cam.position ={ 10.0f, 10.0f, 10.0f };
			cam.target = Vector3Zero();
			cam.projection = CAMERA_PERSPECTIVE;
			cam.up ={ 0.0f, 1.0f, 0.0f };
			cam.fovy = 45.0f;
		}

		// Unload shadowmap render texture from GPU memory (VRAM)
		void UnloadShadowmapRenderTexture(RenderTexture2D target)
		{
			if (target.id > 0)
			{
				// NOTE: Depth texture/renderbuffer is automatically
				// queried and deleted before deleting framebuffer
				UnloadFramebuffer(target.id);
			}
		}


		void CreateAndInitialize() {
			CreateCamera();
			LoaderShaderIn();
			ModelLoader();

		}
	private:
		int lightVPLoc; 
		int shadowMapLoc;
		int lightDirLoc;
		float dt = 0.0;
		const float cameraSpeed = 0.05f;
		Model robot;
		Model cube;
		int fc = 0;
		int animCount = 0;
		ModelAnimation* robotAnimations;
		RenderTexture2D shadowMap;
		// For the shadowmapping algorithm, we will be rendering everything from the light's point of view
		Camera3D lightCam = { 0 };
		Camera3D cam = { 0 };
		Shader shadowShader;
		Vector3 lightDir;
		Vector4 lightColorNormalized;
	};
}
