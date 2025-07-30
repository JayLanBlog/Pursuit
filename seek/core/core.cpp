#include "core.h"
#include "rmd/gl/gl_include.h"
#include <logger/logger.h>
#include <math/pr_math.h>
#include <core/core_global.h>
#include <core/platform/wcore_sdl.h>
#include <file/sys_file.h>
#include <section/enum/config_em.h>
#include <core/text/draw_text.h>
#include <section/martrix/cg_def.h>
using namespace System;
using namespace Seek;
using namespace DRAW::GL;
CoreData CORE = { 0 };
extern bool isGpuReady = false;
//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const char* pursuitlib_version = "1.1";
using namespace PMath;
// Initialize window and OpenGL context
void InitWindow(int width, int height, const char* title) {

	TRACELOG(LOG_INFO, "Initializing lib %s", pursuitlib_version);
#if defined(PLATFORM_DESKTOP_GLFW)
    TRACELOG(LOG_INFO, "Platform backend: DESKTOP (GLFW)");
#elif defined(PLATFORM_DESKTOP_SDL)
    TRACELOG(LOG_INFO, "Platform backend: DESKTOP (SDL)");
#elif defined(PLATFORM_DESKTOP_RGFW)
    TRACELOG(LOG_INFO, "Platform backend: DESKTOP (RGFW)");
#elif defined(PLATFORM_WEB_RGFW)
    TRACELOG(LOG_INFO, "Platform backend: WEB (RGFW) (HTML5)");
#elif defined(PLATFORM_WEB)
    TRACELOG(LOG_INFO, "Platform backend: WEB (HTML5)");
#elif defined(PLATFORM_DRM)
    TRACELOG(LOG_INFO, "Platform backend: NATIVE DRM");
#elif defined(PLATFORM_ANDROID)
    TRACELOG(LOG_INFO, "Platform backend: ANDROID");
#else
    // TODO: Include your custom platform backend!
    // i.e software rendering backend or console backend!
    TRACELOG(LOG_INFO, "Platform backend: CUSTOM");
#endif

    TRACELOG(LOG_INFO, "Supported lib modules:");
    TRACELOG(LOG_INFO, "    > rcore:..... loaded (mandatory)");
    TRACELOG(LOG_INFO, "    > pursit:...... loaded (mandatory)");
#if defined(SUPPORT_MODULE_RSHAPES)
    TRACELOG(LOG_INFO, "    > rshapes:... loaded (optional)");
#else
    TRACELOG(LOG_INFO, "    > rshapes:... not loaded (optional)");
#endif
#if defined(SUPPORT_MODULE_RTEXTURES)
    TRACELOG(LOG_INFO, "    > rtextures:. loaded (optional)");
#else
    TRACELOG(LOG_INFO, "    > rtextures:. not loaded (optional)");
#endif
#if defined(SUPPORT_MODULE_RTEXT)
    TRACELOG(LOG_INFO, "    > rtext:..... loaded (optional)");
#else
    TRACELOG(LOG_INFO, "    > rtext:..... not loaded (optional)");
#endif
#if defined(SUPPORT_MODULE_RMODELS)
    TRACELOG(LOG_INFO, "    > rmodels:... loaded (optional)");
#else
    TRACELOG(LOG_INFO, "    > rmodels:... not loaded (optional)");
#endif
#if defined(SUPPORT_MODULE_RAUDIO)
    TRACELOG(LOG_INFO, "    > raudio:.... loaded (optional)");
#else
    TRACELOG(LOG_INFO, "    > raudio:.... not loaded (optional)");
#endif
    // Initialize window data
    CORE.Window.screen.width = width;
    CORE.Window.screen.height = height;
    CORE.Window.eventWaiting = false;
    CORE.Window.screenScale = MatrixIdentity();
    if ((title != NULL) && (title[0] != 0)) CORE.Window.title = title;
    // Initialize global input state
    memset(&CORE.Input, 0, sizeof(CORE.Input));     // Reset CORE.Input structure to 0
    CORE.Input.Keyboard.exitKey = KEY_ESCAPE;
    CORE.Input.Mouse.scale = { 1.0f, 1.0f };
    CORE.Input.Mouse.cursor = MOUSE_CURSOR_ARROW;
    CORE.Input.Gamepad.lastButtonPressed = GAMEPAD_BUTTON_UNKNOWN;
    // Initialize platform
   //--------------------------------------------------------------
    int result = InitPlatform();
    // Initialize PLGL default data (buffers and shaders)
    // NOTE: CORE.Window.currentFbo.width and CORE.Window.currentFbo.height not used, just stored as globals in PLGL
    plglInit(CORE.Window.currentFbo.width, CORE.Window.currentFbo.height);
    isGpuReady = true; // Flag to note GPU has been initialized successfully


    // Setup default viewport
    SetupViewport(CORE.Window.currentFbo.width, CORE.Window.currentFbo.height);
#if defined(SUPPORT_MODULE_RTEXT)
#if defined(SUPPORT_DEFAULT_FONT)
    // Load default font
    // WARNING: External function: Module required: rtext
    LoadFontDefault();
#if defined(SUPPORT_MODULE_RSHAPES)
    // Set font white rectangle for shapes drawing, so shapes and text can be batched together
    // WARNING: rshapes module is required, if not available, default internal white rectangle is used
   Seek::Rectangle  rec = GetFontDefault().recs[95];
    if (CORE.Window.flags & FLAG_MSAA_4X_HINT)
    {
        // NOTE: We try to maxime rec padding to avoid pixel bleeding on MSAA filtering
      //  SetShapesTexture(GetFontDefault().texture, { rec.x + 2, rec.y + 2, 1, 1 });
    }
    else
    {
        // NOTE: We set up a 1px padding on char rectangle to avoid pixel bleeding
        //SetShapesTexture(GetFontDefault().texture,  { rec.x + 1, rec.y + 1, rec.width - 2, rec.height - 2 });
    }
#endif
#endif
#else
#if defined(SUPPORT_MODULE_RSHAPES)
    // Set default texture and rectangle to be used for shapes drawing
    // NOTE: PLGL default texture is a 1x1 pixel UNCOMPRESSED_R8G8B8A8
    Texture2D texture = { rlGetTextureIdDefault(), 1, 1, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
    SetShapesTexture(texture, (Rectangle) { 0.0f, 0.0f, 1.0f, 1.0f });    // WARNING: Module required: rshapes
#endif
#endif
    CORE.Time.frameCounter = 0;
    CORE.Window.shouldClose = false;
    // Initialize random seed
   // SetRandomSeed((unsigned int)time(NULL));
    TRACELOG(LOG_INFO, "SYSTEM: Working Directory: %s", GetWorkingDirectory());
}



// Set viewport for a provided width and height
void SetupViewport(int width, int height) {
    CORE.Window.render.width = width;
    CORE.Window.render.height = height;

    // Set viewport width and height
    // NOTE: We consider render size (scaled) and offset in case black bars are required and
    // render area does not match full display area (this situation is only applicable on fullscreen mode)
#if defined(__APPLE__)
    Vector2 scale = GetWindowScaleDPI();
    rlViewport(CORE.Window.renderOffset.x / 2 * scale.x, CORE.Window.renderOffset.y / 2 * scale.y, (CORE.Window.render.width) * scale.x, (CORE.Window.render.height) * scale.y);
#else
    Viewport(CORE.Window.renderOffset.x / 2, CORE.Window.renderOffset.y / 2, CORE.Window.render.width, CORE.Window.render.height);
#endif
    MatrixMode(PL_PROJECTION);        // Switch to projection matrix
    LoadIdentity();                   // Reset current matrix (projection)
    // Set orthographic projection to current framebuffer size
    // NOTE: Configured top-left corner as (0, 0)
    Ortho(0, CORE.Window.render.width, CORE.Window.render.height, 0, 0.0f, 1.0f);
    MatrixMode(PL_MODELVIEW);         // Switch back to modelview matrix
    LoadIdentity();                   // Reset current matrix (modelview)
}


// Close window and unload OpenGL context
void CloseWindow(void) {
#if defined(SUPPORT_GIF_RECORDING)
    if (gifRecording)
    {
        MsfGifResult result = msf_gif_end(&gifState);
        msf_gif_free(result);
        gifRecording = false;
    }
#endif
#if defined(SUPPORT_MODULE_RTEXT) && defined(SUPPORT_DEFAULT_FONT)
    UnloadFontDefault();        // WARNING: Module required: rtext
#endif
    plglClose();
    // De-initialize platform
   //--------------------------------------------------------------
    ClosePlatform();
    //--------------------------------------------------------------
    CORE.Window.ready = false;
    TRACELOG(LOG_INFO, "Window closed successfully");
}
// Check if application should close
bool WindowShouldClose(void)
{
    if (CORE.Window.ready) return CORE.Window.shouldClose;
    else return true;
}

// Check if window has been initialized successfully
bool IsWindowReady(void)
{
    return CORE.Window.ready;
}


// Check if window is currently fullscreen
bool IsWindowFullscreen(void)
{
    return CORE.Window.fullscreen;
}


// Check if window is currently hidden
bool IsWindowHidden(void)
{
    return ((CORE.Window.flags & FLAG_WINDOW_HIDDEN) > 0);
}

// Check if window has been minimized
bool IsWindowMinimized(void)
{
    return ((CORE.Window.flags & FLAG_WINDOW_MINIMIZED) > 0);
}


// Check if window has been maximized
bool IsWindowMaximized(void)
{
    return ((CORE.Window.flags & FLAG_WINDOW_MAXIMIZED) > 0);
}


// Check if window has the focus
bool IsWindowFocused(void)
{
    return ((CORE.Window.flags & FLAG_WINDOW_UNFOCUSED) == 0);
}


// Check if window has been resizedLastFrame
bool IsWindowResized(void)
{
    return CORE.Window.resizedLastFrame;
}


// Check if one specific window flag is enabled
bool IsWindowState(unsigned int flag)
{
    return ((CORE.Window.flags & flag) > 0);
}


// Get current screen width
int GetScreenWidth(void)
{
    return CORE.Window.screen.width;
}


// Get current screen height
int GetScreenHeight(void)
{
    return CORE.Window.screen.height;
}

// Get current render width which is equal to screen width*dpi scale
int GetRenderWidth(void)
{
    int width = 0;
#if defined(__APPLE__)
    Vector2 scale = GetWindowScaleDPI();
    width = (int)((float)CORE.Window.render.width * scale.x);
#else
    width = CORE.Window.render.width;
#endif
    return width;
}


// Get current screen height which is equal to screen height*dpi scale
int GetRenderHeight(void)
{
    int height = 0;
#if defined(__APPLE__)
    Vector2 scale = GetWindowScaleDPI();
    height = (int)((float)CORE.Window.render.height * scale.y);
#else
    height = CORE.Window.render.height;
#endif
    return height;
}

// Enable waiting for events on EndDrawing(), no automatic event polling
void EnableEventWaiting(void)
{
    CORE.Window.eventWaiting = true;
}

// Disable waiting for events on EndDrawing(), automatic events polling
void DisableEventWaiting(void)
{
    CORE.Window.eventWaiting = false;
}
// Check if cursor is not visible
bool IsCursorHidden(void)
{
    return CORE.Input.Mouse.cursorHidden;
}

// Check if cursor is on the current screen
bool IsCursorOnScreen(void)
{
    return CORE.Input.Mouse.cursorOnScreen;
}
#if defined(_WIN32)
#include <windows.h>
#endif
#include <section/enum/camera_em.h>
void WaitTime(double seconds)
{
    if (seconds < 0) return;    // Security check

#if defined(SUPPORT_BUSY_WAIT_LOOP) || defined(SUPPORT_PARTIALBUSY_WAIT_LOOP)
    double destinationTime = GetTime() + seconds;
#endif

#if defined(SUPPORT_BUSY_WAIT_LOOP)
    while (GetTime() < destinationTime) {}
#else
#if defined(SUPPORT_PARTIALBUSY_WAIT_LOOP)
    double sleepSeconds = seconds - seconds * 0.05;  // NOTE: We reserve a percentage of the time for busy waiting
#else
    double sleepSeconds = seconds;
#endif

    // System halt functions
#if defined(_WIN32)
    Sleep((sleepSeconds * 1000.0));
#endif
#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__EMSCRIPTEN__)
    struct timespec req = { 0 };
    time_t sec = sleepSeconds;
    long nsec = (sleepSeconds - sec) * 1000000000L;
    req.tv_sec = sec;
    req.tv_nsec = nsec;

    // NOTE: Use nanosleep() on Unix platforms... usleep() it's deprecated
    while (nanosleep(&req, &req) == -1) continue;
#endif
#if defined(__APPLE__)
    usleep(sleepSeconds * 1000000.0);
#endif

#if defined(SUPPORT_PARTIALBUSY_WAIT_LOOP)
    while (GetTime() < destinationTime) {}
#endif
#endif
}



// Check if a mouse button has been pressed once
bool IsMouseButtonPressed(int button) {
    bool pressed = false;

    if ((CORE.Input.Mouse.currentButtonState[button] == 1) && (CORE.Input.Mouse.previousButtonState[button] == 0)) pressed = true;

    // Map touches to mouse buttons checking
    if ((CORE.Input.Touch.currentTouchState[button] == 1) && (CORE.Input.Touch.previousTouchState[button] == 0)) pressed = true;

    return pressed;
}
// Check if a mouse button is being pressed
bool IsMouseButtonDown(int button) {
    bool down = false;

    if (CORE.Input.Mouse.currentButtonState[button] == 1) down = true;

    // NOTE: Touches are considered like mouse buttons
    if (CORE.Input.Touch.currentTouchState[button] == 1) down = true;

    return down;
}
// Check if a mouse button has been released once
bool IsMouseButtonReleased(int button) {
    bool released = false;

    if ((CORE.Input.Mouse.currentButtonState[button] == 0) && (CORE.Input.Mouse.previousButtonState[button] == 1)) released = true;

    // Map touches to mouse buttons checking
    if ((CORE.Input.Touch.currentTouchState[button] == 0) && (CORE.Input.Touch.previousTouchState[button] == 1)) released = true;

    return released;
}
// Check if a mouse button is NOT being pressed
bool IsMouseButtonUp(int button) {
    bool up = false;

    if (CORE.Input.Mouse.currentButtonState[button] == 0) up = true;

    // NOTE: Touches are considered like mouse buttons
    if (CORE.Input.Touch.currentTouchState[button] == 0) up = true;

    return up;
}
// Get mouse delta between frames
Vector2 GetMouseDelta(void) {
    Vector2 delta = { 0 };

    delta.x = CORE.Input.Mouse.currentPosition.x - CORE.Input.Mouse.previousPosition.x;
    delta.y = CORE.Input.Mouse.currentPosition.y - CORE.Input.Mouse.previousPosition.y;

    return delta;
}
// Set mouse offset
// NOTE: Useful when rendering to different size targets
void SetMouseOffset(int offsetX, int offsetY) {
    CORE.Input.Mouse.offset = { (float)offsetX, (float)offsetY };
}
// Get mouse position XY
Vector2 GetMousePosition(void) {
    Vector2 position = { 0 };

    position.x = (CORE.Input.Mouse.currentPosition.x + CORE.Input.Mouse.offset.x) * CORE.Input.Mouse.scale.x;
    position.y = (CORE.Input.Mouse.currentPosition.y + CORE.Input.Mouse.offset.y) * CORE.Input.Mouse.scale.y;

    return position;
}
// Get mouse position X
int GetMouseX(void) {
    int mouseX = (int)((CORE.Input.Mouse.currentPosition.x + CORE.Input.Mouse.offset.x) * CORE.Input.Mouse.scale.x);
    return mouseX;
}
// Get mouse position Y
int GetMouseY(void) {
    int mouseY = (int)((CORE.Input.Mouse.currentPosition.y + CORE.Input.Mouse.offset.y) * CORE.Input.Mouse.scale.y);
    return mouseY;
}

// Get time in seconds for last frame drawn (delta time)
float GetFrameTime(void) {
    return (float)CORE.Time.frame;
}
// Check if a key is being pressed (key held down)
bool IsKeyDown(int key)
{
    bool down = false;
    if ((key > 0) && (key < MAX_KEYBOARD_KEYS))
    {
        if (CORE.Input.Keyboard.currentKeyState[key] == 1) down = true;
    }
    return down;
}
// Check if a gamepad is available
bool IsGamepadAvailable(int gamepad)
{
    bool result = false;
    if ((gamepad < MAX_GAMEPADS) && CORE.Input.Gamepad.ready[gamepad]) result = true;
    return result;
}


// Set target FPS (maximum)
void SetTargetFPS(int fps)
{
    if (fps < 1) CORE.Time.target = 0.0;
    else CORE.Time.target = 1.0 / (double)fps;

    TRACELOG(LOG_INFO, "TIMER: Target time per frame: %02.03f milliseconds", (float)CORE.Time.target * 1000.0f);
}

int GetFPS(void)
{
    int fps = 0;
#if !defined(SUPPORT_CUSTOM_FRAME_CONTROL)
#define FPS_CAPTURE_FRAMES_COUNT    30      // 30 captures
#define FPS_AVERAGE_TIME_SECONDS   0.5f     // 500 milliseconds
#define FPS_STEP (FPS_AVERAGE_TIME_SECONDS/FPS_CAPTURE_FRAMES_COUNT)
    static int index = 0;
    static float history[FPS_CAPTURE_FRAMES_COUNT] = { 0 };
    static float average = 0, last = 0;
    float fpsFrame = GetFrameTime();

    // if we reset the window, reset the FPS info
    if (CORE.Time.frameCounter == 0)
    {
        average = 0;
        last = 0;
        index = 0;

        for (int i = 0; i < FPS_CAPTURE_FRAMES_COUNT; i++) history[i] = 0;
    }
    if (fpsFrame == 0) return 0;
    if ((GetTime() - last) > FPS_STEP)
    {
        last = (float)GetTime();
        index = (index + 1) % FPS_CAPTURE_FRAMES_COUNT;
        average -= history[index];
        history[index] = fpsFrame / FPS_CAPTURE_FRAMES_COUNT;
        average += history[index];
    }
    fps = (int)roundf(1.0f / average);
#endif
    return fps;
}

Ray GetScreenToWorldRay(Vector2 position, Camera camera) {
    Ray ray = GetScreenToWorldRayEx(position, camera, GetScreenWidth(), GetScreenHeight());
    return ray;
}

Ray GetScreenToWorldRayEx(Vector2 position, Camera camera, int width, int height) {
    Ray ray = { 0 };

    // Calculate normalized device coordinates
    // NOTE: y value is negative
    float x = (2.0f * position.x) / (float)width - 1.0f;
    float y = 1.0f - (2.0f * position.y) / (float)height;
    float z = 1.0f;
    // Store values in a vector
    Vector3 deviceCoords = { x, y, z };

    // Calculate view matrix from camera look at
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);

    Matrix matProj = MatrixIdentity();

    if (camera.projection == CAMERA_PERSPECTIVE)
    {
        // Calculate projection matrix from perspective
        matProj = MatrixPerspective(camera.fovy * DEG2RAD, ((double)width / (double)height), GetCullDistanceNear(), GetCullDistanceFar());
    }
    else if (camera.projection == CAMERA_ORTHOGRAPHIC)
    {
        double aspect = (double)width / (double)height;
        double top = camera.fovy / 2.0;
        double right = top * aspect;

        // Calculate projection matrix from orthographic
        matProj = MatrixOrtho(-right, right, -top, top, 0.01, 1000.0);
    }

    // Unproject far/near points
    Vector3 nearPoint = Vector3Unproject( { deviceCoords.x, deviceCoords.y, 0.0f }, matProj, matView);
    Vector3 farPoint = Vector3Unproject({ deviceCoords.x, deviceCoords.y, 1.0f }, matProj, matView);
    // Unproject the mouse cursor in the near plane
    // We need this as the source position because orthographic projects,
    // compared to perspective doesn't have a convergence point,
    // meaning that the "eye" of the camera is more like a plane than a point
    Vector3 cameraPlanePointerPos = Vector3Unproject({ deviceCoords.x, deviceCoords.y, -1.0f }, matProj, matView);
    // Calculate normalized direction vector
    Vector3 direction = Vector3Normalize(Vector3Subtract(farPoint, nearPoint));

    if (camera.projection == CAMERA_PERSPECTIVE) ray.position = camera.position;
    else if (camera.projection == CAMERA_ORTHOGRAPHIC) ray.position = cameraPlanePointerPos;

    // Apply calculated vectors to ray
    ray.direction = direction;

    return ray;

}

Vector2 GetWorldToScreenEx(Vector3 position, Camera camera, int width, int height) {
    // Calculate projection matrix (from perspective instead of frustum
    Matrix matProj = MatrixIdentity();

    if (camera.projection == CAMERA_PERSPECTIVE)
    {
        // Calculate projection matrix from perspective
        matProj = MatrixPerspective(camera.fovy * DEG2RAD, ((double)width / (double)height), GetCullDistanceNear(), GetCullDistanceFar());
    }
    else if (camera.projection == CAMERA_ORTHOGRAPHIC)
    {
        double aspect = (double)width / (double)height;
        double top = camera.fovy / 2.0;
        double right = top * aspect;
        // Calculate projection matrix from orthographic
        matProj = MatrixOrtho(-right, right, -top, top, GetCullDistanceNear(), GetCullDistanceFar());
    }

    // Calculate view matrix from camera look at (and transpose it)
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);
    // TODO: Why not use Vector3Transform(Vector3 v, Matrix mat)?

   // Convert world position vector to quaternion
    Quaternion worldPos = { position.x, position.y, position.z, 1.0f };
    // Transform world position to view
    worldPos = QuaternionTransform(worldPos, matView);

    // Transform result to projection (clip space position)
    worldPos = QuaternionTransform(worldPos, matProj);
    // Calculate normalized device coordinates (inverted y)
    Vector3 ndcPos = { worldPos.x / worldPos.w, -worldPos.y / worldPos.w, worldPos.z / worldPos.w };

    // Calculate 2d screen position vector
    Vector2 screenPosition = { (ndcPos.x + 1.0f) / 2.0f * (float)width, (ndcPos.y + 1.0f) / 2.0f * (float)height };

    return screenPosition;


}

Vector2 GetWorldToScreen2D(Vector2 position, Camera2D camera) {
    Matrix matCamera = GetCameraMatrix2D(camera);
    Vector3 transform = Vector3Transform({ position.x, position.y, 0 }, matCamera);
    return { transform.x, transform.y };
}
Vector2 GetScreenToWorld2D(Vector2 position, Camera2D camera) {
    Matrix invMatCamera = MatrixInvert(GetCameraMatrix2D(camera));
    Vector3 transform = Vector3Transform({ position.x, position.y, 0 }, invMatCamera);
    return { transform.x, transform.y };
}
Matrix GetCameraMatrix(Camera camera) {
    Matrix mat = MatrixLookAt(camera.position, camera.target, camera.up);

    return mat;
}
Matrix GetCameraMatrix2D(Camera2D camera) {
    Matrix matTransform = { 0 };
    // The camera in world-space is set by
    //   1. Move it to target
    //   2. Rotate by -rotation and scale by (1/zoom)
    //      When setting higher scale, it's more intuitive for the world to become bigger (= camera become smaller),
    //      not for the camera getting bigger, hence the invert. Same deal with rotation
    //   3. Move it by (-offset);
    //      Offset defines target transform relative to screen, but since we're effectively "moving" screen (camera)
    //      we need to do it into opposite direction (inverse transform)

    // Having camera transform in world-space, inverse of it gives the modelview transform
    // Since (A*B*C)' = C'*B'*A', the modelview is
    //   1. Move to offset
    //   2. Rotate and Scale
    //   3. Move by -target
    Matrix matOrigin = MatrixTranslate(-camera.target.x, -camera.target.y, 0.0f);
    Matrix matRotation = MatrixRotate( { 0.0f, 0.0f, 1.0f }, camera.rotation* DEG2RAD);
    Matrix matScale = MatrixScale(camera.zoom, camera.zoom, 1.0f);
    Matrix matTranslation = MatrixTranslate(camera.offset.x, camera.offset.y, 0.0f);

    matTransform = MatrixMultiply(MatrixMultiply(matOrigin, MatrixMultiply(matScale, matRotation)), matTranslation);

    return matTransform;
}
