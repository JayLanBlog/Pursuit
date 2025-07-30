#pragma once
#include <section/enum/winplat_em.h>
#include <section/enum/key_em.h>



// Initialize platform: graphics, inputs and more
int InitPlatform(void);

double GetTime(void);


// Swap back buffer with front buffer (screen drawing)
void SwapScreenBuffer(void);


// Close platform
void ClosePlatform(void);


// Register all input events
void PollInputEvents(void);
