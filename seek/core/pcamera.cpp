#include "pcarmera.h"
#include <core/core.h>
#include <math/pr_math.h>
#include <section/enum/camera_em.h>
#include <section/enum/key_em.h>
#include <section/enum/winplat_em.h>
#include <core/core.h>
#include <core/core_global.h>
#if defined(RCAMERA_STANDALONE)
#define CAMERA_CULL_DISTANCE_NEAR      0.01
#define CAMERA_CULL_DISTANCE_FAR    1000.0
#else
#define CAMERA_CULL_DISTANCE_NEAR   PL_CULL_DISTANCE_NEAR
#define CAMERA_CULL_DISTANCE_FAR    PL_CULL_DISTANCE_FAR
#endif
using namespace PMath;

// Returns the camera projection matrix
Matrix GetCameraProjectionMatrix(Camera* camera, float aspect) {
    if (camera->projection == CAMERA_PERSPECTIVE)
    {
        return MatrixPerspective(camera->fovy * DEG2RAD, aspect, CAMERA_CULL_DISTANCE_NEAR, CAMERA_CULL_DISTANCE_FAR);
    }
    else if (camera->projection == CAMERA_ORTHOGRAPHIC)
    {
        double top = camera->fovy / 2.0;
        double right = top * aspect;

        return MatrixOrtho(-right, right, -top, top, CAMERA_CULL_DISTANCE_NEAR, CAMERA_CULL_DISTANCE_FAR);
    }

    return MatrixIdentity();
}


Vector3 GetCameraForward(Camera* camera)
{
    return Vector3Normalize(Vector3Subtract(camera->target, camera->position));
}

// Returns the cameras right vector (normalized)
Vector3 GetCameraRight(Camera* camera)
{
    Vector3 forward = GetCameraForward(camera);
    Vector3 up = GetCameraUp(camera);

    return Vector3Normalize(Vector3CrossProduct(forward, up));
}

// Moves the camera in its forward direction
void CameraMoveForward(Camera* camera, float distance, bool moveInWorldPlane)
{
    Vector3 forward = GetCameraForward(camera);

    if (moveInWorldPlane)
    {
        // Project vector onto world plane
        forward.y = 0;
        forward = Vector3Normalize(forward);
    }

    // Scale by distance
    forward = Vector3Scale(forward, distance);

    // Move position and target
    camera->position = Vector3Add(camera->position, forward);
    camera->target = Vector3Add(camera->target, forward);
}



Vector3 GetCameraUp(Camera* camera) {
    return Vector3Normalize(camera->up);
}

// Rotates the camera around its right vector, pitch is "looking up and down"
//  - lockView prevents camera overrotation (aka "somersaults")
//  - rotateAroundTarget defines if rotation is around target or around its position
//  - rotateUp rotates the up direction as well (typically only usefull in CAMERA_FREE)
// NOTE: angle must be provided in radians
void CameraPitch(Camera* camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp) {
    // Up direction
    Vector3 up = GetCameraUp(camera);
    // View vector
    Vector3 targetPosition = Vector3Subtract(camera->target, camera->position);

    if (lockView)
    {
        // In these camera modes we clamp the Pitch angle
        // to allow only viewing straight up or down.

        // Clamp view up
        float maxAngleUp = Vector3Angle(up, targetPosition);
        maxAngleUp -= 0.001f; // avoid numerical errors
        if (angle > maxAngleUp) angle = maxAngleUp;

        // Clamp view down
        float maxAngleDown = Vector3Angle(Vector3Negate(up), targetPosition);
        maxAngleDown *= -1.0f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors
        if (angle < maxAngleDown) angle = maxAngleDown;
    }

    // Rotation axis
    Vector3 right = GetCameraRight(camera);

    // Rotate view vector around right axis
    targetPosition = Vector3RotateByAxisAngle(targetPosition, right, angle);


    if (rotateAroundTarget)
    {
        // Move position relative to target
        camera->position = Vector3Subtract(camera->target, targetPosition);
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        camera->target = Vector3Add(camera->position, targetPosition);
    }

    if (rotateUp)
    {
        // Rotate up direction around right axis
        camera->up = Vector3RotateByAxisAngle(camera->up, right, angle);
    }
}

// Rotates the camera around its up vector
// Yaw is "looking left and right"
// If rotateAroundTarget is false, the camera rotates around its position
// Note: angle must be provided in radians
void CameraYaw(Camera* camera, float angle, bool rotateAroundTarget)
{
    // Rotation axis
    Vector3 up = GetCameraUp(camera);

    // View vector
    Vector3 targetPosition = Vector3Subtract(camera->target, camera->position);

    // Rotate view vector around up axis
    targetPosition = Vector3RotateByAxisAngle(targetPosition, up, angle);

    if (rotateAroundTarget)
    {
        // Move position relative to target
        camera->position = Vector3Subtract(camera->target, targetPosition);
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        camera->target = Vector3Add(camera->position, targetPosition);
    }
}

// Rotates the camera around its forward vector
// Roll is "turning your head sideways to the left or right"
// Note: angle must be provided in radians
void CameraRoll(Camera* camera, float angle)
{
    // Rotation axis
    Vector3 forward = GetCameraForward(camera);

    // Rotate up direction around forward axis
    camera->up = Vector3RotateByAxisAngle(camera->up, forward, angle);
}
// Moves the camera target in its current right direction
void CameraMoveRight(Camera* camera, float distance, bool moveInWorldPlane)
{
    Vector3 right = GetCameraRight(camera);

    if (moveInWorldPlane)
    {
        // Project vector onto world plane
        right.y = 0;
        right = Vector3Normalize(right);
    }

    // Scale by distance
    right = Vector3Scale(right, distance);

    // Move position and target
    camera->position = Vector3Add(camera->position, right);
    camera->target = Vector3Add(camera->target, right);
}

// Moves the camera in its up direction
void CameraMoveUp(Camera* camera, float distance)
{
    Vector3 up = GetCameraUp(camera);

    // Scale by distance
    up = Vector3Scale(up, distance);

    // Move position and target
    camera->position = Vector3Add(camera->position, up);
    camera->target = Vector3Add(camera->target, up);
}
// Get axis movement vector for a gamepad
float GetGamepadAxisMovement(int gamepad, int axis)
{
    float value = ((axis == GAMEPAD_AXIS_LEFT_TRIGGER) || (axis == GAMEPAD_AXIS_RIGHT_TRIGGER)) ? -1.0f : 0.0f;

    if ((gamepad < MAX_GAMEPADS) && CORE.Input.Gamepad.ready[gamepad] && (axis < MAX_GAMEPAD_AXES))
    {
        float movement = (value < 0.0f) ? CORE.Input.Gamepad.axisState[gamepad][axis] : fabsf(CORE.Input.Gamepad.axisState[gamepad][axis]);

        if (movement > value) value = CORE.Input.Gamepad.axisState[gamepad][axis];
    }

    return value;
}

#if !defined(RCAMERA_STANDALONE)
// Update camera position for selected mode
// Camera mode: CAMERA_FREE, CAMERA_FIRST_PERSON, CAMERA_THIRD_PERSON, CAMERA_ORBITAL or CUSTOM
void UpdateCamera(Camera* camera, int mode) {
	Vector2 mousePositionDelta = GetMouseDelta();
    bool moveInWorldPlane = ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON));
    bool rotateAroundTarget = ((mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL));
    bool lockView = ((mode == CAMERA_FREE) || (mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL));
    bool rotateUp = false;

    // Camera speeds based on frame time
    float cameraMoveSpeed = CAMERA_MOVE_SPEED * GetFrameTime();
    float cameraRotationSpeed = CAMERA_ROTATION_SPEED * GetFrameTime();
    float cameraPanSpeed = CAMERA_PAN_SPEED * GetFrameTime();
    float cameraOrbitalSpeed = CAMERA_ORBITAL_SPEED * GetFrameTime();

    if (mode == CAMERA_CUSTOM) {}
    else if (mode == CAMERA_ORBITAL)
    {
        // Orbital can just orbit
        Matrix rotation = MatrixRotate(GetCameraUp(camera), cameraOrbitalSpeed);
        Vector3 view = Vector3Subtract(camera->position, camera->target);
        view = Vector3Transform(view, rotation);
        camera->position = Vector3Add(camera->target, view);
    }
    else
    {
        // Camera rotation
        if (IsKeyDown(KEY_DOWN)) CameraPitch(camera, -cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
        if (IsKeyDown(KEY_UP)) CameraPitch(camera, cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
        if (IsKeyDown(KEY_RIGHT)) CameraYaw(camera, -cameraRotationSpeed, rotateAroundTarget);
        if (IsKeyDown(KEY_LEFT)) CameraYaw(camera, cameraRotationSpeed, rotateAroundTarget);
        if (IsKeyDown(KEY_Q)) CameraRoll(camera, -cameraRotationSpeed);
        if (IsKeyDown(KEY_E)) CameraRoll(camera, cameraRotationSpeed);

    }
    // Camera movement
    // Camera pan (for CAMERA_FREE)
    if ((mode == CAMERA_FREE) && (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)))
    {
        const Vector2 mouseDelta = GetMouseDelta();
        if (mouseDelta.x > 0.0f) CameraMoveRight(camera, cameraPanSpeed, moveInWorldPlane);
        if (mouseDelta.x < 0.0f) CameraMoveRight(camera, -cameraPanSpeed, moveInWorldPlane);
        if (mouseDelta.y > 0.0f) CameraMoveUp(camera, -cameraPanSpeed);
        if (mouseDelta.y < 0.0f) CameraMoveUp(camera, cameraPanSpeed);
    }
    else
    {
        // Mouse support
        CameraYaw(camera, -mousePositionDelta.x * CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
        CameraPitch(camera, -mousePositionDelta.y * CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);
    }

    // Keyboard support
    if (IsKeyDown(KEY_W)) CameraMoveForward(camera, cameraMoveSpeed, moveInWorldPlane);
    if (IsKeyDown(KEY_A)) CameraMoveRight(camera, -cameraMoveSpeed, moveInWorldPlane);
    if (IsKeyDown(KEY_S)) CameraMoveForward(camera, -cameraMoveSpeed, moveInWorldPlane);
    if (IsKeyDown(KEY_D)) CameraMoveRight(camera, cameraMoveSpeed, moveInWorldPlane);

    // Gamepad movement
    if (IsGamepadAvailable(0))
    {
        // Gamepad controller support
        CameraYaw(camera, -(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 2) * CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
        CameraPitch(camera, -(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * 2) * CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);

        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) <= -0.25f) CameraMoveForward(camera, cameraMoveSpeed, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) <= -0.25f) CameraMoveRight(camera, -cameraMoveSpeed, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) >= 0.25f) CameraMoveForward(camera, -cameraMoveSpeed, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) >= 0.25f) CameraMoveRight(camera, cameraMoveSpeed, moveInWorldPlane);
    }

    if (mode == CAMERA_FREE)
    {
        if (IsKeyDown(KEY_SPACE)) CameraMoveUp(camera, cameraMoveSpeed);
        if (IsKeyDown(KEY_LEFT_CONTROL)) CameraMoveUp(camera, -cameraMoveSpeed);
    }
}
#endif