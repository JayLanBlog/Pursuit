#pragma once
#include "core/core_global.h"

//// Drawing-related functions
void ClearBackground(Color color);                          // Set background color (framebuffer clear color)
void BeginDrawing(void);                                    // Setup canvas (framebuffer) to start drawing
void EndDrawing(void);                                      // End canvas drawing and swap buffers (double buffering)
//void BeginMode2D(Camera2D camera);                          // Begin 2D mode with custom camera (2D)
//void EndMode2D(void);                                       // Ends 2D mode with custom camera
void BeginMode3D(Camera3D camera);                          // Begin 3D mode with custom camera (3D)
void EndMode3D(void);                                       // Ends 3D mode and returns to default 2D orthographic mode
//void BeginTextureMode(RenderTexture2D target);              // Begin drawing to render texture
//void EndTextureMode(void);                                  // Ends drawing to render texture
//void BeginShaderMode(Shader shader);                        // Begin custom shader drawing
//void EndShaderMode(void);                                   // End custom shader drawing (use default shader)
//void BeginBlendMode(int mode);                              // Begin blending mode (alpha, additive, multiplied, subtract, custom)
//void EndBlendMode(void);                                    // End blending mode (reset to default: alpha blending)
//void BeginScissorMode(int x, int y, int width, int height); // Begin scissor mode (define screen area for following drawing)
//void EndScissorMode(void);                                  // End scissor mode
//void BeginVrStereoMode(VrStereoConfig config);              // Begin stereo rendering (requires VR simulator)
//void EndVrStereoMode(void);                                 // End stereo rendering (requires VR simulator)
void  Begin(int mode);
void BeginTextureMode(RenderTexture2D target);
void EndTextureMode(void);
// Finish vertex providing
void End(void);

void BeginShaderMode(Shader shader);

void EndShaderMode(void);

void DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Color tint);
