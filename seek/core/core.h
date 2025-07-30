#pragma once
#include <section/martrix/cg_def.h>
#include <section/graph/model_def.h>
using namespace Seek;
//Core API
// Window-related functions
void InitWindow(int width, int height, const char* title);  // Initialize window and OpenGL context
void CloseWindow(void);                                     // Close window and unload OpenGL context
bool WindowShouldClose(void);                               // Check if application should close (KEY_ESCAPE pressed or windows close icon clicked)
bool IsWindowReady(void);                                   // Check if window has been initialized successfully
bool IsWindowFullscreen(void);                              // Check if window is currently fullscreen
bool IsWindowHidden(void);                                  // Check if window is currently hidden
bool IsWindowMinimized(void);                               // Check if window is currently minimized
bool IsWindowMaximized(void);                               // Check if window is currently maximized
bool IsWindowFocused(void);                                 // Check if window is currently focused
bool IsWindowResized(void);                                 // Check if window has been resized last frame
bool IsWindowState(unsigned int flag);                      // Check if one specific window flag is enabled
// Check if a mouse button has been pressed once
bool IsMouseButtonPressed(int button);
// Check if a mouse button is being pressed
bool IsMouseButtonDown(int button);
// Check if a mouse button has been released once
bool IsMouseButtonReleased(int button);
// Check if a mouse button is NOT being pressed
bool IsMouseButtonUp(int button);
// Get mouse delta between frames
Vector2 GetMouseDelta(void);
// Set mouse offset
// NOTE: Useful when rendering to different size targets
void SetMouseOffset(int offsetX, int offsetY);
// Get mouse position XY
Vector2 GetMousePosition(void);
// Get mouse position X
int GetMouseX(void);
// Get mouse position Y
int GetMouseY(void);
// Get time in seconds for last frame drawn (delta time)
float GetFrameTime(void);
// Check if a key is being pressed (key held down)
bool IsKeyDown(int key);
// Check if a gamepad is available
bool IsGamepadAvailable(int gamepad);
// Disables cursor (lock cursor)
void DisableCursor(void);
// Enables cursor (unlock cursor)
void EnableCursor(void);
// Timing-related functions
void SetTargetFPS(int fps);
// Get current FPS
// NOTE: We calculate an average framerate
int GetFPS(void);
// Get time in seconds for last frame drawn (delta time)
float GetFrameTime(void);
//void SetWindowState(unsigned int flags);                    // Set window configuration state using flags
//void ClearWindowState(unsigned int flags);                  // Clear window configuration state flags
//void ToggleFullscreen(void);                                // Toggle window state: fullscreen/windowed, resizes monitor to match window resolution
//void ToggleBorderlessWindowed(void);                        // Toggle window state: borderless windowed, resizes window to match monitor resolution
//void MaximizeWindow(void);                                  // Set window state: maximized, if resizable
//void MinimizeWindow(void);                                  // Set window state: minimized, if resizable
//void RestoreWindow(void);                                   // Restore window from being minimized/maximized
//void SetWindowIcon(Image image);                            // Set icon for window (single image, RGBA 32bit)
//void SetWindowIcons(Image* images, int count);              // Set icon for window (multiple images, RGBA 32bit)
//void SetWindowTitle(const char* title);                     // Set title for window
//void SetWindowPosition(int x, int y);                       // Set window position on screen
//void SetWindowMonitor(int monitor);                         // Set monitor for the current window
//void SetWindowMinSize(int width, int height);               // Set window minimum dimensions (for FLAG_WINDOW_RESIZABLE)
//void SetWindowMaxSize(int width, int height);               // Set window maximum dimensions (for FLAG_WINDOW_RESIZABLE)
//void SetWindowSize(int width, int height);                  // Set window dimensions
//void SetWindowOpacity(float opacity);                       // Set window opacity [0.0f..1.0f]
//void SetWindowFocused(void);                                // Set window focused
//void* GetWindowHandle(void);                                // Get native window handle
//int GetScreenWidth(void);                                   // Get current screen width
//int GetScreenHeight(void);                                  // Get current screen height
//int GetRenderWidth(void);                                   // Get current render width (it considers HiDPI)
//int GetRenderHeight(void);                                  // Get current render height (it considers HiDPI)
//int GetMonitorCount(void);                                  // Get number of connected monitors
//int GetCurrentMonitor(void);                                // Get current monitor where window is placed
//Vector2 GetMonitorPosition(int monitor);                    // Get specified monitor position
//int GetMonitorWidth(int monitor);                           // Get specified monitor width (current video mode used by monitor)
//int GetMonitorHeight(int monitor);                          // Get specified monitor height (current video mode used by monitor)
//int GetMonitorPhysicalWidth(int monitor);                   // Get specified monitor physical width in millimetres
//int GetMonitorPhysicalHeight(int monitor);                  // Get specified monitor physical height in millimetres
//int GetMonitorRefreshRate(int monitor);                     // Get specified monitor refresh rate
//Vector2 GetWindowPosition(void);                            // Get window position XY on monitor
//Vector2 GetWindowScaleDPI(void);                            // Get window scale DPI factor
//const char* GetMonitorName(int monitor);                    // Get the human-readable, UTF-8 encoded name of the specified monitor
//void SetClipboardText(const char* text);                    // Set clipboard text content
//const char* GetClipboardText(void);                         // Get clipboard text content
//Image GetClipboardImage(void);                              // Get clipboard image content
//void EnableEventWaiting(void);                              // Enable waiting for events on EndDrawing(), no automatic event polling
//void DisableEventWaiting(void);                             // Disable waiting for events on EndDrawing(), automatic events polling
// Set viewport for a provided width and height
void SetupViewport(int width, int height);
void WaitTime(double seconds);



// Screen-space-related functions
#define GetMouseRay GetScreenToWorldRay   
Ray GetScreenToWorldRay(Vector2 position, Camera camera);
Ray GetScreenToWorldRayEx(Vector2 position, Camera camera, int width, int height); // Get a ray trace from screen position (i.e mouse) in a viewport
Vector2 GetWorldToScreenEx(Vector3 position, Camera camera, int width, int height); // Get size position for a 3d world space position
Vector2 GetWorldToScreen2D(Vector2 position, Camera2D camera);    // Get the screen space position for a 2d camera world space position
Vector2 GetScreenToWorld2D(Vector2 position, Camera2D camera);    // Get the world space position for a 2d camera screen space position
Matrix GetCameraMatrix(Camera camera);                            // Get camera transform matrix (view matrix)
Matrix GetCameraMatrix2D(Camera2D camera);                        // Get camera 2d transform matrix







