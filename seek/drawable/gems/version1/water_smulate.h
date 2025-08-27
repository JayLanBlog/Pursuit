#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>

using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace Loader;
using namespace System;

namespace Smulate {

	struct FrameBufferScene
	{
		unsigned int  frameBuffer;
		unsigned int depthBuffer;
		unsigned int texture;
		int m_width, m_height;
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


	class WaterSmulater {
	public:	
		WaterSmulater() {}
		~WaterSmulater() {}
		const string BasePath = "E:/gl/resource/res/water/gems1/";
		const string RootPath = "E:/gl/resource/";

	

		void LoadScene() {
			camera.position = { 50.0f, 50.0f, 50.0f }; // Camera position
			camera.target = { 0.0f, 10.0f, 0.0f };     // Camera looking at point
			camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 45.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;
			plane = Loader::LoadModel("resource/obj/gems1/water/plane.obj");
		
			//texture = LoadTexture("castle_diffuse.png");
			//model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;   
			// Set map diffuse texture
			string skyVsPath = RootPath+"shader/water/gems/cubemap.vs";
			string skyPsPath = RootPath + "shader/water/gems/cubemap.fs";
			skyShader = LoadShader(skyVsPath.c_str(), skyPsPath.c_str());
			cube = GenMeshCube(1.0f, 1.0f, 1.0f);
			skybox = LoadModelFromMesh(cube);
			//waterShader = LoadShader("resource/shader/water/gems1/gerstner.vs", "resource/shader/water/gems1/gerstner.fs");
			//LoadTextureCubemap();
			//LoadTextureCubemap();
			std::vector<string> faces
			{
					BasePath + "sky/right.jpg",
					BasePath + "sky/left.jpg",
					BasePath + "sky/top.jpg",
					BasePath + "sky/bottom.jpg",
					BasePath + "sky/front.jpg",
					BasePath + "sky/back.jpg",
			};

			std::vector<Image> skyTextures;
			for (string imgUrl: faces) {
				skyTextures.push_back(LoadImageSource(imgUrl.c_str()));
			}
			
			skyTexture = LoadTextureCubemap(skyTextures,1);

			for (Image img : skyTextures) {
				UnloadImage(img);
			}
			
			skybox.materials[0].shader = skyShader;
			skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = { skyTexture };
			//CubeMapTexture
		//	CubeMapTextureLoc = GetShaderLocation(skybox.materials[0].shader,"CubeMapTexture");
			//SetShaderParamter();
		}


		void SetShaderParamter() {
			SetShaderValue(skybox.materials[0].shader, CubeMapTextureLoc,&skyTexture,SHADER_UNIFORM_INT);
		}

		void Tick() {
			UpdateCamera(&camera, CAMERA_THIRD_PERSON);
		}

		void Render() {
			BeginMode3D(camera);
			/*
			  Vector3 vScale = { scale, scale, scale };
			  Vector3 rotationAxis = { 0.0f, 1.0f, 0.0f };
			  DrawModelEx(model, position, rotationAxis, 0.0f, vScale, tint);
			*/
			//float scale = 0.5f;
			DisableBackfaceCulling();
			DisableDepthMask();
			DrawModel(skybox, { 0, 0, 0 }, 1.0f, WHITE);
			EnableBackfaceCulling();
			EnableDepthMask();
			//DrawModelEx(plane, { 10.0f,10.0f, 10.0f }, { 0.0f,1.0f,0.0f }, 0.0f, { scale,scale ,scale }, WHITE);
			EndMode3D();
		}


		void RenderCube() {
		
		}

		Mesh cube;
		Model skybox;

		Shader skyShader;
		Shader waterShader;
		Shader modelShader;


		Model plane;
		Camera camera = { 0 };
		Texture2D texture;
		Vector3 position = { 0.0f, 0.0f, 0.0f };

		unsigned int skyTexture;
		//Shader Location
		int CubeMapTextureLoc;

	};
}