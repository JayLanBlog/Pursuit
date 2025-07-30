#include "control_panel.h"
#include <style/font/fonts.h>

#include "../tool/helper/time_h.h"
#include "../tool/helper/imgui_help.h"
#include "../tool/helper/obf.h"
#include "entry_manager.h"



namespace ControlPanel {

	Panel::Panel() {
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
		MakePanel();
	}


	void Panel::MakePanel() {

	}

	void Panel::RenderLogo() {
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
		}

	}

	void Panel::RenderPanel() {
		//ImGuiIO& io = ImGui::GetIO();
		////IMGUI_DEMO_MARKER("Configuration/Backend Flags");
		//if (ImGui::TreeNode("Backend Flags"))
		//{

		//	// FIXME: Maybe we need a BeginReadonly() equivalent to keep label bright?
		//	ImGui::BeginDisabled();
		//	ImGui::CheckboxFlags("io.BackendFlags: HasGamepad", &io.BackendFlags, ImGuiBackendFlags_HasGamepad);
		//	ImGui::CheckboxFlags("io.BackendFlags: HasMouseCursors", &io.BackendFlags, ImGuiBackendFlags_HasMouseCursors);
		//	ImGui::CheckboxFlags("io.BackendFlags: HasSetMousePos", &io.BackendFlags, ImGuiBackendFlags_HasSetMousePos);
		//	ImGui::CheckboxFlags("io.BackendFlags: RendererHasVtxOffset", &io.BackendFlags, ImGuiBackendFlags_RendererHasVtxOffset);
		//	ImGui::EndDisabled();

		//	ImGui::TreePop();
		//	ImGui::Spacing();
		//}

		//if (ImGui::TreeNode("Backend Flags2"))
		//{

		//	// FIXME: Maybe we need a BeginReadonly() equivalent to keep label bright?
		//	ImGui::BeginDisabled();
		//	ImGui::CheckboxFlags("io.BackendFlags: HasGamepad", &io.BackendFlags, ImGuiBackendFlags_HasGamepad);
		//	ImGui::CheckboxFlags("io.BackendFlags: HasMouseCursors", &io.BackendFlags, ImGuiBackendFlags_HasMouseCursors);
		//	ImGui::CheckboxFlags("io.BackendFlags: HasSetMousePos", &io.BackendFlags, ImGuiBackendFlags_HasSetMousePos);
		//	ImGui::CheckboxFlags("io.BackendFlags: RendererHasVtxOffset", &io.BackendFlags, ImGuiBackendFlags_RendererHasVtxOffset);
		//	ImGui::EndDisabled();

		//	ImGui::TreePop();
		//	ImGui::Spacing();
		//}

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
	}


	void Panel::RendrUserData() {

	}


	void Panel::RenderTabs() {



	}

	void Panel::Render() {
		ImGui::Begin(" ");

		RenderLogo();
		ImGui::Spacing();
		RenderPanel();

		ImGui::End();
	}
}