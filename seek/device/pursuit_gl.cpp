#include "pursuit_gl.h"
#include "rmd/gl/gl_global.h"

using namespace Seek::UI;
namespace Seek {
	namespace Device {
		void SeekGLDevice::Create() {
			// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100 (WebGL 1.0)
			const char* glsl_version = "#version 100";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
	// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
			const char* glsl_version = "#version 300 es";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
			const char* glsl_version = "#version 150";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
			const char* glsl_version = "#version 130";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
			// Create window with graphics context
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
			CreateProfilerWindow();
			CreateProfilerContext();
			seek_ui->Create();
		}

		void SeekGLDevice::MakeContextForDevice() {
			MakeWindow();
			seek_ui->MakeContextForUI(sdlWindow, gl_context);
			//load opengl address 
			gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
			seek_ui->LoadFonts();
		}

		void SeekGLDevice::SubmitForPresent() {
			seek_ui->SubmitImguiDataForPresent();
			SDL_GL_SwapWindow(sdlWindow);
		}

		bool SeekGLDevice::ConfigMinSizeIFNeed() {
			bool needMin = false;
			// [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
			if (SDL_GetWindowFlags(sdlWindow) & SDL_WINDOW_MINIMIZED)
			{
				needMin = true;
			}
			return needMin;
		}

		void SeekGLDevice::UpdareViewPort(ImVec4 view, ImVec4 clear_color) {
			glViewport(view.x, view.y, (int)view.z, (int)view.w);
			glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		bool SeekGLDevice::ProcessEvent(SDL_Event event) {
			bool close = false;
			seek_ui->ProccessEvent(event);
			if (event.type == SDL_EVENT_QUIT)
				close = true;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(sdlWindow))
				close = true;
			return close;
		}

		void SeekGLDevice::CreateProfilerWindow() {
			Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
			sdlWindow = SDL_CreateWindow("Dear ImGui SDL3+OpenGL3 example", windowWidth, windowHeight, window_flags);
			if (sdlWindow == nullptr)
			{
				printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
			}
			SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}

		void SeekGLDevice::CreateProfilerContext() {
			gl_context = SDL_GL_CreateContext(sdlWindow);
			if (gl_context == nullptr)
			{
				printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
				return;
			}
		}
	}
}