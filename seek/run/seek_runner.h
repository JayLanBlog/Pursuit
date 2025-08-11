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
using namespace SDF;
using namespace ModelView;
using namespace MarkView;
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
			


			SeekerApp() {
				//pbrView.EnbleConfig();
			//	shadowView.EnablePiplineState();
				Create();
				Init();
				//render.LoadPritiveShader();
				//actor.LoadIn();
				//pbrView.LoadPBR();
				//shadowView.CreateAndInitialize();
			//	round.CreateRectange();
				//hibird.LoaderScene(screenWidth, screenHeight);
				//deffered.LoaderScene(screenWidth, screenHeight);
				baseSdf.LoadSdfFile("resource/shader/sdf/sher.fs",screenWidth,screenHeight);
				//model.LoadIn();
				SetTargetFPS(60);
			}
			
			virtual void Create() {
				InitWindow(screenWidth, screenHeight, "models loading");
			}
			virtual void Init() {
				//render.MakeGeometry();
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
				//	deffered.Tick();
					baseSdf.Tick();
					BeginDrawing();
					ClearBackground(RAYWHITE);
					//round.Draw(screenWidth, screenHeight);
					//shadowView.Draw();
					//actor.Shaow();
					//pbrView.Draw();
					//model.Shaow();
				//	hibird.Render();
				//	deffered.Render();
					baseSdf.Draw();
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