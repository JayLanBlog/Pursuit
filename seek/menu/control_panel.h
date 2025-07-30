#pragma once
#include "imgui.h"
#include "../tool/helper/obf.h"
#include <string>
#include "../tool/helper/imgui_help.h"
#include "entry_view.h"

namespace ControlPanel {

	class Panel {
	public:
		Panel();
		void MakePanel();
		void RenderLogo();
		void RenderPanel();
		void RendrUserData();
		void RenderTabs();
		void Render();
		ImFont* bigFont;
		ImGuiStyle* style;
	};

}


