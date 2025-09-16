#include "gestures.h"


GesturesData GESTURES = {
   -1,
   GESTURE_NONE,        // No current gesture detected
   0b0000001111111111  // All gestures supported by default
};





//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Enable only desired gestures to be detected
void SetGesturesEnabled(unsigned int flags)
{
    GESTURES.enabledFlags = flags;
}

// Check if a gesture have been detected
bool IsGestureDetected(unsigned int gesture)
{
    if ((GESTURES.enabledFlags & GESTURES.current) == gesture) return true;
    else return false;
}

// Process gesture event and translate it into gestures
void ProcessGestureEvent(GestureEvent event)
{
    // Reset required variables
    GESTURES.Touch.pointCount = event.pointCount;      // Required on UpdateGestures()

    if (GESTURES.Touch.pointCount == 1)     // One touch point
    {
        if (event.touchAction == TOUCH_ACTION_DOWN)
        {
            GESTURES.Touch.tapCounter++;    // Tap counter

            // Detect GESTURE_DOUBLE_TAP
            if ((GESTURES.current == GESTURE_NONE) && (GESTURES.Touch.tapCounter >= 2) && ((rgGetCurrentTime() - GESTURES.Touch.eventTime) < TAP_TIMEOUT) && (rgVector2Distance(GESTURES.Touch.downPositionA, event.position[0]) < DOUBLETAP_RANGE))
            {
                GESTURES.current = GESTURE_DOUBLETAP;
                GESTURES.Touch.tapCounter = 0;
            }
            else    // Detect GESTURE_TAP
            {
                GESTURES.Touch.tapCounter = 1;
                GESTURES.current = GESTURE_TAP;
            }

            GESTURES.Touch.downPositionA = event.position[0];
            GESTURES.Touch.downDragPosition = event.position[0];

            GESTURES.Touch.upPosition = GESTURES.Touch.downPositionA;
            GESTURES.Touch.eventTime = rgGetCurrentTime();

            GESTURES.Swipe.startTime = rgGetCurrentTime();

            GESTURES.Drag.vector ={ 0.0f, 0.0f };
        }
        else if (event.touchAction == TOUCH_ACTION_UP)
        {
            // A swipe can happen while the current gesture is drag, but (specially for web) also hold, so set upPosition for both cases
            if (GESTURES.current == GESTURE_DRAG || GESTURES.current == GESTURE_HOLD) GESTURES.Touch.upPosition = event.position[0];

            // NOTE: GESTURES.Drag.intensity dependent on the resolution of the screen
            GESTURES.Drag.distance = rgVector2Distance(GESTURES.Touch.downPositionA, GESTURES.Touch.upPosition);
            GESTURES.Drag.intensity = GESTURES.Drag.distance / (float)((rgGetCurrentTime() - GESTURES.Swipe.startTime));

            // Detect GESTURE_SWIPE
            if ((GESTURES.Drag.intensity > FORCE_TO_SWIPE) && (GESTURES.current != GESTURE_DRAG))
            {
                // NOTE: Angle should be inverted in Y
                GESTURES.Drag.angle = 360.0f - rgVector2Angle(GESTURES.Touch.downPositionA, GESTURES.Touch.upPosition);

                if ((GESTURES.Drag.angle < 30) || (GESTURES.Drag.angle > 330)) GESTURES.current = GESTURE_SWIPE_RIGHT;          // Right
                else if ((GESTURES.Drag.angle >= 30) && (GESTURES.Drag.angle <= 150)) GESTURES.current = GESTURE_SWIPE_UP;      // Up
                else if ((GESTURES.Drag.angle > 150) && (GESTURES.Drag.angle < 210)) GESTURES.current = GESTURE_SWIPE_LEFT;     // Left
                else if ((GESTURES.Drag.angle >= 210) && (GESTURES.Drag.angle <= 330)) GESTURES.current = GESTURE_SWIPE_DOWN;   // Down
                else GESTURES.current = GESTURE_NONE;
            }
            else
            {
                GESTURES.Drag.distance = 0.0f;
                GESTURES.Drag.intensity = 0.0f;
                GESTURES.Drag.angle = 0.0f;

                GESTURES.current = GESTURE_NONE;
            }

            GESTURES.Touch.downDragPosition = { 0.0f, 0.0f };
            GESTURES.Touch.pointCount = 0;
        }
        else if (event.touchAction == TOUCH_ACTION_MOVE)
        {
            GESTURES.Touch.moveDownPositionA = event.position[0];

            if (GESTURES.current == GESTURE_HOLD)
            {
                if (GESTURES.Hold.resetRequired) GESTURES.Touch.downPositionA = event.position[0];

                GESTURES.Hold.resetRequired = false;

                // Detect GESTURE_DRAG
                if ((rgGetCurrentTime() - GESTURES.Touch.eventTime) > DRAG_TIMEOUT)
                {
                    GESTURES.Touch.eventTime = rgGetCurrentTime();
                    GESTURES.current = GESTURE_DRAG;
                }
            }

            GESTURES.Drag.vector.x = GESTURES.Touch.moveDownPositionA.x - GESTURES.Touch.downDragPosition.x;
            GESTURES.Drag.vector.y = GESTURES.Touch.moveDownPositionA.y - GESTURES.Touch.downDragPosition.y;
        }
    }
    else if (GESTURES.Touch.pointCount == 2)    // Two touch points
    {
        if (event.touchAction == TOUCH_ACTION_DOWN)
        {
            GESTURES.Touch.downPositionA = event.position[0];
            GESTURES.Touch.downPositionB = event.position[1];

            GESTURES.Touch.previousPositionA = GESTURES.Touch.downPositionA;
            GESTURES.Touch.previousPositionB = GESTURES.Touch.downPositionB;

            //GESTURES.Pinch.distance = rgVector2Distance(GESTURES.Touch.downPositionA, GESTURES.Touch.downPositionB);

            GESTURES.Pinch.vector.x = GESTURES.Touch.downPositionB.x - GESTURES.Touch.downPositionA.x;
            GESTURES.Pinch.vector.y = GESTURES.Touch.downPositionB.y - GESTURES.Touch.downPositionA.y;

            GESTURES.current = GESTURE_HOLD;
            GESTURES.Hold.timeDuration = rgGetCurrentTime();
        }
        else if (event.touchAction == TOUCH_ACTION_MOVE)
        {
            GESTURES.Pinch.distance = rgVector2Distance(GESTURES.Touch.moveDownPositionA, GESTURES.Touch.moveDownPositionB);

            GESTURES.Touch.moveDownPositionA = event.position[0];
            GESTURES.Touch.moveDownPositionB = event.position[1];

            GESTURES.Pinch.vector.x = GESTURES.Touch.moveDownPositionB.x - GESTURES.Touch.moveDownPositionA.x;
            GESTURES.Pinch.vector.y = GESTURES.Touch.moveDownPositionB.y - GESTURES.Touch.moveDownPositionA.y;

            if ((rgVector2Distance(GESTURES.Touch.previousPositionA, GESTURES.Touch.moveDownPositionA) >= MINIMUM_PINCH) || (rgVector2Distance(GESTURES.Touch.previousPositionB, GESTURES.Touch.moveDownPositionB) >= MINIMUM_PINCH))
            {
                if (rgVector2Distance(GESTURES.Touch.previousPositionA, GESTURES.Touch.previousPositionB) > rgVector2Distance(GESTURES.Touch.moveDownPositionA, GESTURES.Touch.moveDownPositionB)) GESTURES.current = GESTURE_PINCH_IN;
                else GESTURES.current = GESTURE_PINCH_OUT;
            }
            else
            {
                GESTURES.current = GESTURE_HOLD;
                GESTURES.Hold.timeDuration = rgGetCurrentTime();
            }

            // NOTE: Angle should be inverted in Y
            GESTURES.Pinch.angle = 360.0f - rgVector2Angle(GESTURES.Touch.moveDownPositionA, GESTURES.Touch.moveDownPositionB);
        }
        else if (event.touchAction == TOUCH_ACTION_UP)
        {
            GESTURES.Pinch.distance = 0.0f;
            GESTURES.Pinch.angle = 0.0f;
            GESTURES.Pinch.vector = { 0.0f, 0.0f };
            GESTURES.Touch.pointCount = 0;

            GESTURES.current = GESTURE_NONE;
        }
    }
    else if (GESTURES.Touch.pointCount > 2)     // More than two touch points
    {
        // TODO: Process gesture events for more than two points
    }
}

// Update gestures detected (must be called every frame)
void UpdateGestures(void)
{
    // NOTE: Gestures are processed through system callbacks on touch events

    // Detect GESTURE_HOLD
    if (((GESTURES.current == GESTURE_TAP) || (GESTURES.current == GESTURE_DOUBLETAP)) && (GESTURES.Touch.pointCount < 2))
    {
        GESTURES.current = GESTURE_HOLD;
        GESTURES.Hold.timeDuration = rgGetCurrentTime();
    }

    // Detect GESTURE_NONE
    if ((GESTURES.current == GESTURE_SWIPE_RIGHT) || (GESTURES.current == GESTURE_SWIPE_UP) || (GESTURES.current == GESTURE_SWIPE_LEFT) || (GESTURES.current == GESTURE_SWIPE_DOWN))
    {
        GESTURES.current = GESTURE_NONE;
    }
}

// Get latest detected gesture
int GetGestureDetected(void)
{
    // Get current gesture only if enabled
    return (GESTURES.enabledFlags & GESTURES.current);
}

// Hold time measured in seconds
float GetGestureHoldDuration(void)
{
    // NOTE: time is calculated on current gesture HOLD

    double time = 0.0;

    if (GESTURES.current == GESTURE_HOLD) time = rgGetCurrentTime() - GESTURES.Hold.timeDuration;

    return (float)time;
}

// Get drag vector (between initial touch point to current)
Vector2 GetGestureDragVector(void)
{
    // NOTE: drag vector is calculated on one touch points TOUCH_ACTION_MOVE

    return GESTURES.Drag.vector;
}

// Get drag angle
// NOTE: Angle in degrees, horizontal-right is 0, counterclockwise
float GetGestureDragAngle(void)
{
    // NOTE: drag angle is calculated on one touch points TOUCH_ACTION_UP

    return GESTURES.Drag.angle;
}

// Get distance between two pinch points
Vector2 GetGesturePinchVector(void)
{
    // NOTE: Pinch distance is calculated on two touch points TOUCH_ACTION_MOVE

    return GESTURES.Pinch.vector;
}

// Get angle between two pinch points
// NOTE: Angle in degrees, horizontal-right is 0, counterclockwise
float GetGesturePinchAngle(void)
{
    // NOTE: pinch angle is calculated on two touch points TOUCH_ACTION_MOVE

    return GESTURES.Pinch.angle;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
// Get angle from two-points vector with X-axis
static float rgVector2Angle(Vector2 v1, Vector2 v2)
{
    float angle = atan2f(v2.y - v1.y, v2.x - v1.x) * (180.0f / PI);

    if (angle < 0) angle += 360.0f;

    return angle;
}

// Calculate distance between two Vector2
static float rgVector2Distance(Vector2 v1, Vector2 v2)
{
    float result;

    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;

    result = (float)sqrt(dx * dx + dy * dy);

    return result;
}

// Time measure returned are seconds
static double rgGetCurrentTime(void)
{
    double time = 0;

#if !defined(RGESTURES_STANDALONE)
    time = GetTime();
#else
#if defined(_WIN32)
    unsigned long long int clockFrequency, currentTime;

    QueryPerformanceFrequency(&clockFrequency); // BE CAREFUL: Costly operation!
    QueryPerformanceCounter(&currentTime);

    time = (double)currentTime / clockFrequency;  // Time in seconds
#endif

#if defined(__linux__)
    // NOTE: Only for Linux-based systems
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    unsigned long long int nowTime = (unsigned long long int)now.tv_sec * 1000000000LLU + (unsigned long long int)now.tv_nsec;     // Time in nanoseconds

    time = ((double)nowTime * 1e-9);     // Time in seconds
#endif

#if defined(__APPLE__)
    //#define CLOCK_REALTIME  CALENDAR_CLOCK    // returns UTC time since 1970-01-01
    //#define CLOCK_MONOTONIC SYSTEM_CLOCK      // returns the time since boot time

    clock_serv_t cclock;
    mach_timespec_t now;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);

    // NOTE: OS X does not have clock_gettime(), using clock_get_time()
    clock_get_time(cclock, &now);
    mach_port_deallocate(mach_task_self(), cclock);
    unsigned long long int nowTime = (unsigned long long int)now.tv_sec * 1000000000LLU + (unsigned long long int)now.tv_nsec;     // Time in nanoseconds

    time = ((double)nowTime * 1e-9);     // Time in seconds
#endif
#endif

    return time;
}
