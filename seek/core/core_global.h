#pragma once
#include <config/platform_def.h>
#include <section/martrix/cg_def.h>
#include <section/graph/material_def.h>
#include <section/enum/key_em.h>

using namespace Seek;

#ifndef MAX_GAMEPAD_NAME_LENGTH
#define MAX_GAMEPAD_NAME_LENGTH      128        // Maximum number of characters in a gamepad name (byte size)
#endif
// core: Configuration values
//------------------------------------------------------------------------------------
#define MAX_FILEPATH_CAPACITY        8192       // Maximum file paths capacity
#define MAX_FILEPATH_LENGTH          4096       // Maximum length for filepaths (Linux PATH_MAX default value)

#define MAX_KEYBOARD_KEYS             512       // Maximum number of keyboard keys supported
#define MAX_MOUSE_BUTTONS               8       // Maximum number of mouse buttons supported
#define MAX_GAMEPADS                    4       // Maximum number of gamepads supported
#define MAX_GAMEPAD_AXES                8       // Maximum number of axes supported (per gamepad)
#define MAX_GAMEPAD_BUTTONS            32       // Maximum number of buttons supported (per gamepad)
#define MAX_GAMEPAD_VIBRATION_TIME      2.0f    // Maximum vibration time in seconds
#define MAX_TOUCH_POINTS                8       // Maximum number of touch points supported
#define MAX_KEY_PRESSED_QUEUE          16       // Maximum number of keys in the key input queue
#define MAX_CHAR_PRESSED_QUEUE         16       // Maximum number of characters in the char input queue

#define MAX_DECOMPRESSION_SIZE         64       // Max size allocated for decompression in MB

#define MAX_AUTOMATION_EVENTS       16384       // Maximum number of automation events to record



//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct { int x; int y; } Point;
typedef struct { unsigned int width; unsigned int height; } Size;
// Core global state context data
typedef struct CoreData {
    struct {
        const char* title;                  // Window text title const pointer
        unsigned int flags;                 // Configuration flags (bit based), keeps window state
        bool ready;                         // Check if window has been initialized successfully
        bool fullscreen;                    // Check if fullscreen mode is enabled
        bool shouldClose;                   // Check if window set for closing
        bool resizedLastFrame;              // Check if window has been resized last frame
        bool eventWaiting;                  // Wait for events before ending frame
        bool usingFbo;                      // Using FBO (RenderTexture) for rendering instead of default framebuffer
        Point position;                     // Window position (required on fullscreen toggle)
        Point previousPosition;             // Window previous position (required on borderless windowed toggle)
        Size display;                       // Display width and height (monitor, device-screen, LCD, ...)
        Size screen;                        // Screen width and height (used render area)
        Size previousScreen;                // Screen previous width and height (required on borderless windowed toggle)
        Size currentFbo;                    // Current render width and height (depends on active fbo)
        Size render;                        // Framebuffer width and height (render area, including black bars if required)
        Point renderOffset;                 // Offset from render area (must be divided by 2)
        Size screenMin;                     // Screen minimum width and height (for resizable window)
        Size screenMax;                     // Screen maximum width and height (for resizable window)
        Matrix screenScale;                 // Matrix to scale screen (framebuffer rendering)
        char** dropFilepaths;               // Store dropped files paths pointers (provided by GLFW)
        unsigned int dropFileCount;         // Count dropped files strings
    } Window;

    struct {
        const char* basePath;               // Base path for data storage
    } Storage;
    struct {
        struct {
            int exitKey;                    // Default exit key
            char currentKeyState[MAX_KEYBOARD_KEYS];        // Registers current frame key state
            char previousKeyState[MAX_KEYBOARD_KEYS];       // Registers previous frame key state
            // NOTE: Since key press logic involves comparing prev vs cur key state, we need to handle key repeats specially
            char keyRepeatInFrame[MAX_KEYBOARD_KEYS];       // Registers key repeats for current frame
            int keyPressedQueue[MAX_KEY_PRESSED_QUEUE];     // Input keys queue
            int keyPressedQueueCount;       // Input keys queue count
            int charPressedQueue[MAX_CHAR_PRESSED_QUEUE];   // Input characters queue (unicode)
            int charPressedQueueCount;      // Input characters queue count
        } Keyboard;
        struct {
            Vector2 offset;                 // Mouse offset
            Vector2 scale;                  // Mouse scaling
            Vector2 currentPosition;        // Mouse position on screen
            Vector2 previousPosition;       // Previous mouse position
            int cursor;                     // Tracks current mouse cursor
            bool cursorHidden;              // Track if cursor is hidden
            bool cursorOnScreen;            // Tracks if cursor is inside client area
            char currentButtonState[MAX_MOUSE_BUTTONS];     // Registers current mouse button state
            char previousButtonState[MAX_MOUSE_BUTTONS];    // Registers previous mouse button state
            Vector2 currentWheelMove;       // Registers current mouse wheel variation
            Vector2 previousWheelMove;      // Registers previous mouse wheel variation

        } Mouse;
        struct {
            int pointCount;                             // Number of touch points active
            int pointId[MAX_TOUCH_POINTS];              // Point identifiers
            Vector2 position[MAX_TOUCH_POINTS];         // Touch position on screen
            char currentTouchState[MAX_TOUCH_POINTS];   // Registers current touch state
            char previousTouchState[MAX_TOUCH_POINTS];  // Registers previous touch state

        } Touch;
        struct {
            int lastButtonPressed;          // Register last gamepad button pressed
            int axisCount[MAX_GAMEPADS];    // Register number of available gamepad axes
            bool ready[MAX_GAMEPADS];       // Flag to know if gamepad is ready
            char name[MAX_GAMEPADS][MAX_GAMEPAD_NAME_LENGTH];               // Gamepad name holder
            char currentButtonState[MAX_GAMEPADS][MAX_GAMEPAD_BUTTONS];     // Current gamepad buttons state
            char previousButtonState[MAX_GAMEPADS][MAX_GAMEPAD_BUTTONS];    // Previous gamepad buttons state
            float axisState[MAX_GAMEPADS][MAX_GAMEPAD_AXES];                // Gamepad axes state
        } Gamepad;

    } Input;
    struct {
        double current;                     // Current time measure
        double previous;                    // Previous time measure
        double update;                      // Time measure for frame update
        double draw;                        // Time measure for frame draw
        double frame;                       // Time measure for one frame
        double target;                      // Desired time for one frame, if 0 not applied
        unsigned long long int base;        // Base time measure for hi-res timer (PLATFORM_ANDROID, PLATFORM_DRM)
        unsigned int frameCounter;          // Frame counter
    } Time;

}CoreData;

// VrStereoConfig, VR stereo rendering configuration for simulator
typedef struct VrStereoConfig {
    Matrix projection[2];           // VR projection matrices (per eye)
    Matrix viewOffset[2];           // VR view offset matrices (per eye)
    float leftLensCenter[2];        // VR left lens center
    float rightLensCenter[2];       // VR right lens center
    float leftScreenCenter[2];      // VR left screen center
    float rightScreenCenter[2];     // VR right screen center
    float scale[2];                 // VR distortion scale
    float scaleIn[2];               // VR distortion scale in
} VrStereoConfig;


// Gamepad buttons
typedef enum {
    GAMEPAD_BUTTON_UNKNOWN = 0,         // Unknown button, just for error checking
    GAMEPAD_BUTTON_LEFT_FACE_UP,        // Gamepad left DPAD up button
    GAMEPAD_BUTTON_LEFT_FACE_RIGHT,     // Gamepad left DPAD right button
    GAMEPAD_BUTTON_LEFT_FACE_DOWN,      // Gamepad left DPAD down button
    GAMEPAD_BUTTON_LEFT_FACE_LEFT,      // Gamepad left DPAD left button
    GAMEPAD_BUTTON_RIGHT_FACE_UP,       // Gamepad right button up (i.e. PS3: Triangle, Xbox: Y)
    GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,    // Gamepad right button right (i.e. PS3: Circle, Xbox: B)
    GAMEPAD_BUTTON_RIGHT_FACE_DOWN,     // Gamepad right button down (i.e. PS3: Cross, Xbox: A)
    GAMEPAD_BUTTON_RIGHT_FACE_LEFT,     // Gamepad right button left (i.e. PS3: Square, Xbox: X)
    GAMEPAD_BUTTON_LEFT_TRIGGER_1,      // Gamepad top/back trigger left (first), it could be a trailing button
    GAMEPAD_BUTTON_LEFT_TRIGGER_2,      // Gamepad top/back trigger left (second), it could be a trailing button
    GAMEPAD_BUTTON_RIGHT_TRIGGER_1,     // Gamepad top/back trigger right (first), it could be a trailing button
    GAMEPAD_BUTTON_RIGHT_TRIGGER_2,     // Gamepad top/back trigger right (second), it could be a trailing button
    GAMEPAD_BUTTON_MIDDLE_LEFT,         // Gamepad center buttons, left one (i.e. PS3: Select)
    GAMEPAD_BUTTON_MIDDLE,              // Gamepad center buttons, middle one (i.e. PS3: PS, Xbox: XBOX)
    GAMEPAD_BUTTON_MIDDLE_RIGHT,        // Gamepad center buttons, right one (i.e. PS3: Start)
    GAMEPAD_BUTTON_LEFT_THUMB,          // Gamepad joystick pressed button left
    GAMEPAD_BUTTON_RIGHT_THUMB          // Gamepad joystick pressed button right
} GamepadButton;


extern CoreData CORE;
// Flag to note GPU acceleration is available,
// referenced from other modules to support GPU data loading
// NOTE: Useful to allow Texture, RenderTexture, Font.texture, Mesh.vaoId/vboId, Shader loading
extern bool isGpuReady;

//#if defined(SUPPORT_SCREEN_CAPTURE)
//extern int screenshotCounter = 0;           // Screenshots counter
//#endif
//
//
//#if defined(SUPPORT_GIF_RECORDING)
//extern unsigned int gifFrameCounter = 0;    // GIF frames counter
//extern bool gifRecording = false;           // GIF recording state
//extern MsfGifState gifState = { 0 };        // MSGIF context state
//#endif