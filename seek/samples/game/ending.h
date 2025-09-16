#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <gemo/TorusKnotGeometry.h>
#include <core/pass/draw_pass2d.h>
#include <core/gestures.h>
#include <core/text/draw_text.h>
#include <file/sys_text.h>

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace GEMO;
using namespace Text;


namespace Game {
	class Ending {
	public:
		const string RootPath = "E:/gl/resource/";

		int score = 0;
		int result = -1;
		Font font2 = { 0 };
		Font font = { 0 };
		int framesCounter = 0;
		int finishScreen = 0;

		int scrollPositionX = 0;

		Ending() {}
		~Ending() {}

		void InitEndingScreen() {
			framesCounter = 0;
			finishScreen = 0;
			string fontPath = RootPath + "res/cat/star.fnt";

			font = LoadFont(fontPath.c_str());
			string font2Path = RootPath + "res/cat/star2.fnt";
			font2 = LoadFont(font2Path.c_str());
		}

		void UpdateEndingScreen() {
			framesCounter++;

			scrollPositionX -= 5;
			if (scrollPositionX < -GetScreenWidth()) scrollPositionX = 0;

			// Press enter or tap to return to TITLE screen
			if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
			{
				finishScreen = 1;
				//PlaySound(fxCoin);
			}
		}

		void DrawEndingScreen() {
			for (int i = 0; i < 64 * 2 * 2; i++)
			{
				DrawRectangle(64 * i + scrollPositionX, 0, 64, GetScreenHeight(), (i % 2 == 0) ? GetColor(0xf3726dff) : GetColor(0xffcf6bff));
			}

			if (result == 0) DrawTextEx(font2, "YOU LOOSE...", { 350, 200 }, font2.baseSize * 2, 2, WHITE);
			else if (result == 1) DrawTextEx(font, "YOU WIN!!!", { 380, 200 }, font.baseSize * 2, 2, WHITE);

			// Draw score
			DrawTextEx(font, TextFormat("FINAL SCORE: %i", score), { 400, 360 }, font2.baseSize, 2, WHITE);

			if ((framesCounter / 30) % 2) DrawTextEx(font2, "PRESS ENTER to TITLE", { 340, 550 }, font2.baseSize, 2, WHITE);
		}
	};
	
}