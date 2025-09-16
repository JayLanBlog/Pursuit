#pragma once
#include "core.h"
#ifndef PI
#define PI 3.14159265358979323846
#endif


//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#ifndef MAX_TOUCH_POINTS
#define MAX_TOUCH_POINTS        8        // Maximum number of touch points supported
#endif


// Boolean type
#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
#include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool) && !defined(RL_BOOL_TYPE)
typedef enum bool{ false = 0, true = !false } bool;
#endif


//
//#if !defined(PL_VECTOR2_TYPE)
//// Vector2 type
//typedef struct Vector2 {
//    float x;
//    float y;
//} Vector2;
//#endif
// Gesture
// NOTE: Provided as bit-wise flags to enable only desired gestures
typedef enum {
    GESTURE_NONE = 0,        // No gesture
    GESTURE_TAP = 1,        // Tap gesture
    GESTURE_DOUBLETAP = 2,        // Double tap gesture
    GESTURE_HOLD = 4,        // Hold gesture
    GESTURE_DRAG = 8,        // Drag gesture
    GESTURE_SWIPE_RIGHT = 16,       // Swipe right gesture
    GESTURE_SWIPE_LEFT = 32,       // Swipe left gesture
    GESTURE_SWIPE_UP = 64,       // Swipe up gesture
    GESTURE_SWIPE_DOWN = 128,      // Swipe down gesture
    GESTURE_PINCH_IN = 256,      // Pinch in gesture
    GESTURE_PINCH_OUT = 512       // Pinch out gesture
} Gesture;


#if defined(RGESTURES_STANDALONE)
// Gestures type
// NOTE: It could be used as flags to enable only some gestures
typedef enum {
    GESTURE_NONE = 0,
    GESTURE_TAP = 1,
    GESTURE_DOUBLETAP = 2,
    GESTURE_HOLD = 4,
    GESTURE_DRAG = 8,
    GESTURE_SWIPE_RIGHT = 16,
    GESTURE_SWIPE_LEFT = 32,
    GESTURE_SWIPE_UP = 64,
    GESTURE_SWIPE_DOWN = 128,
    GESTURE_PINCH_IN = 256,
    GESTURE_PINCH_OUT = 512
} Gesture;

#endif


typedef enum {
    TOUCH_ACTION_UP = 0,
    TOUCH_ACTION_DOWN,
    TOUCH_ACTION_MOVE,
    TOUCH_ACTION_CANCEL
} TouchAction;

// Gesture event
typedef struct {
    int touchAction;
    int pointCount;
    int pointId[MAX_TOUCH_POINTS];
    Vector2 position[MAX_TOUCH_POINTS];
} GestureEvent;

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif
    void ProcessGestureEvent(GestureEvent event);           // Process gesture event and translate it into gestures
    void UpdateGestures(void);
#if defined(RGESTURES_STANDALONE)
    void SetGesturesEnabled(unsigned int flags);            // Enable a set of gestures using flags
    bool IsGestureDetected(int gesture);                    // Check if a gesture have been detected
    int GetGestureDetected(void);                           // Get latest detected gesture

    float GetGestureHoldDuration(void);                     // Get gesture hold time in seconds
    Vector2 GetGestureDragVector(void);                     // Get gesture drag vector
    float GetGestureDragAngle(void);                        // Get gesture drag angle
    Vector2 GetGesturePinchVector(void);                    // Get gesture pinch delta
    float GetGesturePinchAngle(void);                       // Get gesture pinch angle
#endif

#if defined(__cplusplus)
}
#endif


#if defined(RGESTURES_IMPLEMENTATION)

#if defined(RGESTURES_STANDALONE)
#if defined(_WIN32)
#if defined(__cplusplus)
extern "C" {        // Prevents name mangling of functions
#endif
// Functions required to query time on Windows
    int __stdcall QueryPerformanceCounter(unsigned long long int* lpPerformanceCount);
    int __stdcall QueryPerformanceFrequency(unsigned long long int* lpFrequency);
#if defined(__cplusplus)
}
#endif
#elif defined(__linux__)
#if _POSIX_C_SOURCE < 199309L
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L // Required for CLOCK_MONOTONIC if compiled with c99 without gnu ext.
#endif
#include <sys/time.h>               // Required for: timespec
#include <time.h>                   // Required for: clock_gettime()

#include <math.h>                   // Required for: sqrtf(), atan2f()
#endif
#if defined(__APPLE__)                  // macOS also defines __MACH__
#include <mach/clock.h>             // Required for: clock_get_time()
#include <mach/mach.h>              // Required for: mach_timespec_t
#endif
#endif


#endif



//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define FORCE_TO_SWIPE      0.2f        // Swipe force, measured in normalized screen units/time
#define MINIMUM_DRAG        0.015f      // Drag minimum force, measured in normalized screen units (0.0f to 1.0f)
#define DRAG_TIMEOUT        0.3f        // Drag minimum time for web, measured in seconds
#define MINIMUM_PINCH       0.005f      // Pinch minimum force, measured in normalized screen units (0.0f to 1.0f)
#define TAP_TIMEOUT         0.3f        // Tap minimum time, measured in seconds
#define PINCH_TIMEOUT       0.3f        // Pinch minimum time, measured in seconds
#define DOUBLETAP_RANGE     0.03f       // DoubleTap range, measured in normalized screen units (0.0f to 1.0f)


// Gestures module state context [136 bytes]
typedef struct {
    unsigned int current;               // Current detected gesture
    unsigned int enabledFlags;          // Enabled gestures flags
    struct {
        int firstId;                    // Touch id for first touch point
        int pointCount;                 // Touch points counter
        double eventTime;               // Time stamp when an event happened
        Vector2 upPosition;             // Touch up position
        Vector2 downPositionA;          // First touch down position
        Vector2 downPositionB;          // Second touch down position
        Vector2 downDragPosition;       // Touch drag position
        Vector2 moveDownPositionA;      // First touch down position on move
        Vector2 moveDownPositionB;      // Second touch down position on move
        Vector2 previousPositionA;      // Previous position A to compare for pinch gestures
        Vector2 previousPositionB;      // Previous position B to compare for pinch gestures
        int tapCounter;                 // TAP counter (one tap implies TOUCH_ACTION_DOWN and TOUCH_ACTION_UP actions)
    } Touch;
    struct {
        bool resetRequired;             // HOLD reset to get first touch point again
        double timeDuration;            // HOLD duration in seconds
    } Hold;
    struct {
        Vector2 vector;                 // DRAG vector (between initial and current position)
        float angle;                    // DRAG angle (relative to x-axis)
        float distance;                 // DRAG distance (from initial touch point to final) (normalized [0..1])
        float intensity;                // DRAG intensity, how far why did the DRAG (pixels per frame)
    } Drag;
    struct {
        double startTime;               // SWIPE start time to calculate drag intensity
    } Swipe;
    struct {
        Vector2 vector;                 // PINCH vector (between first and second touch points)
        float angle;                    // PINCH angle (relative to x-axis)
        float distance;                 // PINCH displacement distance (normalized [0..1])
    } Pinch;
} GesturesData;


extern GesturesData GESTURES;


float rgVector2Angle(Vector2 initialPosition, Vector2 finalPosition);

float rgVector2Distance(Vector2 v1, Vector2 v2);

double rgGetCurrentTime(void);

