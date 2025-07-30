#pragma once
#include "pursuit_device.h"
#include "math/pr_math.h"
using namespace Seek::UI;
namespace Seek {
	namespace Device {
		class SeekGLDevice : public SeekDevice {
		public:
			SeekGLDevice() {seek_ui = new SeekGLUI();}
			~SeekGLDevice() {	Destory();}
			virtual void Create();
			virtual void MakeContextForDevice();
			virtual void Update() {}
			virtual void NewFrame() {seek_ui->NewUIFrame();}
			virtual void EndFrame() {seek_ui->EndUIFrme();}
			virtual bool ConfigMinSizeIFNeed();
			virtual bool ProcessEvent(SDL_Event event);
			virtual void Destory() {
				if (seek_ui) {
					delete seek_ui;
				}
				SDL_GL_DestroyContext(gl_context);
				SDL_DestroyWindow(sdlWindow);
			}
			virtual void UpdareViewPort(ImVec4 view, ImVec4 clear_color);
			virtual void SubmitForPresent();
			virtual void SetWindowSize(float window_width, float window_height) {
				windowWidth = window_width;
				windowHeight = window_height;
			}
		private:
			void CreateProfilerWindow();
			void CreateProfilerContext();
			void MakeWindow() {
				SDL_GL_MakeCurrent(sdlWindow, gl_context);
				SDL_GL_SetSwapInterval(1); // Enable vsync
				SDL_ShowWindow(sdlWindow);
			}
			SeekGLUI* seek_ui;
		private:
			//window
			SDL_Window* sdlWindow = nullptr;
			float windowWidth = 800, windowHeight = 600;
			SDL_GLContext gl_context = nullptr;
			//comment data
			bool show_demo_window = true;
			bool show_another_window = false;
			ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 0.00f);
		};
	}
}