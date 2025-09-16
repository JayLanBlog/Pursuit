#pragma once
#include "pursuit_app.h"
#include <core/text/draw_text.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <drawable/benchmark/model_viewer.h>
#include <drawable/benchmark/gltf_view.h>
#include <drawable/benchmark/pbr_view.h>
#include <drawable/benchmark/shadow_view.h>
#include <drawable/benchmark/rectangle_view.h>
#include <drawable/benchmark/shader_draw.h>
#include <drawable/benchmark/derferender_view.h>
#include <drawable/sdf/sdf_base.h>
#include <drawable/blur/gaussion_blur.h>
#include <drawable/blur/box_blur.h>
#include <drawable/blur/kawase_blur.h>
#include <drawable/blur/bokeh_blur.h>
#include <drawable/blur/tiltshift_blur.h>
#include <drawable/blur/iris_blur.h>
#include <drawable/blur/grainy_blur.h>
#include <drawable/blur/radial_blur.h>
#include <drawable/blur/directional_blur.h>
#include <drawable/gems/version1/water_smulate.h>
#include <drawable/benchmark/cube_sky.h>
#include <drawable/benchmark/sky_sample.h>
#include <drawable/gems/version1/water/water.h>
#include <drawable/brdf/brdf_light.h>
#include <drawable/brdf/ao_sample.h>
#include <drawable/gems/version1/grass/grass.h>
#include <drawable/benchmark/sky_box.h>
#include <drawable/benchmark/custom_model.h>
#include <samples/scene/disolve_scenes.h>
#include <drawable/brdf/sky_scatter.h>
#include <drawable/brdf/hybird.h>
#include <samples/scene/disolver.h>
#include <samples/game/cat_in_room.h>

using namespace SDF;
using namespace ModelView;
using namespace MarkView;
using namespace BLUR;
using namespace Smulate;
using namespace MView;
using namespace Water;
using namespace BRDF;
using namespace Gems;
using namespace Acr;
using namespace Scene;
using namespace Game;

namespace Seek {
	namespace Run {
		class SeekerApp : public App {
		public:
			// Initialization
			//--------------------------------------------------------------------------------------
			const int screenWidth = 2000;
			const int screenHeight = 1280;
			GltfView actor;
			ModerActor model;
			PBRView pbrView;
			ShadowView shadowView;
			RoundedRectView round;
			ShaderHibrid hibird;
			DeferedRenderView deffered;
			SDFBase baseSdf;
			GaussionBlur gaussionBlur;
			BoxBlur boxBlur;
			KawaseBlur kawasBlur;
			BokehBlur bokehBlur;
			TiltShiftBlur tiltBlur;
			IrisBlur irisBlur;
			GrainyBlur grainyBlur;
			RadialBlur radialBlur;
			DirectionBlur dirctionBlur;
			WaterSmulater waterSm;
			CubeSkyView  sky;
			GLCubeView cubSky;
			AmbientOcclusion aoSample;
			GerstnerWave water;
			LightBrdf brdf;
			GrassView grass;
			SkyBox box;
			CustM cust;
			DisolveScene disScene;
			Fog fog;
			HyBird hyBird;
			Disolver disolver;
			Room room;

			SeekerApp() {
				//pbrView.EnbleConfig();
				//shadowView.EnablePiplineState();
			    //SetConfigFlags(FLAG_MSAA_4X_HINT);
				Create();
				Init();
			}
			
			virtual void Create() {
				InitWindow(screenWidth, screenHeight, "models loading");
			}
			virtual void Init() {
				//render.MakeGeometry();

				//render.LoadPritiveShader();
				//actor.LoadIn();
				//pbrView.LoadPBR();
				//shadowView.CreateAndInitialize();
				//round.CreateRectange();
				//hibird.LoaderScene(screenWidth, screenHeight);
				//deffered.LoaderScene(screenWidth, screenHeight);
				//baseSdf.LoadSdfFile("resource/shader/sdf/wall.fs",screenWidth,screenHeight);
				//model.LoadIn();
				//gaussionBlur.LoadScene();
				//boxBlur.LoadSceneAndTexture("resource/shader/blur/box_blur.fs", "resource/res/blur/senlin.png");
				//bokehBlur.LoadSceneAndTexture("resource/res/blur/senlin.png", screenWidth,screenHeight);
				//tiltBlur.LoadSceneAndTexture("resource/res/blur/senlin.png", screenWidth, screenHeight);
				//irisBlur.LoadSceneAndTexture("resource/res/blur/senlin.png", screenWidth, screenHeight);
				//grainyBlur.LoadSceneAndTexture("resource/res/blur/senlin.png", screenWidth, screenHeight);
				//radialBlur.LoadSceneAndTexture("resource/res/blur/senlin.png", screenWidth, screenHeight);
				//dirctionBlur.LoadSceneAndTexture("resource/res/blur/senlin.png", screenWidth, screenHeight);
				//waterSm.LoadScene();
				//sky.LoadScene();
				//cubSky.LoadScene(screenWidth,screenHeight);
				//water.LoadScene(screenWidth, screenHeight);
				//brdf.LoadScene(screenWidth,screenHeight);
				//aoSample.LoadSecene(screenWidth, screenHeight);
				//box.LoadScene(screenWidth, screenHeight);
				//cust.LoadScene(screenWidth, screenHeight);
				//disScene.LoaderScene(screenWidth, screenHeight);
				//fog.LoadScene(screenWidth, screenHeight);
				//hyBird.LoadScene(screenWidth,screenHeight);
				//disolver.LoaderScene(screenWidth, screenHeight);
				room.Load();
				SetTargetFPS(120);
			}

			

			virtual void Run() {
				// Main game loop
				int i = 0;
				//DisableCursor();                // Limit cursor to relative movement inside the window
				while (!WindowShouldClose()) {
					//actor.Animation();
					//pbrView.Update();
					//shadowView.Tick();
					//hibird.Tick();
					//hibird.TextureModelRender();
					//deffered.Tick();
					//baseSdf.Tick();
					//bokehBlur.Tick();
					//tiltBlur.Tick();
					//irisBlur.Tick();
					//grainyBlur.Tick();
					//radialBlur.Tick();
					//dirctionBlur.Tick();
					//waterSm.Tick();
					//sky.Tick();
					//cubSky.Tick();
					//water.Tick();
					//aoSample.Tick();
					//box.Tick();
					//cust.Tick();
					//disScene.Tick();
					//fog.Tick();
				//	hyBird.Tick();
				//	hyBird.PreRender();
				//	disolver.Tick();
				//	disolver.PreRender();
					room.Tick();
					BeginDrawing();
					ClearBackground(WHITE);
					//round.Draw(screenWidth, screenHeight);
					//shadowView.Draw();
					//actor.Shaow();
					//pbrView.Draw();
					//model.Shaow();
					//hibird.Render();
					//deffered.Render();
					//baseSdf.Draw();
					//boxBlur.Render();
					//kawasBlur.BlurDraw();
					//bokehBlur.Render();
					//tiltBlur.Render();
				    //irisBlur.Render();
				    //grainyBlur.Render();
				    //radialBlur.Render();
					//dirctionBlur.Render();
					//waterSm.Render();
					//sky.Render();
					//cubSky.Render();
					//water.Render();
					//brdf.Render();
					//aoSample.Render();
					//box.Render();
					//cust.Render();
					//disScene.Render();
					//fog.Render();
					//hyBird.Render();
				//	disolver.Render();
					room.Render();
					DrawFPS(500, 300);
					EndDrawing();
				}    // Detect window close button or ESC key
				Destroy();
			}

			virtual void Destroy() {
				CloseWindow();              // Close window and OpenGL context
			}


		};
	}
}