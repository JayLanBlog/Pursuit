#include "seek_gl.h"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"
//Vulkan Impl
#include <SDL3/SDL_vulkan.h> 
//OpengGL Impl
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#include "imgui_impl_opengl3.h"
#endif



namespace Seek {
	namespace UI {
		void SeekGLUI::Create() {
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui::StyleColorsDark();
		};

		void SeekGLUI::LoadFonts() {
			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->Clear(); // clear fonts if you loaded some before (even if only default one was loaded)
			ImFontConfig font_cfg;
			font_cfg.FontDataOwnedByAtlas = false; // if true it will try to free memory and fail
			io.Fonts->AddFontFromMemoryTTF((void*)poppinsFont, sizeof(poppinsFont), sizePiexl, &font_cfg);
			static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
			ImFontConfig icons_config;
			icons_config.MergeMode = true;
			icons_config.PixelSnapH = true;
			icons_config.FontDataOwnedByAtlas = false;
			io.Fonts->AddFontFromMemoryTTF((void*)fontAwesome, sizeof(fontAwesome), sizePiexl, &icons_config, icons_ranges);
			ImFontConfig bigFontCfg;
			bigFontCfg.FontDataOwnedByAtlas = false; // if true it will try to free memory and fail
			bigFont = io.Fonts->AddFontFromMemoryTTF((void*)poppinsFont, sizeof(poppinsFont), 24, &bigFontCfg);
			io.Fonts->AddFontFromMemoryTTF((void*)fontAwesome, sizeof(fontAwesome), sizePiexl, &icons_config, icons_ranges);
		}


		void SeekGLUI::MakeContextForUI(SDL_Window* window, SDL_GLContext gl_context) {
			ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
#if defined(IMGUI_IMPL_OPENGL_ES2)
			// GL ES 2.0 + GLSL 100 (WebGL 1.0)
			const char* glsl_version = "#version 100";
#elif defined(IMGUI_IMPL_OPENGL_ES3)
			// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
			const char* glsl_version = "#version 300 es";
#elif defined(__APPLE__)
			// GL 3.2 Core + GLSL 150
			const char* glsl_version = "#version 150";
#else
			// GL 3.0 + GLSL 130
			const char* glsl_version = "#version 130";
#endif
			ImGui_ImplOpenGL3_Init(glsl_version);
		};

		void SeekGLUI::NewUIFrame() {
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();
		};

		void SeekGLUI::SubmitImguiDataForPresent() {
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		};

		void SeekGLUI::EndUIFrme() {
			 ImGui::Render();
		 };

		void SeekGLUI::ProccessEvent(SDL_Event event) {
			ImGui_ImplSDL3_ProcessEvent(&event);
		}

		void SeekGLUI::Destroy() {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL3_Shutdown();
			ImGui::DestroyContext();
		}

	}
}

