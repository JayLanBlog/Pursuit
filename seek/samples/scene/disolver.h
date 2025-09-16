#pragma once
#include <drawable/benchmark/sky_box.h>
#include <drawable/benchmark/custom_model.h>

#include <math/pr_math.h>
#include <core/pass/draw_pass2d.h>
#include <core/text/draw_shape.h>

using namespace PMath;
using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace GEMO;
using namespace Acr;

namespace Scene {
	
	class Disolver {
	public:
		void LoaderScene(int width, int height) {
		//	disFbo = new DIsolveFBO(width, height);
			screenWidth = width;
			screenHeight = height;
			cust.LoadScene(width, height);
			target = LoadRenderTextureDepthTex(screenWidth, screenHeight);
			box.LoadScene(width, height);
		}

		void Tick() {
			cust.Tick();
			box.Tick();
		}

		void PreRender() {
			BeginTextureMode(target);
			ClearBackground(WHITE);
			//EnableDepthTest(); //Manually enable Depth Test to handle multiple rendering methods.
			cust.Render();
			EndTextureMode();
		}

		void Render() {
			box.Render();

			DrawTextureRec(target.texture, { 0, 0, (float)screenWidth, (float)-screenHeight }, { 0, 0 }, WHITE);
			//box.Render();
		}
		int screenWidth;
		int screenHeight;
		CustM cust;
		SkyBox box;
		RenderTexture2D target;
	};
	
}