#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <math/pr_math.h>

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace PMath;

namespace BRDF {

	class LightParam : public EntryDebugger {
	public:
		LightParam() {
			name = "PBR Light";
		}

		virtual void InitEntry() {
		
		}

	};

	class LightBrdf {
	public:
		const string RootPath = "E:/gl/resource/";
		void LoadScene(int screenWidth,int screenHeight) {
		//	Vector3 enemySpherePos = { 4.0f, 0.0f, 0.0f };
			width = screenWidth;
			height = screenHeight;
			string vsPath = RootPath + "shader/brdf/brdflight.vs";
			string fsPath = RootPath + "shader/brdf/brdflight.fs";
			lighterShader = LoadShader(vsPath.c_str(), fsPath.c_str());
			model = MatrixIdentity();
			ConifgShaderValue(lighterShader,"albedo",&albedo,SHADER_UNIFORM_VEC3);
			ConifgShaderValue(lighterShader, "ao", &ao, SHADER_UNIFORM_FLOAT);
		}


		void ConifgShaderValue(Shader shader, string name, void* value, int type) {
			SetShaderValue(shader, GetShaderLocation(shader, name.c_str()), value, type);
		}

		void UpdateShaderParam() {
			ConifgShaderValue(lighterShader, "camPos", &camera.position, SHADER_UNIFORM_VEC3);
		}

		void EnableConfig() {
			EnableDepthTest();
		}

		void Tick() {
		
		}

		void Render() {

			ConifgShaderValue(lighterShader, "camPos", &camera.position, SHADER_UNIFORM_VEC3);
			for (int row = 0; row < nrColumns; ++row) {
				float meta = (float)row / (float)nrRows;
				ConifgShaderValue(lighterShader, "metallic", &meta, SHADER_UNIFORM_FLOAT);
				for (int col = 0; col < nrColumns; ++col) {
					float rouhf = Clamp((float)col / (float)nrColumns, 0.05f, 1.0f);
					ConifgShaderValue(lighterShader, "roughness", &rouhf, SHADER_UNIFORM_FLOAT);


					Vector3 dis = { (col - (nrColumns / 2)) * spacing,
					(row - (nrRows / 2)) * spacing,
					0.0f };
					model = MatrixIdentity();
					model = Matrix_Translate(&model,dis);
					SetShaderValueMatrix(lighterShader, GetShaderLocation(lighterShader, "model"), model);
					SetShaderValueMatrix3(lighterShader, GetShaderLocation(lighterShader, "normalMatrix"), Matrix_InvTranspose3x3(&model));
					RenderSphere();
				}
			}

			for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
				//Vector3 newPos =	lightPositions[i] + {sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0};
			
				float deltaTime = GetFrameTime();
				u_time += deltaTime;
				Vector3 newPos = { lightPositions[i].x + sin(u_time * 0.5)*4.0,lightPositions[i].y,lightPositions[i].z};
				string lightPos = "lightPositions[" + std::to_string(i) + "]";
				string lightc = "lightColors[" + std::to_string(i) + "]";
				ConifgShaderValue(lighterShader, lightPos.c_str(), &newPos, SHADER_UNIFORM_VEC3);
				ConifgShaderValue(lighterShader, lightc.c_str(), &lightColors[i], SHADER_UNIFORM_VEC3);
				model = MatrixIdentity();
				model = Matrix_Translate(&model, newPos);
			//	model = MatrixScale(model, glm::vec3(0.5f));
				//MatrixRotate(&model,0.5f);
				model = Matrix_Scale(&model,{0.5f});
				SetShaderValueMatrix(lighterShader, GetShaderLocation(lighterShader, "model"), model);
				SetShaderValueMatrix3(lighterShader, GetShaderLocation(lighterShader, "normalMatrix"), Matrix_InvTranspose3x3(&model));
				RenderSphere();
			}

			
		}

		void RenderSphere() {
			BeginShaderMode(lighterShader);
			BeginMode3D(camera);
			DrawSphere(enemySpherePos, enemySphereSize, BLACK);
			//DrawSphereWires(enemySpherePos, enemySphereSize, 16, 64,DARKGRAY);
			EndMode3D();
			EndShaderMode();
		}

		void Disable() {
			DisableDepthTest();
		}

		float u_time = 0.0f;
		Matrix model;
		int nrRows = 7;
		int nrColumns = 7;
		float spacing = 2.5;	

		Vector3 albedo = {0.5,0.0,0.0};
		float ao = 1.0f;
		
		Vector3 lightPositions[4] = {
			{ -10.0f,  10.0f, 10.0f}, {10.0f,  10.0f, 10.0f},
			{-10.0f, -10.0f, 10.0f },{10.0f, -10.0f, 10.0f}
		};

		Vector3 lightColors[4] = {
			{300.0f, 300.0f, 300.0f},
			{300.0f, 300.0f, 300.0f},
			{300.0f, 300.0f, 300.0f},
			{300.0f, 300.0f, 300.0f}
		};

		Shader lighterShader;
		int width = 0, height = 0;
		Camera camera = { { 0.0f,0.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
		float enemySphereSize = 1.0f;
		Vector3 enemySpherePos = { 4.0f, 0.0f, 0.0f };
	};
}