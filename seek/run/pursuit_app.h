#pragma once
#include <rmd/gl/gl_include.h>
#include <menu/console.h>
#include <menu/control_panel.h>
#include <drawable/render/base_render.h>
#include <view/3D/loader/loader_md.h>
#include <device/pursuit_gl.h>
#include <view/3D/model/modelv.h>

using namespace Seek::Device;
namespace Seek {
	namespace Run {
		
		class App {
		public:
			App() {}
			virtual void Create() = 0;
			virtual void Init() = 0;
			virtual void Run() = 0;
			virtual void Destroy() = 0;
		};




		class PursuitApp : public App {
		public:
			Model model;
			PurglData pgl = {0};
			Drawable::PrimitiveRender render;
			PursuitApp() {
				EnableDeviceExtantion();
				seekDevice = new SeekGLDevice();
				if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
				{
					printf("Error: SDL_Init(): %s\n", SDL_GetError());
					return;
				}
				Create();
				Init();
				LoadDefaultShader();
				//PLGL = pgl;
				pannel = new ControlPanel::Panel();
				Shader shader = DRAW::GL::LoadShader("resource/shader/1.model_loading.vs", "resource/shader/1.model_loading.fs");
				int defalutSahderId = PLGL.State.defaultFShaderId;
				int ix = 0;
				render.LoadPritiveShader();
				model = Loader::LoadModel("resource/res/gltf/cube/cube.gltf");
				//Seek::GLTF::GLTFParse("resource/res/gltf/cube/cube.gltf");
				/*	cgltf_data* data = NULL;
					cgltf_options options = {};
					cgltf_result result = cgltf_parse_file(&options, "resource/res/gltf/cube/cube.gltf", &data);*/
					//	Seek::GLTF::Util::ParseGltf("resource/res/gltf/cube/cube.gltf");
			}
			void EnableDeviceExtantion() {
				PLGL.ExtSupported.vao = true;
				PLGL.ExtSupported.instancing = true;
				PLGL.ExtSupported.texNPOT = true;
				PLGL.ExtSupported.texFloat32 = true;
				PLGL.ExtSupported.texFloat16 = true;
				PLGL.ExtSupported.texDepth = true;
				PLGL.ExtSupported.maxDepthBits = 32;
				PLGL.ExtSupported.texAnisoFilter = true;
				PLGL.ExtSupported.texMirrorClamp = true;
			}

			virtual void Create() {
				//TRACELOG(LOG_INFO,"PursuitApp Create : %d",1);
				seekDevice->SetWindowSize(2000, 1280);
				seekDevice->Create();
			}

			virtual void Init() {
				seekDevice->MakeContextForDevice();
				//actor = new ACTOR::ShadowActor();
				//actor->MakeActor();
				render.MakeGeometry();
			}

			virtual void Run() {
				while (!done) {
					SDL_Event event;
					while (SDL_PollEvent(&event)) {
						done = seekDevice->ProcessEvent(event);
					}
					if (seekDevice->ConfigMinSizeIFNeed()) {
						SDL_Delay(10);
						continue;
					}
					seekDevice->NewFrame();
					pannel->Render();
					if (logEnable)
						RenderLogToConsole();
					seekDevice->EndFrame();
				//	actor->Draw();
					render.Draw();
					seekDevice->SubmitForPresent();
				}
				Destroy();
			}

			virtual void Destroy() {
				UnloadDefautlShader();
				if (seekDevice) {
					delete seekDevice;
				}
			}
			void RenderLogToConsole() {
				Console::i().render();
			}
		private:
			ControlPanel::Panel* pannel = nullptr;
			SeekGLDevice* seekDevice = nullptr;
			//ACTOR::Actor* actor = nullptr;
			bool logEnable = true;
			bool done = false;
		};
	
	}
}