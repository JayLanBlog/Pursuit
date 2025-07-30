#pragma once
#include "seek_ui.h"
#include "style/font/fonts.h"
#include "imgui.h"


namespace Seek {
	namespace UI {

		class SeekGLUI : public SeekUI {
		public:
			ImFont* bigFont;
			SeekGLUI() = default;
			~SeekGLUI() { Destroy(); }
			virtual void Create();
			virtual void LoadFonts();
			virtual void MakeContextForUI(SDL_Window* window, SDL_GLContext gl_context);
			virtual void NewUIFrame();
			virtual void EndUIFrme();
			virtual void SubmitImguiDataForPresent();
			virtual void ProccessEvent(SDL_Event event);
			virtual void Destroy();
		};
	}
}