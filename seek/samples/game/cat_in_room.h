#pragma once
#include "logo.h"
#include "title.h"
#include "ending.h"
#include "gameplay.h"

namespace Game {
	typedef enum GameScreen { LOGO = 0, TITLE, OPTIONS, GAMEPLAY, ENDING } GameScreen;

	class Room {
	public:
		Logo logo;
		Title title;
		Ending ending;
		GamePlay play;
		int currentScreen;
		bool onTransition = false;
		void Load() {
		//	logo.LoadScreen();
		//	title.InitTitleScreen();
	    //ending.InitEndingScreen();
			currentScreen = LOGO;
		//	play.InitGameplayScreen();
			logo.InitLogoScreen();

		}
		void UpdateTransition(void) {
			if (!transFadeOut)
			{
				transAlpha += 0.02f;
				if (transAlpha > 1.01f) {
					transAlpha = 1.0f;

					switch (transFromScreen) {
					case LOGO:
						logo.UnloadLogoScreen();
						break;
					case TITLE:
						title.UnloadTitleScreen();
						break;
					}

					switch (currentScreen) {
					case LOGO:
						logo.InitLogoScreen();
						break;
					case TITLE:
						title.InitTitleScreen();
						break;
					}

					  currentScreen = transToScreen;

					 // Activate fade out effect to next loaded screen
					  transFadeOut = true;
				}
			}
			else  // Transition fade out logic
			{
				transAlpha -= 0.02f;

				if (transAlpha < -0.01f)
				{
					transAlpha = 0.0f;
					transFadeOut = false;
					onTransition = false;
					transFromScreen = -1;
					transToScreen = -1;
				}
			}
		}
		void Tick() {

			//logo.Tick();
		
			//title.UpdateTitleScreen();
			//ending.UpdateEndingScreen();
			//play.UpdateGameplayScreen();
			//	logo.InitLogoScreen();

			if (!onTransition)
			{
				switch (currentScreen) {
				  case LOGO:
					  logo.UpdateLogoScreen();
					  if (logo.FinishLogoScreen()) { TransitionToScreen(TITLE); }
					  break;
				  case TITLE:
					  title.UpdateTitleScreen();
					  if (title.FinishTitleScreen()) { TransitionToScreen(LOGO); }

					  break;
				}
			}
			else {
				 UpdateTransition();    // Update transition (fade-in, fade-out)
			}
		}

		void Render() {
		//	logo.Render();
			//title.Rende
			//title.DrawTitleScreen();
			//ending.DrawEndingScreen();
			//play.DrawGameplayScreen();

			switch (currentScreen)
			{
			case LOGO: logo.DrawLogScreen(); break;
			case TITLE: title.DrawTitleScreen(); break;
			default: break;
			}
			// Draw full screen rectangle in front of everything
			if (onTransition) DrawTransition();
		}

		// Draw transition effect (full-screen rectangle)
		 void DrawTransition(void)
		{
			DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transAlpha));
		}


		void TransitionToScreen(int screen) {
			onTransition = true;
			transFadeOut = false;
			transFromScreen = currentScreen;
			transToScreen = screen;
			transAlpha = 0.0f;
		}


		bool transFadeOut = false;
		int transToScreen = -1;
		int transFromScreen = -1;
		float transAlpha = 0.0f;
	};
}