#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <math/pr_math.h>
#include <random>
#include <core/text/draw_shape.h>

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace PMath;


/*
1️⃣ 离线烘焙 AO（Blender + Python 脚本）
• 仓库：https://github.com/adobe/blender-ao-baker
• 特点：一键把高模 AO 烘焙到低模，输出 8K 贴图；Python 脚本可嵌入 CI/CD。
• 运行：Blender 3.6LTS → File → Preferences → Add-ons → Install → 启用 AO Baker → N 面板一键烘焙。
• 适用：移动游戏、AR/VR 轻量场景。

2️⃣ 实时 SSAO（Unity URP 2022.3）
• 工程：https://github.com/Unity-Technologies/Graphics/tree/master/Packages/com.unity.render-pipelines.universal/Runtime/Passes/SSAO
• 特点：官方 SSAO 完整 Pass，含 HBAO、GTAO 两种算法；Shader 用 URP Shader Library，跨 Win/Mac/iOS/Android。
• 运行：Unity 2022.3 → Window → Package Manager → 添加 URP → 在 Renderer Feature 里勾选 SSAO 即可在 Scene 视图实时预览。
• 适用：PC/主机实时渲染、教学 Demo。

3️⃣ RTAO（DXR 1.1 样例，微软官方）
• 仓库：https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12Raytracing/src/D3D12RaytracingAO
• 特点：纯 DX12 + DXR 1.1，展示硬件加速光线追踪 AO；Shader 用 HLSL 6.6，附带 VS 2022 工程。
• 运行：Win11 + RTX 20 系列以上 → 双击 D3D12RaytracingAO.sln → F5 直接跑；可改 g_AORayCount 实时调质量。
• 适用：高端 PC、RTX GI 研究。
4️⃣ Compute Shader SSAO（OpenGL 4.6 + GLFW）
• 仓库：https://github.com/JoeyDeVries/SSAO
• 特点：LearnOpenGL 经典教程配套工程，GLSL Compute Shader 实现 Alchemy AO；CMake 跨平台。

*/


namespace BRDF {
	struct BufferG
	{
		unsigned int framebuffer;
		unsigned int positionTexture;
		unsigned int normalTexture;
		unsigned int albedoSpecTexture;
		unsigned int depthRenderbuffer;
	};

	struct FrameLayerout
	{
		unsigned int frameBuffer;
		unsigned int frameBufferTexture;
	};


	class AmbientUpdateParam : public EntryDebugger {
	public:
		AmbientUpdateParam() {
			name = "AmbientOcclusion";
		}
		virtual void InitEntry() {

			 static float pos[] = { 0.,0.,0. };
			if (ImGui::SliderFloat3("CubePos", pos,-30.0f, 30.0f)) {
				cubePosisiotn.x = pos[0];
				cubePosisiotn.y = pos[1];
				cubePosisiotn.z = pos[2];
			}
		}


		Vector3 cubePosisiotn = { 0.,0.,0. };
	};

	class AmbientOcclusion {
	public:
		const string RootPath = "E:/gl/resource/";

		float ourLerp(float a, float b, float f)
		{
			return a + f * (b - a);
		}

		void ConifgShaderValue(Shader shader, string name, void* value, int type) {
			SetShaderValue(shader, GetShaderLocation(shader, name.c_str()), value, type);
		}

		void LoadSecene(int screenWidth, int screenHeight) {
			width = screenWidth;
			height = screenHeight;
			EntryManager::i().PushDebugEntry(&param);
			camera.position = { 0.0f, 0.0f, 5.0f };    // Camera position
			camera.target = { 0.0f, 0.0f,-1.0f };      // Camera looking at point
			camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 45.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;
			string vsPath = RootPath + "shader/brdf/ssao_geometry.vs";
			string fsPath = RootPath + "shader/brdf/ssao_geometry.fs";
			geometry = LoadShader(vsPath.c_str(), fsPath.c_str());

			vsPath = RootPath + "shader/brdf/ssao.vs";
			fsPath = RootPath + "shader/brdf/ssao_lighting.fs";
			light = LoadShader(vsPath.c_str(), fsPath.c_str());

			vsPath = RootPath + "shader/brdf/ssao.vs";
			fsPath = RootPath + "shader/brdf/ssao.fs";
			ssao = LoadShader(vsPath.c_str(), fsPath.c_str());

			vsPath = RootPath + "shader/brdf/ssao.vs";
			fsPath = RootPath + "shader/brdf/ssao_blur.fs";
			ssaoBlur = LoadShader(vsPath.c_str(), fsPath.c_str());
			//std::uniform_real_distribution<GLfloat>
			string obj = RootPath + "obj/backpack/backpack.obj";
			backpack = LoadModel(obj.c_str());

			cube = LoadModelFromMesh(GenMeshCube(2.0f, 2.0f, 2.0f));
			cube.materials[0].shader = geometry;

			backpack.materials[0].shader = geometry;
			gBuffer.framebuffer = LoadFramebuffer();
			if (!gBuffer.framebuffer)
			{
				TraceLog(LOG_WARNING, "Failed to create framebuffer");
				exit(1);
			}
			EnableFramebuffer(gBuffer.framebuffer);
			gBuffer.positionTexture = LoadTexture(NULL, width, height, PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
			gBuffer.normalTexture = LoadTexture(NULL, width, height, PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
			gBuffer.albedoSpecTexture = LoadTexture(NULL, width, height, PL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
			// Activate the draw buffers for our framebuffer
			ActiveDrawBuffers(3);

			/* Frame Buffer Render To Attach*/
			FramebufferAttach(gBuffer.framebuffer, gBuffer.positionTexture, PL_ATTACHMENT_COLOR_CHANNEL0, PL_ATTACHMENT_TEXTURE2D, 0);
			FramebufferAttach(gBuffer.framebuffer, gBuffer.normalTexture, PL_ATTACHMENT_COLOR_CHANNEL1, PL_ATTACHMENT_TEXTURE2D, 0);
			FramebufferAttach(gBuffer.framebuffer, gBuffer.albedoSpecTexture, PL_ATTACHMENT_COLOR_CHANNEL2, PL_ATTACHMENT_TEXTURE2D, 0);
			// Finally we attach the depth buffer.
			gBuffer.depthRenderbuffer = LoadTextureDepth(screenWidth, screenHeight, true);
			FramebufferAttach(gBuffer.framebuffer, gBuffer.depthRenderbuffer, PL_ATTACHMENT_DEPTH, PL_ATTACHMENT_RENDERBUFFER, 0);
			// Make sure our framebuffer is complete.
			// NOTE: rlFramebufferComplete() automatically unbinds the framebuffer, so we don't have
			// to rlDisableFramebuffer() here.
			if (!FramebufferComplete(gBuffer.framebuffer))
			{
				TraceLog(LOG_WARNING, "Framebuffer is not complete");
			}

			ssaoFbo.frameBuffer = LoadFramebuffer();
			ssaoFbo.frameBufferTexture = LoadTexture(NULL, width, height, PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
			FramebufferAttach(ssaoFbo.frameBuffer, ssaoFbo.frameBufferTexture, PL_ATTACHMENT_COLOR_CHANNEL0, PL_ATTACHMENT_TEXTURE2D, 0);

			if (!FramebufferComplete(ssaoFbo.frameBuffer))
			{
				TraceLog(LOG_WARNING, "Framebuffer is not complete");
			}

			ssaoBlurFbo.frameBuffer = LoadFramebuffer();
			ssaoBlurFbo.frameBufferTexture = LoadTexture(NULL, width, height, PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
			FramebufferAttach(ssaoBlurFbo.frameBuffer, ssaoBlurFbo.frameBufferTexture, PL_ATTACHMENT_COLOR_CHANNEL0, PL_ATTACHMENT_TEXTURE2D, 0);
			if (!FramebufferComplete(ssaoBlurFbo.frameBuffer))
			{
				TraceLog(LOG_WARNING, "Framebuffer is not complete");
			}

			// generate sample kernel
			// ----------------------
			std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
			std::default_random_engine generator;
			
			for (unsigned int i = 0; i < 64; ++i) {
				Vector3 sample = { randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) };
			//	Normalize(sammple);
			//	Normal3f();
				sample = Vector3Normalize(sample);
				//sammple *= 
				sample *= randomFloats(generator);
				float scale = float(i) / 64.0f;
				scale = ourLerp(0.1f, 1.0f, scale * scale);
				sample *= scale;
				ssaoKernel.push_back(sample);
			}

			// generate noise texture
			// ----------------------
		   // generate noise texture
			// ----------------------
			std::vector<Vector3> ssaoNoise;
			for (unsigned int i = 0; i < 16; i++) {
				Vector3 noise = { randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f };
				ssaoNoise.push_back(noise);
			}
			noiseTexture = LoadTexture(&ssaoNoise[0],4,4, PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16,1);
			//Set Shader Param

			int aPostion = 0;
			int normal = 1;
			int gAbledo = 2;
			int ssaov = 3;
			//Light Shader
			ConifgShaderValue(light,"gPosition", &aPostion, SHADER_UNIFORM_INT);
			ConifgShaderValue(light,"gNormal", &normal, SHADER_UNIFORM_INT);
			ConifgShaderValue(light,"gAlbedo", &gAbledo, SHADER_UNIFORM_INT);
			ConifgShaderValue(light,"ssao", &ssaov, SHADER_UNIFORM_INT);

			int texNoise = 2;
			//ssao 
			ConifgShaderValue(ssao,"gPosition", &aPostion, SHADER_UNIFORM_INT);
			ConifgShaderValue(ssao,"gNormal", &normal, SHADER_UNIFORM_INT);
			ConifgShaderValue(ssao,"texNoise", &texNoise, SHADER_UNIFORM_INT);

			//ssaoBlur
			ConifgShaderValue(ssaoBlur, "ssaoInput", &aPostion, SHADER_UNIFORM_INT);

			model = MatrixIdentity();

			
			EnableDepthTest();
		}

		void Tick() {
			UpdateCamera(&camera, CAMERA_THIRD_PERSON);
			matView = MatrixLookAt(camera.position, camera.target, camera.up);
			float aspect = (float)CORE.Window.currentFbo.width / (float)CORE.Window.currentFbo.height;

			perspective = MatrixPerspective(camera.fovy, aspect, GetCullDistanceNear(), GetCullDistanceFar());
		}
		
		void Render() {
			//geometry
			EnableFramebuffer(gBuffer.framebuffer);
			ClearScreenBuffers();
			DisableColorBlend();
			BeginMode3D(camera);
			
			glm::mat4 md = glm::mat4(1.0);

			model = MatrixIdentity();
			//model = Matrix_Rotate(&model, { 0.0,1.0,0.0 }, deg2rad(180.0));
			
			model = Matrix_Translate(&model, param.cubePosisiotn);
			float cubeSle = 7.5;

			model = Matrix_Scale(&model, { cubeSle,cubeSle,cubeSle });
			SetShaderValueMatrix(geometry, GetShaderLocation(geometry, "view"), matView);
			//ssaoBlur
			//ConifgShaderValue(ssaoBlur, "ssaoInput", &model, SHADER_UNIFORM_VEC3);
			SetShaderValueMatrix(geometry, GetShaderLocation(geometry, "model"), model);
			//*PLGL.State.projerction
			//ConifgShaderValue(ssaoBlur, "ssaoInput", &model, SHADER_UNIFORM_VEC3);
			SetShaderValueMatrix(geometry, GetShaderLocation(geometry, "projection"), PLGL.State.projection);


			int invertedNormals = 1;

			ConifgShaderValue(geometry, "invertedNormals", &invertedNormals, SHADER_UNIFORM_INT);
			//EnableShader(geometry.id);
			DrawModel(cube, { 0.0, 1.0f, 0.0 }, 1.0f, WHITE);
		//	DisableShader();
		/*	for (int i = 0; i < MAX_CUBES; i++)
			{
				Vector3 position = cubePositions[i];
				DrawModelEx(cube, position, { 1, 1, 1 }, cubeRotations[i], { CUBE_SCALE, CUBE_SCALE, CUBE_SCALE }, WHITE);
			}*/
			//RenderSphere();
			int zoroNormals = 0;
			ConifgShaderValue(geometry, "invertedNormals", &zoroNormals, SHADER_UNIFORM_INT);
			model = MatrixIdentity();
			model = Matrix_Translate(&model, { 0.0,0.5,0.0 });

			model = Matrix_Scale(&model, { 1.,1.,1. });
			model = Matrix_Rotate(&model, { 1.0,0.0,0.0 }, deg2rad(-90.0));

			//ConifgShaderValue(ssaoBlur, "ssaoInput", &model, SHADER_UNIFORM_VEC3);
			SetShaderValueMatrix(geometry, GetShaderLocation(geometry, "model"), model);

			DrawModel(backpack, Vector3Zero(), 1.0f, WHITE);
		

			//DisableShader();
			EndMode3D();
			EnableColorBlend();

			//SetShaderValueMatrix();

			//ClearScreenBuffers(); // Clear color & depth buffer
			EnableFramebuffer(ssaoFbo.frameBuffer);
			//ClearScreenBuffers(); // Clear color & depth buffer
			glClear(GL_COLOR_BUFFER_BIT);

		//	BeginMode3D(camera);
		

			for (unsigned int i = 0; i < 64; ++i) {
				string paramName = "samples[" + std::to_string(i) + "]";
				//SetConfigFlags(ssao,);
				ConifgShaderValue(ssao,paramName,&ssaoKernel[i],SHADER_UNIFORM_VEC3);
			}
			//PLGL.State.projection;
			//*PLGL.State.projection
				//ConifgShaderValue(ssaoBlur, "ssaoInput", &model, SHADER_UNIFORM_VEC3);
			SetShaderValueMatrix(ssao, GetShaderLocation(ssao, "projection"), PLGL.State.projection);

		//	camera.projection;
			// and `gAlbedoSpec`
			EnableShader(ssao.id);
			ActiveTextureSlot(texUnitPosition);
			EnableTexture(gBuffer.positionTexture);
			ActiveTextureSlot(texUnitNormal);
			EnableTexture(gBuffer.normalTexture);
			ActiveTextureSlot(texUnitAlbedoSpec);
			EnableTexture(noiseTexture);

			// Finally, we draw a fullscreen quad to our default framebuffer
			// This will now be shaded using our deferred shader

			LoadDrawQuad();

			
		//	EndMode3D();
			//BindFramebuffer(PL_READ_FRAMEBUFFER, gBuffer.framebuffer);
			//BindFramebuffer(PL_DRAW_FRAMEBUFFER, 0);
			//BlitFramebuffer(0, 0, width, height, 0, 0, width, height, 0x00000100);    // GL_DEPTH_BUFFER_BIT
		
			DisableFramebuffer();


			EnableFramebuffer(ssaoBlurFbo.frameBuffer);
			glClear(GL_COLOR_BUFFER_BIT);
		//	ClearScreenBuffers(); // Clear color & depth buffer
			BeginMode3D(camera);
			EnableShader(ssaoBlur.id);
			ActiveTextureSlot(texUnitPosition);
			EnableTexture(ssaoFbo.frameBufferTexture);
			// This will now be shaded using our deferred shader
			LoadDrawQuad();

			
			EndMode3D();


		
			DisableFramebuffer();

			ClearScreenBuffers();

			BeginMode3D(camera);
			
			Vector4 lPos = {lightPos.x,lightPos.y,lightPos.z,0.0};

			
			Vector4 res =Vector4Transform(lPos,matView);
			Vector3 lightPosView = {res.x,res.y,res.z};
			ConifgShaderValue(light, "light.Position", &lightPosView, SHADER_UNIFORM_VEC3);
			ConifgShaderValue(light, "light.Color", &lightColor, SHADER_UNIFORM_VEC3);
			// Update attenuation parameters
			float linear = 0.09f;
			float quadratic = 0.032f;
			ConifgShaderValue(light, "light.Linear", &linear, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(light, "light.Quadratic", &quadratic, SHADER_UNIFORM_FLOAT);


			EnableShader(light.id);
			// and `gAlbedoSpec`
			ActiveTextureSlot(texUnitPosition);
			EnableTexture(gBuffer.positionTexture);
			ActiveTextureSlot(texUnitNormal);
			EnableTexture(gBuffer.normalTexture);
			ActiveTextureSlot(texUnitAlbedoSpec);
			EnableTexture(gBuffer.albedoSpecTexture);
			ActiveTextureSlot(blurTextureSlot);
			EnableTexture(ssaoBlurFbo.frameBufferTexture);
			LoadDrawQuad();

			DisableShader();
			EndMode3D();
		}


		void RenderSphere() {
			//BeginShaderMode(geometry);
			//BeginMode3D(camera);
			DrawSphere(enemySpherePos, enemySphereSize, BLACK);
			//DrawCube();
			//DrawSphereWires(enemySpherePos, enemySphereSize, 16, 64,DARKGRAY);
			//EndMode3D();
			//EndShaderMode();
		}
		 bool isSet = false;


		 const float CUBE_SCALE = 0.25;
		 
		AmbientUpdateParam param;

		Camera camera = { { 0.0f,0.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };

		float enemySphereSize = 1.0f;
		Vector3 enemySpherePos = { 4.0f, 0.0f, 0.0f };


		std::vector<Vector3> ssaoKernel;
		float cubeRotations[MAX_CUBES] = { 0 };

		Vector3 cubePositions[MAX_CUBES] = { 0 };

		Matrix model;

		Vector3 lightPos = {2.0,4.0,-2.0};
		Vector3 lightColor = {1.0,0.,0. };

		FrameLayerout ssaoFbo;
		FrameLayerout ssaoBlurFbo;
		unsigned int noiseTexture;

		BufferG gBuffer;
		Model backpack;
		Shader geometry;
		Shader light;
		Shader ssao;
		Shader ssaoBlur;
		float width, height;

		Model cube;
		Matrix matView;
		Matrix perspective;

		int texUnitPosition = 0;
		int texUnitNormal = 1;
		int texUnitAlbedoSpec = 2;
		int blurTextureSlot = 3;
	};

}