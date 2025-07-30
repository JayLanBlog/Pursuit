#pragma once
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <common/core.h>
#include <SDL3/SDL.h>

namespace Seek {
	namespace UI{
		class SeekUI {
		public:
			SeekUI() = default;
			virtual ~SeekUI() {}
			virtual void Create() {};
			virtual void MakeContextForUI(SDL_Window* window, SDL_GLContext gl_context) = 0;
			virtual void NewUIFrame() = 0;
			virtual void EndUIFrme() = 0;
			virtual void SubmitImguiDataForPresent() = 0;
			virtual void ProccessEvent(SDL_Event event) = 0;
			virtual void Destroy() {};
			virtual void LoadFonts() {}
			bool loadFonts = true;
			float sizePiexl = 28;
		};
	}
}