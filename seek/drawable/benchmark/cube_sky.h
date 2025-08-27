#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
namespace MView {
	class CubeSkyView {
	public:
		const string BasePath = "E:/gl/resource/res/water/";
		const string RootPath = "E:/gl/resource/";
		CubeSkyView() { }
		~CubeSkyView() { }

		void LoadScene() {
		
			camera.position = { 0.0f, 0.0f, 1.0f };    // Camera position
			camera.target ={ 0.0f, 0.0f, 4.0f };      // Camera looking at point
			camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 45.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;
	
			cube = GenMeshCube(1.0f, 1.0f, 1.0f);
			skyMehs = GenMeshCube(2.0f, 2.0f, 2.0f);

			string cubePath = BasePath +"container.jpg";
			cubeTexture = LoadTexture(cubePath.c_str());
			cubeModel = LoadModelFromMesh(cube);

			skyModel = LoadModelFromMesh(skyMehs);

			string skyVsPath = RootPath + "shader/water/gems/cubemap.vs";
			string skyPsPath = RootPath + "shader/water/gems/cubemap.fs";
			cubeShader = LoadShader(skyVsPath.c_str(), skyPsPath.c_str());

			string vsPath = RootPath + "shader/water/gems/cubesky.vs";
			string fsPath = RootPath + "shader/water/gems/cubesky.fs";
			skyShader = LoadShader(vsPath.c_str(), fsPath.c_str());
	
			std::vector<Image> skyTextures;
			for (string imgUrl : faces) {
				skyTextures.push_back(LoadImageSource(imgUrl.c_str()));
			}
			
			cubeSkyTexture = LoadCubeTexture(skyTextures, 0);
			   // CUBEMAP_LAYOUT_PANORAMA

			for (Image img : skyTextures) {
				UnloadImage(img);
			}

			unsigned int tdex = 0;
			unsigned int sdex = 7;
			SetShaderValue(cubeShader, GetShaderLocation(cubeShader, "texture1"),&tdex,SHADER_UNIFORM_UINT);
			SetShaderValue(skyShader, GetShaderLocation(skyShader, "skybox"),&sdex, SHADER_UNIFORM_INT);


			cubeModel.materials[0].shader = cubeShader;
			cubeModel.materials[0].maps[0].texture = cubeTexture;

			cubeSkyTexture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
			skyModel.materials[0].shader = skyShader;
			skyModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubeSkyTexture;



			//cubeModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubeTexture;
		}
		

		void Tick() {
			UpdateCamera(&camera, CAMERA_THIRD_PERSON);
		}


		void Render() {
			//DrawMesh();
			BeginMode3D(camera);
			/*
			  Vector3 vScale = { scale, scale, scale };
			  Vector3 rotationAxis = { 0.0f, 1.0f, 0.0f };
			  DrawModelEx(model, position, rotationAxis, 0.0f, vScale, tint);
			*/
			//float scale = 0.5f;
			DisableBackfaceCulling();
			DisableDepthMask();
			DrawModel(skyModel, { 0, 0, 0 }, 1.0f, WHITE);
			EnableBackfaceCulling();
			EnableDepthMask();
			//DrawModelEx(plane, { 10.0f,10.0f, 10.0f }, { 0.0f,1.0f,0.0f }, 0.0f, { scale,scale ,scale }, WHITE);
			EndMode3D();
		}

		Mesh cube;

		Mesh skyMehs;
		std::vector<string> faces
		{
				BasePath + "gems1/sky/right.jpg",
				BasePath + "gems1/sky/left.jpg",
				BasePath + "gems1/sky/top.jpg",
				BasePath + "gems1/sky/bottom.jpg",
				BasePath + "gems1/sky/front.jpg",
				BasePath + "gems1/sky/back.jpg",
		};
		
		

		Shader skyShader;

		Shader cubeShader;

		Model cubeModel;


		Model skyModel;
		

		Texture cubeSkyTexture;
		
		Camera camera = { 0 };
		
		Texture cubeTexture;
	};
}