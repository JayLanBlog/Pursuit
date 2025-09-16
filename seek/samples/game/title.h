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

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace GEMO;

namespace Game {
	class Title {
	public:
		const string RootPath = "E:/gl/resource/";
		Title() {}
		~Title() {}

        Font font2 = { 0 };

        int framesCounter = 0;
        int finishScreen = 0;
        int state = 0;


        int scrollPositionX = 0;

        int catPosX = 0;
        int roombaPosX = 0;
        float vsAlpha = 0.0f;
        float vsScale = 0.0f;

        Texture2D cat = { 0 };
        Texture2D vs = { 0 };
        Texture2D roomba = { 0 };

		void InitTitleScreen() {
            framesCounter = 0;
            finishScreen = 0;
            string catPath = RootPath + "res/cat/title_cat.png";
            string vPath = RootPath + "res/cat/title_vs.png";
            string roombaPath = RootPath + "res/cat/title_roomba.png";

            cat = LoadTexture(catPath.c_str());
            vs = LoadTexture(vPath.c_str());
            roomba = LoadTexture(roombaPath.c_str());
            state = 0;
            catPosX = 1760;
            roombaPosX = -700;
            scrollPositionX = 0;
            vsAlpha = 0.0f;
            vsScale = 10.0f;
            string font2Path = RootPath + "res/cat/star2.fnt";
            font2 = LoadFont(font2Path.c_str());
		}

        void UpdateTitleScreen() {
            scrollPositionX -= 5;
            if (scrollPositionX < -GetScreenWidth()) scrollPositionX = 0;
            
            if (state == 0) {
                catPosX -= 4;
                roombaPosX += 3;

                if (catPosX < (GetScreenWidth() / 2 - cat.width / 2)) catPosX = (GetScreenWidth() / 2 - cat.width / 2);
                if (roombaPosX > (GetScreenWidth() / 2 - roomba.width / 2)) roombaPosX = (GetScreenWidth() / 2 - roomba.width / 2);

                if ((catPosX == (GetScreenWidth() / 2 - cat.width / 2)) && (roombaPosX == (GetScreenWidth() / 2 - roomba.width / 2)))
                {
                    state = 1;
                    framesCounter = 0;
                }
            }

            else if (state == 1)
            {
                framesCounter++;

                vsScale -= 0.1f;
                vsAlpha += 0.01f;

                if (vsScale < 1.0f) vsScale = 1.0f;
                if (vsAlpha > 1.0f) vsAlpha = 1.0f;

                if (framesCounter > 160)
                {
                    state = 2;
                    framesCounter = 0;
                }
            }
            else if (state == 2) framesCounter++;

            // Press enter or tap to change to GAMEPLAY screen
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
            {
                //finishScreen = 1;   // OPTIONS
                finishScreen = 2;   // GAMEPLAY
            //    PlaySound(fxCoin);
            }

        }

        // Title Screen Draw logic
        void DrawTitleScreen(void)
        {
            for (int i = 0; i < 64 * 2 * 2; i++)
            {
                DrawRectangle(64 * i + scrollPositionX, 0, 64, GetScreenHeight(), (i % 2 == 0) ? GetColor(0xf3726dff) : GetColor(0xffcf6bff));
            }

            DrawTexture(cat, catPosX, 80, WHITE);
            DrawTexture(roomba, roombaPosX, 320, WHITE);

            if (state > 0)
            {
                DrawTexturePro(vs,  { 0, 0, (float)vs.width, (float)vs.height }, { (float)GetScreenWidth() / 2, 300, vs.width* vsScale, vs.height* vsScale }, { vs.width / 2 * vsScale, vs.height / 2 * vsScale }, 0.0f, Fade(WHITE, vsAlpha));
            }

            if ((state == 2) && ((framesCounter / 30) % 2)) DrawTextEx(font2, "PRESS ENTER to START",  { 640, 550 }, font2.baseSize, 2, WHITE);
        }

        // Title Screen Unload logic
        void UnloadTitleScreen(void)
        {
            UnloadTexture(cat.id);
            UnloadTexture(vs.id);
            UnloadTexture(roomba.id);
        }

        // Title Screen should finish?
        int FinishTitleScreen(void)
        {
            return finishScreen;
        }
	};
}