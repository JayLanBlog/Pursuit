#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <math/pr_math.h>
#include <drawable/benchmark/sky_sample.h>
#include "menu/entry_manager.h"
using namespace MView;
using namespace Seek::Menu;
namespace Water {

	class ActorUpdateParam : public EntryDebugger {
	public:
		ActorUpdateParam() {
			name = "GerstnerWave";

		}

		virtual void InitEntry() {
			//ImGui::SliderFloat("size", &size, 0.0f, 1.0f);
			if (ImGui::SliderInt("WaveCount", &WaveCount, 0, 100)) {
			
			}
			if (ImGui::SliderFloat("Amplitude", &Amplitude, 0.0f, 1.0f)) {

			}
			if (ImGui::SliderFloat("Frequency", &Frequency, 0.0f, 1.0f)) {

			}
			if (ImGui::SliderFloat("Steepness", &Steepness, 0.0f, 1.0f)) {

			}
			if (ImGui::SliderFloat("MaxAngle", &MaxAngle, 0.0f, 360.0f)) {

			}
			float waveDir[] = { -0.60,0.5 };
			if (ImGui::SliderFloat2("waveDir", waveDir, 0.0f, 1.0f)) {
				WaveDirection.x = waveDir[0];
				WaveDirection.y = waveDir[1];
			}

	/*		float duffes[] = { 0.3,0.61,0.71 };

			if (ImGui::SliderFloat3("DiffuseCol", duffes, 0.0f, 360.0f)) {

			}
			float Ambient[] = { 0.3,0.61,0.71 };

			if (ImGui::SliderFloat3("AmbientCol", Ambient, 0.0f, 360.0f)) {

			}
			float Speculat[] = { 0.3,0.61,0.71 };

			if (ImGui::SliderFloat3("SpeculatCol", Speculat, 0.0f, 360.0f)) {

			}*/

			size.y = 400;
		}

		int WaveCount = 32;
		//水波参数:
		float Amplitude = 0.2f;
		float Frequency = 0.5f;
		float Steepness = 0.7f;
		float MaxAngle = 360.0f;
		Vector3 DiffuseCol = { 0.3,0.61,0.71 };
		Vector3 AmbientCol = { 0.0,0.4,0.5 };
		Vector3 SpeculatCol = { 1.0 ,0.27,0.0 };
		//光照参数
		float Shininess = 64.0f;
		float SpecStrength = 0.7f;
		float SpecFresnelIntensity = 0.2f;
		float FoamHeight = 0.5f;

		float SSS_Distortion = 0.2f;
		float SSS_Scale = 0.4f;
		float SSS_Power = 8.0f;
		float SSS_Dist = 50.0f;
		float SSS_Strength = 0.5f;
		float SunHeight = 0.1f;

		Vector2 SunDir = { 0.0,-1.0 };
		Vector2 WaveDirection = { -0.60,0.5 };
	};

	struct FrameBufferScene
	{
		unsigned int  frameBuffer;
		unsigned int depthBuffer;
		unsigned int texture;
		int m_width, m_height;
		FrameBufferScene();
		FrameBufferScene(int width, int height) {
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
	
	class GerstnerWave {

	public:
		const string RootPath = "E:/gl/resource/";
		GLCubeView skyCube;
	//	FrameBufferScene fbo;
		GerstnerWave() {}
		~GerstnerWave(){}
		void LoadScene(int screanWidth, int screenHeight) {
			camera.position = { 0.0f, 5.0f, 10.0f }; // Camera position
			camera.target = { 0.0f, 10.0f, 0.0f };     // Camera looking at point
			camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 45.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;
			string url = RootPath + "obj/gems1/water/plane.obj";
			plane = Loader::LoadModel(url.c_str());
			width = screanWidth;
			height = screanWidth;
			skyCube.LoadScene(width, height);
	
			string skyVsPath = RootPath + "shader/water/gems/gerstner.vs";
			string skyPsPath = RootPath + "shader/water/gems/gerstner.fs";
			waterShader = LoadShader(skyVsPath.c_str(), skyPsPath.c_str());
			//SetShaderValue(shader, GetShaderLocation(skyBoxShader, "skybox"), &textureIndex, SHADER_UNIFORM_UINT);
			int cubeIndex = MATERIAL_MAP_CUBEMAP;
			ConifgShaderValue(waterShader, "skybox", &cubeIndex, SHADER_UNIFORM_INT);
			ConfigShaderParamaters();

			plane.materials[0].shader = waterShader;                 // Set shader effect to 3d model
			
			Texture cubeTexture = { skyCube.cubemapTexture };
			plane.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubeTexture; // Bind texture to model
			model = MatrixIdentity();
			EntryManager::i().PushDebugEntry(&param);
		}

		void ConifgShaderValue(Shader shader,string name,void * value , int type) {
			SetShaderValue(shader, GetShaderLocation(shader,name.c_str()), value,type);
		}

		void ConfigShaderParamaters() {
			ConifgShaderValue(waterShader,"WaveCount",&param.WaveCount,SHADER_UNIFORM_INT);
			ConifgShaderValue(waterShader, "Amp", &param.Amplitude, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "Frequency", &param.Frequency, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "Steepness", &param.Steepness, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "MAX_ANGLE", &param.MaxAngle, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "Direction", &param.WaveDirection, SHADER_UNIFORM_VEC2);
			ConifgShaderValue(waterShader, "_AmbientCol", &param.AmbientCol, SHADER_UNIFORM_VEC3);
			ConifgShaderValue(waterShader, "_DiffuseCol", &param.DiffuseCol, SHADER_UNIFORM_VEC3);
			ConifgShaderValue(waterShader, "_SpecularCol", &param.SpeculatCol, SHADER_UNIFORM_VEC3);

			ConifgShaderValue(waterShader, "Shininess", &param.Shininess, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "SpecStrength", &param.SpecStrength, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "SpecFresnelIntensity", &param.SpecFresnelIntensity, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "FoamHeight", &param.FoamHeight, SHADER_UNIFORM_FLOAT);
			//ConifgShaderValue(waterShader, "SSS_Distortion", &SSS_Distortion, SHADER_UNIFORM_FLOAT);

			ConifgShaderValue(waterShader, "SSS_Distortion", &param.SSS_Distortion, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "SSS_Strength", &param.SSS_Strength, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "SSS_Power", &param.SSS_Power, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "SSS_Dist", &param.SSS_Dist, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "SSS_Scale", &param.SSS_Scale, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "SunHeight", &param.SunHeight, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(waterShader, "SunDirXZ", &param.SunDir, SHADER_UNIFORM_VEC2);

			SetShaderValueMatrix(waterShader, GetShaderLocation(waterShader,"model"),model);
		}

		void DrawWater() {
			//skyCube.cubemapTexture
		}

		void Tick() {
			UpdateCamera(&camera, CAMERA_ORBITAL);
			matView = MatrixLookAt(camera.position, camera.target, camera.up);
			// glm::lookAt(camera.position, camera.position + camera.target, camera.up);
			//glm::mat4 view = camera.GetViewMatrix();
			//glm::mat4 projection = camera.projection;
			float deltaTime = GetFrameTime();
			u_time += deltaTime;
			ConifgShaderValue(waterShader,"Time",&u_time, SHADER_UNIFORM_FLOAT);

			Vector3 viewPos = {6.28283,13.44041,8.58434};
			ConifgShaderValue(waterShader, "viewPos", &viewPos, SHADER_UNIFORM_VEC3);
			ConfigShaderParamaters();
			matProj = MatrixIdentity();
			matProj = MatrixPerspective(camera.fovy * DEG2RAD, ((double)width / (double)height), GetCullDistanceNear(), GetCullDistanceFar());
			skyCube.Tick();
		}

		void Render() {
			skyCube.Render();
			BeginMode3D(camera);
			float scale = 10.f;
			DrawModelEx(plane, { 10.0f,10.0f, 10.0f }, { 0.0f,1.0f,0.0f }, 0.0f, { scale,scale ,scale }, WHITE);
			EndMode3D();
			//skyCube.Tick();
		}
		/*
		* IF DO NeeD 
		*/

		Matrix model;
		Matrix matView;
		Matrix matProj;


		Shader waterShader;
		Model plane;
		int width = 0, height = 0;
		Camera camera = { 0 };
		
		ActorUpdateParam  param;

		//int WaveCount = 32;
		////水波参数:
		//float Amplitude = 0.2f;
		//float Frequency = 0.5f;
		//float Steepness = 0.7f;
		//float MaxAngle = 360.0f;
		//Vector3 DiffuseCol = {0.3,0.61,0.71};
		//Vector3 AmbientCol = { 0.0,0.4,0.5 };
		//Vector3 SpeculatCol = { 1.0 ,0.27,0.0 };


		////光照参数
		//float Shininess = 64.0f;
		//float SpecStrength = 0.7f;
		//float SpecFresnelIntensity = 0.2f;
		//float FoamHeight = 0.5f;

		//float SSS_Distortion = 0.2f;
		//float SSS_Scale = 0.4f;
		//float SSS_Power = 8.0f;
		//float SSS_Dist = 50.0f;
		//float SSS_Strength = 0.5f;
		//float SunHeight = 0.1f;

		//Vector2 SunDir = {0.0,-1.0};
		//Vector2 WaveDirection = { -0.60,-0.5 };

		float u_time;

		//int WaveCountLoc;
		//int modelLoc;
		////光照参数的shader 定位
		//int AmplitudeLoc;
		//int FrequencyLoc;
		//int SteepnessLoc;
		//int MaxAngleLoc;
		//int DiffuseColLoc;
		//int AmbientColoc;
		//int SpeculatColLoc;

		//int ShininessLoc;
		//int SpecStrengthLoc;
		//int SpecFresnelIntensityLoc;
		//int FoamHeightLoc;

		//int SSS_DistortionLoc;
		//int SSS_ScaleLoc;
		//int SSS_PowerLoc;
		//int SSS_DistLoc;
		//int SSS_StrengthLoc;
		//int SunHeightLoc;
		//int sunDirLoc;
		//int WaveDirectionLoc;



	};

}