#pragma once
#include <vector>
#include <string>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "../tool/helper/singletonh.h"
#include <style/font/fonts.h>



using namespace Seek::Helper;

struct Output {
	std::string text, time;
	bool drawTime;
	int type;

	Output(std::string text_, bool drawTime_, int type_) {
		text = text_;
		drawTime = drawTime_;
		type = type_;
	}
};

class Console : public Singleton<Console> {
	friend class Singleton<Console>;

	void renderLog(Output& output);
	void renderLogError(Output& output);
	void renderLogInfo(Output& output);

public:
	int selectedTypeTab;
	std::vector<std::string> types{ ICON_FA_LIST_ALT " All", ICON_FA_TIMES_CIRCLE " Errors", ICON_FA_INFO_CIRCLE " Info" };

	char inputBuf[256];
	std::vector<Output> outputArr;

	bool DRAW_CONSOLE = true;

	void render();

	void log(std::string, bool printTime = true);
	void logError(std::string, bool printTime = true);
	void logInfo(std::string, bool printTime = true);
};

