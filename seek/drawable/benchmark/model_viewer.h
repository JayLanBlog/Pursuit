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
namespace MarkView {
	class ModerActor
	{
	public:
		ModerActor() {}
		~ModerActor(){}
		void LoadIn() {
			// Camera mode type
			// Define the camera to look into our 3d world
			camera.position = { 50.0f, 50.0f, 50.0f }; // Camera position
			camera.target = { 0.0f, 10.0f, 0.0f };     // Camera looking at point
			camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 45.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;
			model = Loader::LoadModel("resource/res/gltf/obj/castle.obj");
			texture = LoadTexture("castle_diffuse.png");
		
			model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;            // Set map diffuse texture
		}
		void Shaow() {
			UpdateCamera(&camera, CAMERA_CUSTOM);
			BeginMode3D(camera);
			DrawModel(model, position, 1.0f, WHITE);
			EndMode3D();
		}
	private:
		Model model;
		Camera camera = { 0 };
		Texture2D texture;
		Vector3 position = { 0.0f, 0.0f, 0.0f };
	};

}