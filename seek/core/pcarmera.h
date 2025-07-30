#pragma once
#include <section/martrix/cg_def.h>
using namespace Seek;


#define PL_CULL_DISTANCE_NEAR              0.001      // Default projection matrix near cull distance
#define PL_CULL_DISTANCE_FAR             10000.0      // Default projection matrix far cull distance


//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define CAMERA_MOVE_SPEED                               5.4f       // Units per second
#define CAMERA_ROTATION_SPEED                           0.03f
#define CAMERA_PAN_SPEED                                0.2f

// Camera mouse movement sensitivity
#define CAMERA_MOUSE_MOVE_SENSITIVITY                   0.003f

// Camera orbital speed in CAMERA_ORBITAL mode
#define CAMERA_ORBITAL_SPEED                            0.5f       // Radians per second


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


// Returns the camera projection matrix
Matrix GetCameraProjectionMatrix(Camera* camera, float aspect);

// Rotates the camera around its right vector, pitch is "looking up and down"
//  - lockView prevents camera overrotation (aka "somersaults")
//  - rotateAroundTarget defines if rotation is around target or around its position
//  - rotateUp rotates the up direction as well (typically only usefull in CAMERA_FREE)
// NOTE: angle must be provided in radians
void CameraPitch(Camera* camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp);

// Rotates the camera around its up vector
// Yaw is "looking left and right"
// If rotateAroundTarget is false, the camera rotates around its position
// Note: angle must be provided in radians
void CameraYaw(Camera* camera, float angle, bool rotateAroundTarget);

// Rotates the camera around its forward vector
// Roll is "turning your head sideways to the left or right"
// Note: angle must be provided in radians
void CameraRoll(Camera* camera, float angle);

// Moves the camera target in its current right direction
void CameraMoveRight(Camera* camera, float distance, bool moveInWorldPlane);
// Moves the camera in its up direction
void CameraMoveUp(Camera* camera, float distance);

// Get axis movement vector for a gamepad
float GetGamepadAxisMovement(int gamepad, int axis);

// Returns the cameras up vector (normalized)
// Note: The up vector might not be perpendicular to the forward vector
Vector3 GetCameraUp(Camera* camera);

Vector3 GetCameraForward(Camera* camera);
// Returns the cameras right vector (normalized)
Vector3 GetCameraRight(Camera* camera);

// Moves the camera in its forward direction
void CameraMoveForward(Camera* camera, float distance, bool moveInWorldPlane);

#if !defined(RCAMERA_STANDALONE)
// Update camera position for selected mode
// Camera mode: CAMERA_FREE, CAMERA_FIRST_PERSON, CAMERA_THIRD_PERSON, CAMERA_ORBITAL or CUSTOM
void UpdateCamera(Camera* camera, int mode);
#endif