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

namespace MarkView {
	class GltfView {
	public:
		GltfView() {}
		~GltfView() {}
		void LoadIn() {
			camera.position = { 6.0f, 6.0f, 6.0f };    // Camera position
			camera.target = { 0.0f, 2.0f, 0.0f };      // Camera looking at point
			camera.up ={ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 45.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
			 // Load gltf model
			model = LoadModel("resource/res/gltf/gltf/robot.glb");
			Vector3 position = { 0.0f, 0.0f, 0.0f }; // Set model position
			// Load gltf model animations
			int animsCount = 0;
			unsigned int animIndex = 0;
			unsigned int animCurrentFrame = 0;
			 modelAnimations = LoadModelAnimations("resource/res/gltf/gltf/robot.glb", &animsCount);
		}

		void Animation() {
			// Update model animation
			ModelAnimation anim = modelAnimations[animIndex];
			animCurrentFrame = (animCurrentFrame + 1) % anim.frameCount;
			UpdateModelAnimation(model, anim, animCurrentFrame);
		}

		void Shaow() {
			UpdateCamera(&camera, CAMERA_CUSTOM);
			BeginMode3D(camera);
			DrawModel(model, position, 1.0f, WHITE);
			EndMode3D();
		}

	private:
		unsigned int animIndex = 5;
		unsigned int animCurrentFrame = 0;
		ModelAnimation* modelAnimations;
		Vector3 position = { 0.0f, 0.0f, 0.0f };
		Model model;
		Camera camera = { 0 };
	};

}