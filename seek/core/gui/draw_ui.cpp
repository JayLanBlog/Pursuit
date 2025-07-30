#include "draw_ui.h"
#include "imgui.h"
#include "logger/logger.h"
#include <style/font/fonts.h>
#include "rmd/gl/gl_include.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"
//Vulkan Impl
#include <SDL3/SDL_vulkan.h> 
//OpengGL Impl
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include "imgui_impl_opengl3.h"
#endif
#include "menu/entry_manager.h"
#include <helper/obf.h>

ImFont* bigFont;
ImGuiStyle* style;

void InitImGUI(SDL_Window* window, SDL_GLContext glContext) {
	TRACELOG(LOG_INFO, "Platform InitImGUI (SDL) Load");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	if (window ==nullptr && glContext ==nullptr) {
		TRACELOG(LOG_INFO, "Platform window and glContext is null");
	}
	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    std::string glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version.c_str());


	float sizePiexl = 28;
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear(); // clear fonts if you loaded some before (even if only default one was loaded)
	ImFontConfig font_cfg;
	ImFont* bigFont;
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
	CreatePanel();
}


void NewImGUIFrame() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void EndGUIFrame() {
	ImGui::Render();
}

void PresentImGUI() {
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ProccessImGuiEvent(SDL_Event event) {
	ImGui_ImplSDL3_ProcessEvent(&event);
}

void DestroyImgui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}



void CreatePanel() {
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontFromMemoryTTF((void*)fontAwesome, sizeof(fontAwesome), 27, &icons_config, icons_ranges);
	ImFontConfig bigFontCfg;
	bigFontCfg.FontDataOwnedByAtlas = false; // if true it will try to free memory and fail
	bigFont = io.Fonts->AddFontFromMemoryTTF((void*)poppinsFont, sizeof(poppinsFont), 24, &bigFontCfg);
	style = &ImGui::GetStyle();

}


void RenderPanel() {
	ImGui::Begin(" ");

	ImGui::BeginGroup(); { // group it so we can redirect to Website when its pressed
		ImGui::BeginChild(obf("Logo").c_str(), ImVec2(158, 50), true);
		ImGui::PushFont(bigFont);
		ImGui::SameLine();
		ImGui::SetCursorPosY(11); // dont know how to center it sorry :>
		ImGui::TextUnformatted(obf("Pursue To Seek").c_str());
		ImGui::PopFont();

		ImGui::EndChild();

		if (ImGui::IsItemClicked(1)) { // redirect to a website/discord on right click
		//	::ShellExecuteA(NULL, obf("open").c_str(), obf("https://www.youtube.com/watch?v=dQw4w9WgXcQ").c_str(), NULL, NULL, SW_SHOWDEFAULT);
		}

		ImGui::EndGroup();
		ImGui::Spacing();

		ImGui::BeginGroup();
		int count = EntryManager::i().EntryListCount();
		for (int i = count - 1; i >= 0; i--) {

			EntryDebugger* debuger = EntryManager::i().GetEntryByIndex(i);
			ImGui::BeginChild(debuger->name.c_str(), debuger->size, true);
			ImGui::PushFont(bigFont);
			ImGui::SameLine();
			ImGui::SetCursorPosY(11); // dont know how to center it sorry :>
			ImGui::TextUnformatted(debuger->name.c_str());
			ImGui::PopFont();
			debuger->InitEntry();

			ImGui::EndChild();
		}
		ImGui::EndGroup();

		ImGui::End();
	}

}