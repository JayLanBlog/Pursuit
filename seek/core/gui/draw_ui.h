#pragma once
#include "SDL3/SDL.h"

void InitImGUI(SDL_Window* window, SDL_GLContext glContext);

void NewImGUIFrame();

void EndGUIFrame();

void PresentImGUI();

void ProccessImGuiEvent(SDL_Event event);

void DestroyImgui();

void CreatePanel();

void RenderPanel();



