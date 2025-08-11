#include "wcore_sdl.h"
#ifndef SDL_ENABLE_OLD_NAMES
#define SDL_ENABLE_OLD_NAMES    // Just in case we're on SDL3, we need some in-between compatibily
#endif
#include "SDL3/SDL.h"                // SDL base library (window/rendered, input, timing... functionality)

#include <rmd/gl/gl_include.h>

#if defined(GRAPHICS_API_OPENGL_ES2)
// It seems it does not need to be included to work
#include "SDL_opengles2.h"
#else
#include "SDL3/SDL_opengl.h"     // SDL OpenGL functionality (if required, instead of internal renderer)
#endif
#include <logger/logger.h>
#include <core/core_global.h>
#include <section/enum/config_em.h>
#include <core/core.h>
#include <core/gui/draw_ui.h>


//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#ifndef MAX_CLIPBOARD_BUFFER_LENGTH
#define MAX_CLIPBOARD_BUFFER_LENGTH 1024 // Size of the clipboard buffer used on GetClipboardText()
#endif

#if ((defined(SDL_MAJOR_VERSION) && (SDL_MAJOR_VERSION == 3)) && (defined(SDL_MINOR_VERSION) && (SDL_MINOR_VERSION >= 1)))
#ifndef PLATFORM_DESKTOP_SDL3
#define PLATFORM_DESKTOP_SDL3
#endif
#endif
//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct {
    SDL_Window* window;
    SDL_GLContext glContext;
    SDL_GameController* gamepad[MAX_GAMEPADS];
    SDL_JoystickID gamepadId[MAX_GAMEPADS]; // Joystick instance ids
    SDL_Cursor* cursor;
    bool cursorRelative;
} PlatformData;



static PlatformData platform = { 0 };   // Platform specific data
// Get elapsed time measure in seconds
double GetTime(void)
{
    unsigned int ms = SDL_GetTicks();    // Elapsed time in milliseconds since SDL_Init()
    double time = (double)ms / 1000;
    return time;
}
// Swap back buffer with front buffer (screen drawing)
void SwapScreenBuffer(void)
{
    SDL_GL_SwapWindow(platform.window);
}
// Get physical key name.
const char* GetKeyName(int key)
{
    return SDL_GetKeyName(key);
}

// SDL3 Migration Layer made to avoid `ifdefs` inside functions when we can.
#if defined(PLATFORM_DESKTOP_SDL3)
// SDL3 Migration:
//     SDL_WINDOW_FULLSCREEN_DESKTOP has been removed,
//     and you can call SDL_GetWindowFullscreenMode()
//     to see whether an exclusive fullscreen mode will be used
//     or the borderless fullscreen desktop mode will be used
#define SDL_WINDOW_FULLSCREEN_DESKTOP SDL_WINDOW_FULLSCREEN

#define SDL_IGNORE  false
#define SDL_DISABLE false
#define SDL_ENABLE  true

// SDL3 Migration: SDL_INIT_TIMER - no longer needed before calling SDL_AddTimer()
#define SDL_INIT_TIMER 0x0 // It's a flag, so no problem in setting it to zero if we use in a bitor (|)

// SDL3 Migration: The SDL_WINDOW_SHOWN flag has been removed. Windows are shown by default and can be created hidden by using the SDL_WINDOW_HIDDEN flag.
#define SDL_WINDOW_SHOWN 0x0 // It's a flag, so no problem in setting it to zero if we use in a bitor (|)

// SDL3 Migration: Renamed
// IMPORTANT: Might need to call SDL_CleanupEvent somewhere see :https://github.com/libsdl-org/SDL/issues/3540#issuecomment-1793449852
#define SDL_DROPFILE  SDL_EVENT_DROP_FILE

// Get current monitor where window is placed
int GetCurrentMonitor(void)
{
    int currentMonitor = 0;

    // Be aware that this returns an ID in SDL3 and a Index in SDL2
    currentMonitor = SDL_GetWindowDisplayIndex(platform.window);

    return currentMonitor;
}

int SDL_NumJoysticks(void)
{
    int numJoysticks;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&numJoysticks);
    SDL_free(joysticks);
    return numJoysticks;
}

static void UpdateTouchPointsSDL(SDL_TouchFingerEvent event)
{
#if defined(PLATFORM_DESKTOP_SDL3) // SDL3
    int count = 0;
    SDL_Finger** fingers = SDL_GetTouchFingers(event.touchID, &count);
    CORE.Input.Touch.pointCount = count;

    for (int i = 0; i < CORE.Input.Touch.pointCount; i++)
    {
        SDL_Finger* finger = fingers[i];
        CORE.Input.Touch.pointId[i] = finger->id;
        CORE.Input.Touch.position[i].x = finger->x * CORE.Window.screen.width;
        CORE.Input.Touch.position[i].y = finger->y * CORE.Window.screen.height;
        CORE.Input.Touch.currentTouchState[i] = 1;
    }

    SDL_free(fingers);

#else // SDL2

    CORE.Input.Touch.pointCount = SDL_GetNumTouchFingers(event.touchId);

    for (int i = 0; i < CORE.Input.Touch.pointCount; i++)
    {
        SDL_Finger* finger = SDL_GetTouchFinger(event.touchId, i);
        CORE.Input.Touch.pointId[i] = finger->id;
        CORE.Input.Touch.position[i].x = finger->x * CORE.Window.screen.width;
        CORE.Input.Touch.position[i].y = finger->y * CORE.Window.screen.height;
        CORE.Input.Touch.currentTouchState[i] = 1;
    }
#endif

    for (int i = CORE.Input.Touch.pointCount; i < MAX_TOUCH_POINTS; i++) CORE.Input.Touch.currentTouchState[i] = 0;
}



// SLD3 Migration: To emulate SDL2 this function should return `SDL_DISABLE` or `SDL_ENABLE`
// representing the *processing state* of the event before this function makes any changes to it
Uint8 SDL_EventState(Uint32 type, int state)
{
    Uint8 stateBefore = SDL_EventEnabled(type);

    switch (state)
    {
    case SDL_DISABLE: SDL_SetEventEnabled(type, false); break;
    case SDL_ENABLE: SDL_SetEventEnabled(type, true); break;
    default: TRACELOG(LOG_WARNING, "Event sate: unknow type");
    }

    return stateBefore;
}

// Get window scale DPI factor for current monitor
Vector2 GetWindowScaleDPI(void)
{
    Vector2 scale = { 1.0f, 1.0f };

#ifndef PLATFORM_DESKTOP_SDL3
    // NOTE: SDL_GetWindowDisplayScale was only added on SDL3
    //       see https://wiki.libsdl.org/SDL3/SDL_GetWindowDisplayScale
    // TODO: Implement the window scale factor calculation manually.
    TRACELOG(LOG_WARNING, "GetWindowScaleDPI() not implemented on target platform");
#else
    scale.x = SDL_GetWindowDisplayScale(platform.window);
    scale.y = scale.x;
    TRACELOG(LOG_INFO, "WindowScaleDPI is %f", scale.x);
#endif

    return scale;
}
// SDL2 implementation for SDL3 function
const char* SDL_GameControllerNameForIndex(int joystickIndex)
{
    // NOTE: SDL3 uses the IDs itself (SDL_JoystickID) instead of SDL2 joystick_index
    const char* name = NULL;
    int numJoysticks = 0;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&numJoysticks);

    if (joysticks)
    {
        if (joystickIndex < numJoysticks)
        {
            SDL_JoystickID instance_id = joysticks[joystickIndex];
            name = SDL_GetGamepadNameForID(instance_id);
        }

        SDL_free(joysticks);
    }

    return name;
}

// SDL_SetRelativeMouseMode
// returns 0 on success or a negative error code on failure
// If relative mode is not supported, this returns -1.
int SDL_SetRelativeMouseMode_Adapter(SDL_bool enabled)
{
    // SDL_SetWindowRelativeMouseMode(SDL_Window *window, bool enabled)
    // \returns true on success or false on failure; call SDL_GetError() for more
    if (SDL_SetWindowRelativeMouseMode(platform.window, enabled))
    {
        return 0; // success
    }
    else
    {
        return -1; // failure
    }
}

#define SDL_SetRelativeMouseMode SDL_SetRelativeMouseMode_Adapter

#endif



// Initialize platform: graphics, inputs and more
int InitPlatform(void) {
    // Initialize SDL internal global state, only required systems
    // NOTE: Not all systems need to be initialized, SDL_INIT_AUDIO is not required, managed by miniaudio
    int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
    if (result < 0) { TRACELOG(LOG_WARNING, "SDL: Failed to initialize SDL"); return -1; }

    // Initialize graphic device: display/window and graphic context
    //----------------------------------------------------------------------------
    unsigned int flags = 0;
    flags |= SDL_WINDOW_SHOWN;
    flags |= SDL_WINDOW_OPENGL;
    flags |= SDL_WINDOW_INPUT_FOCUS;
    flags |= SDL_WINDOW_MOUSE_FOCUS;
    flags |= SDL_WINDOW_MOUSE_CAPTURE;  // Window has mouse captured


      // Check window creation flags
    if ((CORE.Window.flags & FLAG_FULLSCREEN_MODE) > 0)
    {
        CORE.Window.fullscreen = true;
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    //if ((CORE.Window.flags & FLAG_WINDOW_HIDDEN) == 0) flags |= SDL_WINDOW_HIDDEN;
    if ((CORE.Window.flags & FLAG_WINDOW_UNDECORATED) > 0) flags |= SDL_WINDOW_BORDERLESS;
    if ((CORE.Window.flags & FLAG_WINDOW_RESIZABLE) > 0) flags |= SDL_WINDOW_RESIZABLE;
    if ((CORE.Window.flags & FLAG_WINDOW_MINIMIZED) > 0) flags |= SDL_WINDOW_MINIMIZED;
    if ((CORE.Window.flags & FLAG_WINDOW_MAXIMIZED) > 0) flags |= SDL_WINDOW_MAXIMIZED;

    if ((CORE.Window.flags & FLAG_WINDOW_UNFOCUSED) > 0)
    {
        flags &= ~SDL_WINDOW_INPUT_FOCUS;
        flags &= ~SDL_WINDOW_MOUSE_FOCUS;
    }

    if ((CORE.Window.flags & FLAG_WINDOW_TOPMOST) > 0) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
    if ((CORE.Window.flags & FLAG_WINDOW_MOUSE_PASSTHROUGH) > 0) flags &= ~SDL_WINDOW_MOUSE_CAPTURE;

    if ((CORE.Window.flags & FLAG_WINDOW_HIGHDPI) > 0) flags |= SDL_WINDOW_ALLOW_HIGHDPI;

    // Check selection OpenGL version
    if (plGetVersion() == PL_OPENGL_21)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    }
    else if (plGetVersion() == PL_OPENGL_33)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    }
    else if (plGetVersion() == PL_OPENGL_43)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#if defined(PLGL_ENABLE_OPENGL_DEBUG_CONTEXT)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);   // Enable OpenGL Debug Context
#endif
    }
    else if (plGetVersion() == PL_OPENGL_ES_20)                 // Request OpenGL ES 2.0 context
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    }
    else if (plGetVersion() == PL_OPENGL_ES_30)                 // Request OpenGL ES 3.0 context
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    }
    if (CORE.Window.flags & FLAG_MSAA_4X_HINT)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    }

    // Init window
#if defined(PLATFORM_DESKTOP_SDL3)
    platform.window = SDL_CreateWindow(CORE.Window.title, CORE.Window.screen.width, CORE.Window.screen.height, flags);
#else
    platform.window = SDL_CreateWindow(CORE.Window.title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CORE.Window.screen.width, CORE.Window.screen.height, flags);
#endif

    // Init OpenGL context
    platform.glContext = SDL_GL_CreateContext(platform.window);
    // Check window and glContext have been initialized successfully
    if ((platform.window != NULL) && (platform.glContext != NULL))
    {
        CORE.Window.ready = true;

 

        const SDL_DisplayMode* displayMode=SDL_GetCurrentDisplayMode(GetCurrentMonitor());

        CORE.Window.display.width = displayMode->w;
        CORE.Window.display.height = displayMode->h;

        CORE.Window.render.width = CORE.Window.screen.width;
        CORE.Window.render.height = CORE.Window.screen.height;
        CORE.Window.currentFbo.width = CORE.Window.render.width;
        CORE.Window.currentFbo.height = CORE.Window.render.height;
        TRACELOG(LOG_INFO, "DISPLAY: Device initialized successfully");
        TRACELOG(LOG_INFO, "    > Display size: %i x %i", CORE.Window.display.width, CORE.Window.display.height);
        TRACELOG(LOG_INFO, "    > Screen size:  %i x %i", CORE.Window.screen.width, CORE.Window.screen.height);
        TRACELOG(LOG_INFO, "    > Render size:  %i x %i", CORE.Window.render.width, CORE.Window.render.height);
        TRACELOG(LOG_INFO, "    > Viewport offsets: %i, %i", CORE.Window.renderOffset.x, CORE.Window.renderOffset.y);
        if (CORE.Window.flags & FLAG_VSYNC_HINT) SDL_GL_SetSwapInterval(1);
        else SDL_GL_SetSwapInterval(0);
    }
    else
    {
        TRACELOG(LOG_FATAL, "PLATFORM: Failed to initialize graphics device");
        return -1;
    }
    // Load OpenGL extensions
  // NOTE: GL procedures address loader is required to load extensions
    LoadExtensions(SDL_GL_GetProcAddress);

    // Initialize input events system
   //----------------------------------------------------------------------------
   // Initialize gamepads
    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        platform.gamepadId[i] = -1; // Set all gamepad initial instance ids as invalid to not conflict with instance id zero
    }

    for (int i = 0; (i < SDL_NumJoysticks()) && (i < MAX_GAMEPADS); i++)
    {
        platform.gamepad[i] = SDL_GameControllerOpen(i);
        platform.gamepadId[i] = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(platform.gamepad[i]));

        if (platform.gamepad[i])
        {
            CORE.Input.Gamepad.ready[i] = true;
            CORE.Input.Gamepad.axisCount[i] = SDL_JoystickNumAxes(SDL_GameControllerGetJoystick(platform.gamepad[i]));
            CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
            CORE.Input.Gamepad.axisState[i][GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;
            strncpy(CORE.Input.Gamepad.name[i], SDL_GameControllerNameForIndex(i), MAX_GAMEPAD_NAME_LENGTH - 1);
            CORE.Input.Gamepad.name[i][MAX_GAMEPAD_NAME_LENGTH - 1] = '\0';
        }
        else TRACELOG(LOG_WARNING, "PLATFORM: Unable to open game controller [ERROR: %s]", SDL_GetError());
    }
    // Disable mouse events being interpreted as touch events
   // NOTE: This is wanted because there are SDL_FINGER* events available which provide unique data
   //       Due to the way PollInputEvents() and rgestures.h are currently implemented, setting this won't break SUPPORT_MOUSE_GESTURES
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    // Initialize timing system
   //----------------------------------------------------------------------------
   // NOTE: No need to call InitTimer(), let SDL manage it internally
    CORE.Time.previous = GetTime();     // Get time as double
#if defined(_WIN32) && defined(SUPPORT_WINMM_HIGHRES_TIMER) && !defined(SUPPORT_BUSY_WAIT_LOOP)
    SDL_SetHint(SDL_HINT_TIMER_RESOLUTION, "1"); // SDL equivalent of timeBeginPeriod() and timeEndPeriod()
#endif
//----------------------------------------------------------------------------

// Initialize storage system
//----------------------------------------------------------------------------
// Define base path for storage
    CORE.Storage.basePath = SDL_GetBasePath(); // Alternative: GetWorkingDirectory();
    //----------------------------------------------------------------------------
#if defined(PLATFORM_DESKTOP_SDL3)
    TRACELOG(LOG_INFO, "PLATFORM: DESKTOP (SDL3): Initialized successfully");
#else
    TRACELOG(LOG_INFO, "PLATFORM: DESKTOP (SDL2): Initialized successfully");
#endif
#ifdef SUPPORT_IMGUI
    InitImGUI(platform.window,platform.glContext);
#endif // SUPPORT_IMGUI

    return 0;
}


// Close platform
void ClosePlatform(void)
{

#ifdef SUPPORT_IMGUI
    DestroyImgui();
#endif
    SDL_FreeCursor(platform.cursor); // Free cursor
    SDL_GL_DeleteContext(platform.glContext); // Deinitialize OpenGL context
    SDL_DestroyWindow(platform.window);
    SDL_Quit(); // Deinitialize SDL internal global state
}

//----------------------------------------------------------------------------------
// Local Variables Definition
//----------------------------------------------------------------------------------
#define SCANCODE_MAPPED_NUM 232
static const PKeyboardKey mapScancodeToKey[SCANCODE_MAPPED_NUM] = {
    KEY_NULL,           // SDL_SCANCODE_UNKNOWN
    KEY_NULL,
    KEY_NULL,
   KEY_NULL,
    KEY_A,              // SDL_SCANCODE_A
    KEY_B,              // SDL_SCANCODE_B
    KEY_C,              // SDL_SCANCODE_C
    KEY_D,              // SDL_SCANCODE_D
    KEY_E,              // SDL_SCANCODE_E
    KEY_F,              // SDL_SCANCODE_F
    KEY_G,              // SDL_SCANCODE_G
    KEY_H,              // SDL_SCANCODE_H
    KEY_I,              // SDL_SCANCODE_I
    KEY_J,              // SDL_SCANCODE_J
    KEY_K,              // SDL_SCANCODE_K
    KEY_L,              // SDL_SCANCODE_L
    KEY_M,              // SDL_SCANCODE_M
    KEY_N,              // SDL_SCANCODE_N
    KEY_O,              // SDL_SCANCODE_O
    KEY_P,              // SDL_SCANCODE_P
    KEY_Q,              // SDL_SCANCODE_Q
    KEY_R,              // SDL_SCANCODE_R
    KEY_S,              // SDL_SCANCODE_S
    KEY_T,              // SDL_SCANCODE_T
    KEY_U,              // SDL_SCANCODE_U
    KEY_V,              // SDL_SCANCODE_V
    KEY_W,              // SDL_SCANCODE_W
    KEY_X,              // SDL_SCANCODE_X
    KEY_Y,              // SDL_SCANCODE_Y
    KEY_Z,              // SDL_SCANCODE_Z
    KEY_ONE,            // SDL_SCANCODE_1
    KEY_TWO,            // SDL_SCANCODE_2
    KEY_THREE,          // SDL_SCANCODE_3
    KEY_FOUR,           // SDL_SCANCODE_4
    KEY_FIVE,           // SDL_SCANCODE_5
    KEY_SIX,            // SDL_SCANCODE_6
    KEY_SEVEN,          // SDL_SCANCODE_7
    KEY_EIGHT,          // SDL_SCANCODE_8
    KEY_NINE,           // SDL_SCANCODE_9
    KEY_ZERO,           // SDL_SCANCODE_0
    KEY_ENTER,          // SDL_SCANCODE_RETURN
    KEY_ESCAPE,         // SDL_SCANCODE_ESCAPE
    KEY_BACKSPACE,      // SDL_SCANCODE_BACKSPACE
    KEY_TAB,            // SDL_SCANCODE_TAB
    KEY_SPACE,          // SDL_SCANCODE_SPACE
    KEY_MINUS,          // SDL_SCANCODE_MINUS
    KEY_EQUAL,          // SDL_SCANCODE_EQUALS
    KEY_LEFT_BRACKET,   // SDL_SCANCODE_LEFTBRACKET
    KEY_RIGHT_BRACKET,  // SDL_SCANCODE_RIGHTBRACKET
    KEY_BACKSLASH,      // SDL_SCANCODE_BACKSLASH
   KEY_NULL,                  // SDL_SCANCODE_NONUSHASH
    KEY_SEMICOLON,      // SDL_SCANCODE_SEMICOLON
    KEY_APOSTROPHE,     // SDL_SCANCODE_APOSTROPHE
    KEY_GRAVE,          // SDL_SCANCODE_GRAVE
    KEY_COMMA,          // SDL_SCANCODE_COMMA
    KEY_PERIOD,         // SDL_SCANCODE_PERIOD
    KEY_SLASH,          // SDL_SCANCODE_SLASH
    KEY_CAPS_LOCK,      // SDL_SCANCODE_CAPSLOCK
    KEY_F1,             // SDL_SCANCODE_F1
    KEY_F2,             // SDL_SCANCODE_F2
    KEY_F3,             // SDL_SCANCODE_F3
    KEY_F4,             // SDL_SCANCODE_F4
    KEY_F5,             // SDL_SCANCODE_F5
    KEY_F6,             // SDL_SCANCODE_F6
    KEY_F7,             // SDL_SCANCODE_F7
    KEY_F8,             // SDL_SCANCODE_F8
    KEY_F9,             // SDL_SCANCODE_F9
    KEY_F10,            // SDL_SCANCODE_F10
    KEY_F11,            // SDL_SCANCODE_F11
    KEY_F12,            // SDL_SCANCODE_F12
    KEY_PRINT_SCREEN,   // SDL_SCANCODE_PRINTSCREEN
    KEY_SCROLL_LOCK,    // SDL_SCANCODE_SCROLLLOCK
    KEY_PAUSE,          // SDL_SCANCODE_PAUSE
    KEY_INSERT,         // SDL_SCANCODE_INSERT
    KEY_HOME,           // SDL_SCANCODE_HOME
    KEY_PAGE_UP,        // SDL_SCANCODE_PAGEUP
    KEY_DELETE,         // SDL_SCANCODE_DELETE
    KEY_END,            // SDL_SCANCODE_END
    KEY_PAGE_DOWN,      // SDL_SCANCODE_PAGEDOWN
    KEY_RIGHT,          // SDL_SCANCODE_RIGHT
    KEY_LEFT,           // SDL_SCANCODE_LEFT
    KEY_DOWN,           // SDL_SCANCODE_DOWN
    KEY_UP,             // SDL_SCANCODE_UP
    KEY_NUM_LOCK,       // SDL_SCANCODE_NUMLOCKCLEAR
    KEY_KP_DIVIDE,      // SDL_SCANCODE_KP_DIVIDE
    KEY_KP_MULTIPLY,    // SDL_SCANCODE_KP_MULTIPLY
    KEY_KP_SUBTRACT,    // SDL_SCANCODE_KP_MINUS
    KEY_KP_ADD,         // SDL_SCANCODE_KP_PLUS
    KEY_KP_ENTER,       // SDL_SCANCODE_KP_ENTER
    KEY_KP_1,           // SDL_SCANCODE_KP_1
    KEY_KP_2,           // SDL_SCANCODE_KP_2
    KEY_KP_3,           // SDL_SCANCODE_KP_3
    KEY_KP_4,           // SDL_SCANCODE_KP_4
    KEY_KP_5,           // SDL_SCANCODE_KP_5
    KEY_KP_6,           // SDL_SCANCODE_KP_6
    KEY_KP_7,           // SDL_SCANCODE_KP_7
    KEY_KP_8,           // SDL_SCANCODE_KP_8
    KEY_KP_9,           // SDL_SCANCODE_KP_9
    KEY_KP_0,           // SDL_SCANCODE_KP_0
    KEY_KP_DECIMAL,     // SDL_SCANCODE_KP_PERIOD
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_LEFT_CONTROL,   //SDL_SCANCODE_LCTRL
    KEY_LEFT_SHIFT,     //SDL_SCANCODE_LSHIFT
    KEY_LEFT_ALT,       //SDL_SCANCODE_LALT
    KEY_LEFT_SUPER,     //SDL_SCANCODE_LGUI
    KEY_RIGHT_CONTROL,  //SDL_SCANCODE_RCTRL
    KEY_RIGHT_SHIFT,    //SDL_SCANCODE_RSHIFT
    KEY_RIGHT_ALT,      //SDL_SCANCODE_RALT
    KEY_RIGHT_SUPER     //SDL_SCANCODE_RGUI
};


// Scancode to keycode mapping
static PKeyboardKey ConvertScancodeToKey(SDL_Scancode sdlScancode)
{
    if ((sdlScancode >= 0) && (sdlScancode < SCANCODE_MAPPED_NUM))
    {
        return mapScancodeToKey[sdlScancode];
    }

    return KEY_NULL; // No equivalent key in Raylib
}




// Register all input events
void PollInputEvents(void) {
#if defined(SUPPORT_GESTURES_SYSTEM)
    // NOTE: Gestures update must be called every frame to reset gestures correctly
    // because ProcessGestureEvent() is just called on an event, not every frame
    UpdateGestures();
#endif
    
    // Reset keys/chars pressed registered
    CORE.Input.Keyboard.keyPressedQueueCount = 0;
    CORE.Input.Keyboard.charPressedQueueCount = 0;

    // Reset mouse wheel
    CORE.Input.Mouse.currentWheelMove.x = 0;
    CORE.Input.Mouse.currentWheelMove.y = 0;

    // Register previous mouse position

    if (platform.cursorRelative) CORE.Input.Mouse.currentPosition ={ 0.0f, 0.0f };
    else CORE.Input.Mouse.previousPosition = CORE.Input.Mouse.currentPosition;
    // Reset last gamepad button/axis registered state
    for (int i = 0; (i < SDL_NumJoysticks()) && (i < MAX_GAMEPADS); i++)
    {
        // Check if gamepad is available
        if (CORE.Input.Gamepad.ready[i])
        {
            // Register previous gamepad button states
            for (int k = 0; k < MAX_GAMEPAD_BUTTONS; k++)
            {
                CORE.Input.Gamepad.previousButtonState[i][k] = CORE.Input.Gamepad.currentButtonState[i][k];
            }
        }
    }

    // Register previous touch states
    for (int i = 0; i < MAX_TOUCH_POINTS; i++) CORE.Input.Touch.previousTouchState[i] = CORE.Input.Touch.currentTouchState[i];
    
    // Map touch position to mouse position for convenience
    if (CORE.Input.Touch.pointCount == 0) CORE.Input.Touch.position[0] = CORE.Input.Mouse.currentPosition;

    int touchAction = -1;       // 0-TOUCH_ACTION_UP, 1-TOUCH_ACTION_DOWN, 2-TOUCH_ACTION_MOVE
    bool realTouch = false;     // Flag to differentiate real touch gestures from mouse ones
    

     // Register previous keys states
    // NOTE: Android supports up to 260 keys
    for (int i = 0; i < MAX_KEYBOARD_KEYS; i++)
    {
        CORE.Input.Keyboard.previousKeyState[i] = CORE.Input.Keyboard.currentKeyState[i];
        CORE.Input.Keyboard.keyRepeatInFrame[i] = 0;
    }
    // Register previous mouse states
    for (int i = 0; i < MAX_MOUSE_BUTTONS; i++) CORE.Input.Mouse.previousButtonState[i] = CORE.Input.Mouse.currentButtonState[i];
    


    CORE.Window.resizedLastFrame = false;

    if ((CORE.Window.eventWaiting) || (((CORE.Window.flags & FLAG_WINDOW_MINIMIZED) > 0) && ((CORE.Window.flags & FLAG_WINDOW_ALWAYS_RUN) == 0)))
    {
        SDL_WaitEvent(NULL);
        CORE.Time.previous = GetTime();
    }

    SDL_Event event = { 0 };
   


    while (SDL_PollEvent(&event) != 0)
    {
#ifdef SUPPORT_IMGUI
        ProccessImGuiEvent(event);
#endif // SUPPORT_IMGUI

       
        // All input events can be processed after polling
        switch (event.type)
        {
        case SDL_QUIT: CORE.Window.shouldClose = true; break;

        case SDL_DROPFILE:      // Dropped file
        {
            if (CORE.Window.dropFileCount == 0)
            {
                // When a new file is dropped, we reserve a fixed number of slots for all possible dropped files
                // at the moment we limit the number of drops at once to 1024 files but this behaviour should probably be reviewed
                // TODO: Pointers should probably be reallocated for any new file added...
                CORE.Window.dropFilepaths = (char**)CALLOC(1024, sizeof(char*));

                CORE.Window.dropFilepaths[CORE.Window.dropFileCount] = (char*)CALLOC(MAX_FILEPATH_LENGTH, sizeof(char));

#if defined(PLATFORM_DESKTOP_SDL3)
                // const char *data;   /**< The text for SDL_EVENT_DROP_TEXT and the file name for SDL_EVENT_DROP_FILE, NULL for other events */
                // Event memory is now managed by SDL, so you should not free the data in SDL_EVENT_DROP_FILE, and if you want to hold onto the text in SDL_EVENT_TEXT_EDITING and SDL_EVENT_TEXT_INPUT events, you should make a copy of it. SDL_TEXTINPUTEVENT_TEXT_SIZE is no longer necessary and has been removed.
                strcpy(CORE.Window.dropFilepaths[CORE.Window.dropFileCount], event.drop.data);
#else
                strcpy(CORE.Window.dropFilepaths[CORE.Window.dropFileCount], event.drop.file);
                SDL_free(event.drop.file);
#endif

                CORE.Window.dropFileCount++;
            }
            else if (CORE.Window.dropFileCount < 1024)
            {
                CORE.Window.dropFilepaths[CORE.Window.dropFileCount] = (char*)CALLOC(MAX_FILEPATH_LENGTH, sizeof(char));

#if defined(PLATFORM_DESKTOP_SDL3)
                strcpy(CORE.Window.dropFilepaths[CORE.Window.dropFileCount], event.drop.data);
#else
                strcpy(CORE.Window.dropFilepaths[CORE.Window.dropFileCount], event.drop.file);
                SDL_free(event.drop.file);
#endif

                CORE.Window.dropFileCount++;
            }
            else TRACELOG(LOG_WARNING, "FILE: Maximum drag and drop files at once is limited to 1024 files!");

        } break;

        // Window events are also polled (Minimized, maximized, close...)

#ifndef PLATFORM_DESKTOP_SDL3
// SDL3 states:
//     The SDL_WINDOWEVENT_* events have been moved to top level events,
//     and SDL_WINDOWEVENT has been removed.
//     In general, handling this change just means checking for the individual events instead of first checking for SDL_WINDOWEVENT
//     and then checking for window events. You can compare the event >= SDL_EVENT_WINDOW_FIRST and <= SDL_EVENT_WINDOW_LAST if you need to see whether it's a window event.
        case SDL_WINDOWEVENT:
        {
            switch (event.window.event)
            {
#endif
            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
            {
                const int width = event.window.data1;
                const int height = event.window.data2;
                SetupViewport(width, height);
                // if we are doing automatic DPI scaling, then the "screen" size is divided by the window scale
                if (IsWindowState(FLAG_WINDOW_HIGHDPI))
                {
                    CORE.Window.screen.width = (int)(width / GetWindowScaleDPI().x);
                    CORE.Window.screen.height = (int)(height / GetWindowScaleDPI().y);
                }
                else
                {
                    CORE.Window.screen.width = width;
                    CORE.Window.screen.height = height;
                }
                CORE.Window.currentFbo.width = width;
                CORE.Window.currentFbo.height = height;
                CORE.Window.resizedLastFrame = true;

#ifndef PLATFORM_DESKTOP_SDL3
                // Manually detect if the window was maximized (due to SDL2 restore being unreliable on some platforms) to remove the FLAG_WINDOW_MAXIMIZED accordingly
                if ((CORE.Window.flags & FLAG_WINDOW_MAXIMIZED) > 0)
                {
                    int borderTop = 0;
                    int borderLeft = 0;
                    int borderBottom = 0;
                    int borderRight = 0;
                    SDL_GetWindowBordersSize(platform.window, &borderTop, &borderLeft, &borderBottom, &borderRight);
                    SDL_Rect usableBounds;
                    SDL_GetDisplayUsableBounds(SDL_GetWindowDisplayIndex(platform.window), &usableBounds);

                    if ((width + borderLeft + borderRight != usableBounds.w) && (height + borderTop + borderBottom != usableBounds.h)) CORE.Window.flags &= ~FLAG_WINDOW_MAXIMIZED;
                }
#endif
            } break;

            case SDL_WINDOWEVENT_ENTER:
            {
                CORE.Input.Mouse.cursorOnScreen = true;
            } break;
            case SDL_WINDOWEVENT_LEAVE:
            {
                CORE.Input.Mouse.cursorOnScreen = false;
            } break;

            case SDL_WINDOWEVENT_MINIMIZED:
            {
                if ((CORE.Window.flags & FLAG_WINDOW_MINIMIZED) == 0) CORE.Window.flags |= FLAG_WINDOW_MINIMIZED;
            } break;
            case SDL_WINDOWEVENT_MAXIMIZED:
            {
                if ((CORE.Window.flags & FLAG_WINDOW_MAXIMIZED) == 0) CORE.Window.flags |= FLAG_WINDOW_MAXIMIZED;
            } break;
            case SDL_WINDOWEVENT_RESTORED:
            {
                if ((SDL_GetWindowFlags(platform.window) & SDL_WINDOW_MINIMIZED) == 0)
                {
                    if ((CORE.Window.flags & FLAG_WINDOW_MINIMIZED) > 0) CORE.Window.flags &= ~FLAG_WINDOW_MINIMIZED;
                }

#ifdef PLATFORM_DESKTOP_SDL3
                if ((SDL_GetWindowFlags(platform.window) & SDL_WINDOW_MAXIMIZED) == 0)
                {
                    if ((CORE.Window.flags & SDL_WINDOW_MAXIMIZED) > 0) CORE.Window.flags &= ~SDL_WINDOW_MAXIMIZED;
                }
#endif
            } break;

            case SDL_WINDOWEVENT_HIDDEN:
            {
                if ((CORE.Window.flags & FLAG_WINDOW_HIDDEN) == 0) CORE.Window.flags |= FLAG_WINDOW_HIDDEN;
            } break;
            case SDL_WINDOWEVENT_SHOWN:
            {
                if ((CORE.Window.flags & FLAG_WINDOW_HIDDEN) > 0) CORE.Window.flags &= ~FLAG_WINDOW_HIDDEN;
            } break;

            case SDL_WINDOWEVENT_FOCUS_GAINED:
            {
                if ((CORE.Window.flags & FLAG_WINDOW_UNFOCUSED) > 0) CORE.Window.flags &= ~FLAG_WINDOW_UNFOCUSED;
            } break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
            {
                if ((CORE.Window.flags & FLAG_WINDOW_UNFOCUSED) == 0) CORE.Window.flags |= FLAG_WINDOW_UNFOCUSED;
            } break;

#ifndef PLATFORM_DESKTOP_SDL3
            default: break;
            }
        } break;
#endif

        // Keyboard events
        case SDL_KEYDOWN:
        {
#if defined(PLATFORM_DESKTOP_SDL3)
            // SDL3 Migration: The following structures have been removed: * SDL_Keysym
            PKeyboardKey key = ConvertScancodeToKey(event.key.scancode);
#else
            KeyboardKey key = ConvertScancodeToKey(event.key.keysym.scancode);
#endif

            if (key != KEY_NULL)
            {
                // If key was up, add it to the key pressed queue
                if ((CORE.Input.Keyboard.currentKeyState[key] == 0) && (CORE.Input.Keyboard.keyPressedQueueCount < MAX_KEY_PRESSED_QUEUE))
                {
                    CORE.Input.Keyboard.keyPressedQueue[CORE.Input.Keyboard.keyPressedQueueCount] = key;
                    CORE.Input.Keyboard.keyPressedQueueCount++;
                }

                CORE.Input.Keyboard.currentKeyState[key] = 1;
            }

            if (event.key.repeat) CORE.Input.Keyboard.keyRepeatInFrame[key] = 1;

            // TODO: Put exitKey verification outside the switch?
            if (CORE.Input.Keyboard.currentKeyState[CORE.Input.Keyboard.exitKey])
            {
                CORE.Window.shouldClose = true;
            }
        } break;

        case SDL_KEYUP:
        {

#if defined(PLATFORM_DESKTOP_SDL3)
            PKeyboardKey key = ConvertScancodeToKey(event.key.scancode);
#else
            KeyboardKey key = ConvertScancodeToKey(event.key.keysym.scancode);
#endif
            if (key != KEY_NULL) CORE.Input.Keyboard.currentKeyState[key] = 0;
        } break;

        case SDL_TEXTINPUT:
        {
            // NOTE: event.text.text data comes an UTF-8 text sequence but we register codepoints (int)

            // Check if there is space available in the queue
            if (CORE.Input.Keyboard.charPressedQueueCount < MAX_CHAR_PRESSED_QUEUE)
            {
                // Add character (codepoint) to the queue
#if defined(PLATFORM_DESKTOP_SDL3)
                size_t textLen = strlen(event.text.text);
                unsigned int codepoint = (unsigned int)SDL_StepUTF8(&event.text.text,&textLen);
#else
                int codepointSize = 0;
                int codepoint = GetCodepointNextSDL(event.text.text, &codepointSize);
#endif
                CORE.Input.Keyboard.charPressedQueue[CORE.Input.Keyboard.charPressedQueueCount] = codepoint;
                CORE.Input.Keyboard.charPressedQueueCount++;
            }
        } break;

        // Check mouse events
        case SDL_MOUSEBUTTONDOWN:
        {
            // NOTE: SDL2 mouse button order is LEFT, MIDDLE, RIGHT, but raylib uses LEFT, RIGHT, MIDDLE like GLFW
            //       The following conditions align SDL with raylib.h MouseButton enum order
            int btn = event.button.button - 1;
            if (btn == 2) btn = 1;
            else if (btn == 1) btn = 2;

            CORE.Input.Mouse.currentButtonState[btn] = 1;
            CORE.Input.Touch.currentTouchState[btn] = 1;

            touchAction = 1;
        } break;
        case SDL_MOUSEBUTTONUP:
        {
            // NOTE: SDL2 mouse button order is LEFT, MIDDLE, RIGHT, but raylib uses LEFT, RIGHT, MIDDLE like GLFW
            //       The following conditions align SDL with raylib.h MouseButton enum order
            int btn = event.button.button - 1;
            if (btn == 2) btn = 1;
            else if (btn == 1) btn = 2;

            CORE.Input.Mouse.currentButtonState[btn] = 0;
            CORE.Input.Touch.currentTouchState[btn] = 0;

            touchAction = 0;
        } break;
        case SDL_MOUSEWHEEL:
        {
            CORE.Input.Mouse.currentWheelMove.x = (float)event.wheel.x;
            CORE.Input.Mouse.currentWheelMove.y = (float)event.wheel.y;
        } break;
        case SDL_MOUSEMOTION:
        {
            if (platform.cursorRelative)
            {
                CORE.Input.Mouse.currentPosition.x = (float)event.motion.xrel;
                CORE.Input.Mouse.currentPosition.y = (float)event.motion.yrel;
                CORE.Input.Mouse.previousPosition = { 0.0f, 0.0f };
            }
            else
            {
                CORE.Input.Mouse.currentPosition.x = (float)event.motion.x;
                CORE.Input.Mouse.currentPosition.y = (float)event.motion.y;
            }

            CORE.Input.Touch.position[0] = CORE.Input.Mouse.currentPosition;
            touchAction = 2;
        } break;

        case SDL_FINGERDOWN:
        {
            UpdateTouchPointsSDL(event.tfinger);
            touchAction = 1;
            realTouch = true;
        } break;
        case SDL_FINGERUP:
        {
            UpdateTouchPointsSDL(event.tfinger);
            touchAction = 0;
            realTouch = true;
        } break;
        case SDL_FINGERMOTION:
        {
            UpdateTouchPointsSDL(event.tfinger);
            touchAction = 2;
            realTouch = true;
        } break;

        // Check gamepad events
        case SDL_JOYDEVICEADDED:
        {
            int jid = event.jdevice.which; // Joystick device index

            if (CORE.Input.Gamepad.ready[jid] && (jid < MAX_GAMEPADS))
            {
                platform.gamepad[jid] = SDL_GameControllerOpen(jid);
                platform.gamepadId[jid] = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(platform.gamepad[jid]));

                if (platform.gamepad[jid])
                {
                    CORE.Input.Gamepad.ready[jid] = true;
                    CORE.Input.Gamepad.axisCount[jid] = SDL_JoystickNumAxes(SDL_GameControllerGetJoystick(platform.gamepad[jid]));
                    CORE.Input.Gamepad.axisState[jid][GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
                    CORE.Input.Gamepad.axisState[jid][GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;
                    memset(CORE.Input.Gamepad.name[jid], 0, MAX_GAMEPAD_NAME_LENGTH);
                    strncpy(CORE.Input.Gamepad.name[jid], SDL_GameControllerNameForIndex(jid), MAX_GAMEPAD_NAME_LENGTH - 1);
                }
                else
                {
                    TRACELOG(LOG_WARNING, "PLATFORM: Unable to open game controller [ERROR: %s]", SDL_GetError());
                }
            }
        } break;
        case SDL_JOYDEVICEREMOVED:
        {
            int jid = event.jdevice.which; // Joystick instance id

            for (int i = 0; i < MAX_GAMEPADS; i++)
            {
                if (platform.gamepadId[i] == jid)
                {
                    SDL_GameControllerClose(platform.gamepad[i]);
                    CORE.Input.Gamepad.ready[i] = false;
                    memset(CORE.Input.Gamepad.name[i], 0, MAX_GAMEPAD_NAME_LENGTH);
                    platform.gamepadId[i] = -1;
                    break;
                }
            }
        } break;
        case SDL_CONTROLLERBUTTONDOWN:
        {
            int button = -1;

            switch (event.jbutton.button)
            {
            case SDL_CONTROLLER_BUTTON_Y: button = GAMEPAD_BUTTON_RIGHT_FACE_UP; break;
            case SDL_CONTROLLER_BUTTON_B: button = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT; break;
            case SDL_CONTROLLER_BUTTON_A: button = GAMEPAD_BUTTON_RIGHT_FACE_DOWN; break;
            case SDL_CONTROLLER_BUTTON_X: button = GAMEPAD_BUTTON_RIGHT_FACE_LEFT; break;

            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: button = GAMEPAD_BUTTON_LEFT_TRIGGER_1; break;
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: button = GAMEPAD_BUTTON_RIGHT_TRIGGER_1; break;

            case SDL_CONTROLLER_BUTTON_BACK: button = GAMEPAD_BUTTON_MIDDLE_LEFT; break;
            case SDL_CONTROLLER_BUTTON_GUIDE: button = GAMEPAD_BUTTON_MIDDLE; break;
            case SDL_CONTROLLER_BUTTON_START: button = GAMEPAD_BUTTON_MIDDLE_RIGHT; break;

            case SDL_CONTROLLER_BUTTON_DPAD_UP: button = GAMEPAD_BUTTON_LEFT_FACE_UP; break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: button = GAMEPAD_BUTTON_LEFT_FACE_RIGHT; break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN: button = GAMEPAD_BUTTON_LEFT_FACE_DOWN; break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT: button = GAMEPAD_BUTTON_LEFT_FACE_LEFT; break;

            case SDL_CONTROLLER_BUTTON_LEFTSTICK: button = GAMEPAD_BUTTON_LEFT_THUMB; break;
            case SDL_CONTROLLER_BUTTON_RIGHTSTICK: button = GAMEPAD_BUTTON_RIGHT_THUMB; break;
            default: break;
            }

            if (button >= 0)
            {
                for (int i = 0; i < MAX_GAMEPADS; i++)
                {
                    if (platform.gamepadId[i] == event.jbutton.which)
                    {
                        CORE.Input.Gamepad.currentButtonState[i][button] = 1;
                        CORE.Input.Gamepad.lastButtonPressed = button;
                        break;
                    }
                }
            }
        } break;
        case SDL_CONTROLLERBUTTONUP:
        {
            int button = -1;

            switch (event.jbutton.button)
            {
            case SDL_CONTROLLER_BUTTON_Y: button = GAMEPAD_BUTTON_RIGHT_FACE_UP; break;
            case SDL_CONTROLLER_BUTTON_B: button = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT; break;
            case SDL_CONTROLLER_BUTTON_A: button = GAMEPAD_BUTTON_RIGHT_FACE_DOWN; break;
            case SDL_CONTROLLER_BUTTON_X: button = GAMEPAD_BUTTON_RIGHT_FACE_LEFT; break;

            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: button = GAMEPAD_BUTTON_LEFT_TRIGGER_1; break;
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: button = GAMEPAD_BUTTON_RIGHT_TRIGGER_1; break;

            case SDL_CONTROLLER_BUTTON_BACK: button = GAMEPAD_BUTTON_MIDDLE_LEFT; break;
            case SDL_CONTROLLER_BUTTON_GUIDE: button = GAMEPAD_BUTTON_MIDDLE; break;
            case SDL_CONTROLLER_BUTTON_START: button = GAMEPAD_BUTTON_MIDDLE_RIGHT; break;

            case SDL_CONTROLLER_BUTTON_DPAD_UP: button = GAMEPAD_BUTTON_LEFT_FACE_UP; break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: button = GAMEPAD_BUTTON_LEFT_FACE_RIGHT; break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN: button = GAMEPAD_BUTTON_LEFT_FACE_DOWN; break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT: button = GAMEPAD_BUTTON_LEFT_FACE_LEFT; break;

            case SDL_CONTROLLER_BUTTON_LEFTSTICK: button = GAMEPAD_BUTTON_LEFT_THUMB; break;
            case SDL_CONTROLLER_BUTTON_RIGHTSTICK: button = GAMEPAD_BUTTON_RIGHT_THUMB; break;
            default: break;
            }

            if (button >= 0)
            {
                for (int i = 0; i < MAX_GAMEPADS; i++)
                {
                    if (platform.gamepadId[i] == event.jbutton.which)
                    {
                        CORE.Input.Gamepad.currentButtonState[i][button] = 0;
                        if (CORE.Input.Gamepad.lastButtonPressed == button) CORE.Input.Gamepad.lastButtonPressed = 0;
                        break;
                    }
                }
            }
        } break;
        case SDL_CONTROLLERAXISMOTION:
        {
            int axis = -1;

            switch (event.jaxis.axis)
            {
            case SDL_CONTROLLER_AXIS_LEFTX: axis = GAMEPAD_AXIS_LEFT_X; break;
            case SDL_CONTROLLER_AXIS_LEFTY: axis = GAMEPAD_AXIS_LEFT_Y; break;
            case SDL_CONTROLLER_AXIS_RIGHTX: axis = GAMEPAD_AXIS_RIGHT_X; break;
            case SDL_CONTROLLER_AXIS_RIGHTY: axis = GAMEPAD_AXIS_RIGHT_Y; break;
            case SDL_CONTROLLER_AXIS_TRIGGERLEFT: axis = GAMEPAD_AXIS_LEFT_TRIGGER; break;
            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: axis = GAMEPAD_AXIS_RIGHT_TRIGGER; break;
            default: break;
            }

            if (axis >= 0)
            {
                for (int i = 0; i < MAX_GAMEPADS; i++)
                {
                    if (platform.gamepadId[i] == event.jaxis.which)
                    {
                        // SDL axis value range is -32768 to 32767, we normalize it to raylib's -1.0 to 1.0f range
                        float value = event.jaxis.value / (float)32767;
                        CORE.Input.Gamepad.axisState[i][axis] = value;

                        // Register button state for triggers in addition to their axes
                        if ((axis == GAMEPAD_AXIS_LEFT_TRIGGER) || (axis == GAMEPAD_AXIS_RIGHT_TRIGGER))
                        {
                            int button = (axis == GAMEPAD_AXIS_LEFT_TRIGGER) ? GAMEPAD_BUTTON_LEFT_TRIGGER_2 : GAMEPAD_BUTTON_RIGHT_TRIGGER_2;
                            int pressed = (value > 0.1f);
                            CORE.Input.Gamepad.currentButtonState[i][button] = pressed;
                            if (pressed) CORE.Input.Gamepad.lastButtonPressed = button;
                            else if (CORE.Input.Gamepad.lastButtonPressed == button) CORE.Input.Gamepad.lastButtonPressed = 0;
                        }
                        break;
                    }
                }
            }
        } break;
        default: break;
        }

#if defined(SUPPORT_GESTURES_SYSTEM)
        if (touchAction > -1)
        {
            // Process mouse events as touches to be able to use mouse-gestures
            GestureEvent gestureEvent = { 0 };

            // Register touch actions
            gestureEvent.touchAction = touchAction;

            // Assign a pointer ID
            gestureEvent.pointId[0] = 0;

            // Register touch points count
            gestureEvent.pointCount = 1;

            // Register touch points position, only one point registered
            if (touchAction == 2 || realTouch) gestureEvent.position[0] = CORE.Input.Touch.position[0];
            else gestureEvent.position[0] = GetMousePosition();

            // Normalize gestureEvent.position[0] for CORE.Window.screen.width and CORE.Window.screen.height
            gestureEvent.position[0].x /= (float)GetScreenWidth();
            gestureEvent.position[0].y /= (float)GetScreenHeight();

            // Gesture data is sent to gestures-system for processing
            ProcessGestureEvent(gestureEvent);

            touchAction = -1;
        }
#endif
    }

}

// Enables cursor (unlock cursor)
void EnableCursor(void)
{
    SDL_SetRelativeMouseMode(SDL_FALSE);

#if defined(PLATFORM_DESKTOP_SDL3)
    // SDL_ShowCursor() has been split into three functions: SDL_ShowCursor(), SDL_HideCursor(), and SDL_CursorVisible()
    SDL_ShowCursor();
#else
    SDL_ShowCursor(SDL_ENABLE);
#endif

    platform.cursorRelative = false;
    CORE.Input.Mouse.cursorHidden = false;
}


void DisableCursor(void) {
    SDL_SetRelativeMouseMode(SDL_TRUE);

    platform.cursorRelative = true;
    CORE.Input.Mouse.cursorHidden = true;
}