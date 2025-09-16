#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <gemo/TorusKnotGeometry.h>

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace GEMO;

namespace UNIT {
	class TorusKnot {
	public:
		const string BasePath = "E:/gl/resource/res/water/";
		const string RootPath = "E:/gl/resource/";
		TorusKnot() {}
		~TorusKnot() {}

		void LoadScene(int width, int height) {
			// Define the camera to look into our 3d world
			camera = { { 5.0f, 5.0f, 5.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
			// We generate a checked image for texturing
			Image checked = GenImageChecked(2, 2, 1, 1, RED, GREEN);
			Texture2D texture = LoadTextureFromImage(checked);
			UnloadImage(checked);
			string cstVs = RootPath + "shader/dissolve/dissolve.vs";
			string cstPs = RootPath + "shader/dissolve/dissolve.fs";
			cstShader = LoadShader(cstVs.c_str(), cstPs.c_str());
			InitShaderParam();
			model = LoadModelFromMesh(GenMeshKnot(1.0f, 2.0f, 16, 128));
			model.materials[0].shader = cstShader;
		}

		void ConifgShaderValue(Shader shader, string name, void* value, int type) {
			SetShaderValue(shader, GetShaderLocation(shader, name.c_str()), value, type);
		}

		void InitShaderParam() {
			ConifgShaderValue(cstShader, "uEdgeColor", &uEdgeColor, SHADER_UNIFORM_VEC3);
			ConifgShaderValue(cstShader, "uFreq", &uFreq, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(cstShader, "uAmp", &uAmp, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(cstShader, "uProgress", &uProgress, SHADER_UNIFORM_FLOAT);
			ConifgShaderValue(cstShader, "uEdge", &uEdge, SHADER_UNIFORM_FLOAT);
		}

		void Tick() {
			UpdateCamera(&camera, CAMERA_THIRD_PERSON);
		}

		void UpdateTexture(Texture texture,int textureIndex,string paramName) {
			ConifgShaderValue(cstShader, paramName,&textureIndex,SHADER_UNIFORM_INT);
			SetShaderValueTexture(cstShader,textureIndex,texture);
		}

		void Render() {
			BeginMode3D(camera);
			DrawModel(model, position, 1.0f, RED);
			EndMode3D();
		}

		Color uEdgeColor = GREEN;
		float uFreq = 0.25;
		float uAmp = 16.0;
		float uProgress = -0.7;
		float uEdge = 0.8;

		// Model drawing position
		Vector3 position = { 0.0f, 0.0f, 0.0f };
		Shader cstShader;
		Model model;
		Camera camera;
	};

}