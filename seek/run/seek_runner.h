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
			SeekerApp() {
				Create();
				Init();
				//render.LoadPritiveShader();
				actor.LoadIn();
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
					actor.Animation();
					BeginDrawing();
					ClearBackground(RAYWHITE);
					actor.Shaow();
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