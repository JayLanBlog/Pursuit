#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <math/pr_math.h>
#include <gemo/TorusKnotGeometry.h>

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace PMath;
using namespace GEMO;


namespace BRDF {
	class Fog {
	public:
		const string BasePath = "E:/gl/resource/res/water/";
		const string RootPath = "E:/gl/resource/";
		Fog() {}
		~Fog() {}

		void LoadScene(int screenWidth, int screenHeight) {

			camera = { 0 };
			camera.position = { 2.0f, 2.0f, 6.0f };    // Camera position
			camera.target = { 0.0f, 0.5f, 0.0f };      // Camera looking at point
			camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 45.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

			 // Load models and texture
			modelA = LoadModelFromMesh(GenMeshKnot(0.4f, 1.0f, 16, 32));
			modelB = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
			modelC = LoadModelFromMesh(GenMeshSphere(0.5f, 32, 32));
			string texPath = RootPath + "res/texel_checker.png";
			texture = LoadTexture(texPath.c_str());

			// Assign texture to default model material
			modelA.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
			modelB.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
			modelC.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
			string vs = RootPath + "shader/brdf/fog.vs";
			string ps = RootPath + "shader/brdf/fog.fs";
			shader = LoadShader(vs.c_str(), ps.c_str());
			shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
			shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
			// Ambient light level
			ambientLoc = GetShaderLocation(shader, "ambient");
			SetShaderValue(shader, ambientLoc, &ambient, SHADER_UNIFORM_VEC4);
			fogDensityLoc = GetShaderLocation(shader, "fogDensity");
			SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);
			// NOTE: All models share the same shader
			modelA.materials[0].shader = shader;
			modelB.materials[0].shader = shader;
			modelC.materials[0].shader = shader;
			// Using just 1 point lights
			mLight =CreateLight(LIGHT_POINT, lightPos, Vector3Zero(), WHITE, shader);
		}

		void Tick() {
			// Update
			//----------------------------------------------------------------------------------
			UpdateCamera(&camera, CAMERA_ORBITAL);

			if (IsKeyDown(KEY_UP))
			{
				fogDensity += 0.001f;
				if (fogDensity > 1.0f) fogDensity = 1.0f;
			}

			if (IsKeyDown(KEY_DOWN))
			{
				fogDensity -= 0.001f;
				if (fogDensity < 0.0f) fogDensity = 0.0f;
			}

			SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);
			modelA.transform = MatrixMultiply(modelA.transform, MatrixRotateX(-0.025f));
			modelA.transform = MatrixMultiply(modelA.transform, MatrixRotateZ(0.012f));
			// Update the light shader with the camera view position
			SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position.x, SHADER_UNIFORM_VEC3);
			//----------------------------------------------------------------------------------
		}

		void Render() {
			BeginMode3D(camera);
			// Draw the three models
			DrawModel(modelA, Vector3Zero(), 1.0f, WHITE);
			DrawModel(modelB,{ -2.6f, 0, 0 }, 1.0f, WHITE);
			DrawModel(modelC,{ 2.6f, 0, 0 }, 1.0f, WHITE);
			for (int i = -20; i < 20; i += 2) DrawModel(modelA,  { (float)i, 0, 2 }, 1.0f, WHITE);
			EndMode3D();
		}


		void ConifgShaderValue(Shader shader, string name, void* value, int type) {
			SetShaderValue(shader, GetShaderLocation(shader, name.c_str()), value, type);
		}

		MLight mLight;
		int fogDensityLoc;
		int ambientLoc;

		float fogDensity = 0.15f;
		Vector3 lightPos = {0,2,6};
		Vector4 ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
		Shader shader;
		Camera camera;
		Model modelA;
		Model modelB;
		Model modelC;
		Texture texture;
	};
}