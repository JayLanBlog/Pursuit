#pragma once
#include <ui/seek_gl.h>

namespace Seek {

	namespace Device {
		class SeekDevice {
		public:
			virtual void Create() = 0;
			virtual void SetWindowSize(float window_width, float window_height) = 0;
			virtual void MakeContextForDevice() = 0;
			virtual void Update() = 0;
			virtual bool ConfigMinSizeIFNeed() = 0;
			virtual bool ProcessEvent(SDL_Event event) = 0;
			virtual void UpdareViewPort(ImVec4 view, ImVec4 clear_color) = 0;
			virtual void Destory() = 0;
			virtual void NewFrame() = 0;
			virtual void EndFrame() = 0;
			virtual void SubmitForPresent() = 0;
		};
	
	}
}
