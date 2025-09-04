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

using namespace MView;
#include <drawable/benchmark/sky_sample.h>

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace PMath;

namespace Gems {
	class GrassView {
	public:

		const string RootPath = "E:/gl/resource/";
		GrassView() {}
		~GrassView() {}

		void LoadScene(int screanWidth, int screenHeight) {
			camera.position = { 0.0f, 5.0f, 10.0f }; // Camera position
			camera.target = { 0.0f, 10.0f, 0.0f };     // Camera looking at point
			camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 45.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;			width = screanWidth;
			width = screanWidth;
			height = screanWidth;
			skyCube.LoadScene(width, height);
		}

		void Tick() {
			UpdateCamera(&camera, CAMERA_FREE);
			skyCube.Tick();
		}

		void Render() {
			skyCube.Render();
		}

		int width = 0, height = 0;
		Camera camera = { 0 };
		GLCubeView skyCube;
	};
}