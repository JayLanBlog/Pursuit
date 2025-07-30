#pragma once


namespace Seek {
	// define for data
	typedef struct Vector2 {
		float x;                // Vector x component
		float y;                // Vector y component
	} Vector2;

	// Vector3, 3 components
	typedef struct Vector3 {
		float x;                // Vector x component
		float y;                // Vector y component
		float z;                // Vector z component
	} Vector3;

	// Vector4, 4 components
	typedef struct Vector4 {
		float x;                // Vector x component
		float y;                // Vector y component
		float z;                // Vector z component
		float w;                // Vector w component
	} Vector4;

	// Quaternion, 4 components (Vector4 alias)
	typedef Vector4 Quaternion;
	// Matrix, 4x4 components, column major, OpenGL style, right-handed
	typedef struct Matrix {
		float m0, m4, m8, m12;  // Matrix first row (4 components)
		float m1, m5, m9, m13;  // Matrix second row (4 components)
		float m2, m6, m10, m14; // Matrix third row (4 components)
		float m3, m7, m11, m15; // Matrix fourth row (4 components)
	} Matrix;

	// Rectangle, 4 components
	typedef struct Rectangle {
		float x;                // Rectangle top-left corner position x
		float y;                // Rectangle top-left corner position y
		float width;            // Rectangle width
		float height;           // Rectangle height
	} Rectangle;

	// Transform, vertex transformation data
	typedef struct Transform {
		Vector3 translation;    // Translation
		Quaternion rotation;    // Rotation
		Vector3 scale;          // Scale
	} Transform;



	typedef struct Camera3D {
		Vector3 position;       // Camera position
		Vector3 target;         // Camera target it looks-at
		Vector3 up;             // Camera up vector (rotation over its axis)
		float fovy;             // Camera field-of-view aperture in Y (degrees) in perspective, used as near plane width in orthographic
		int projection;         // Camera projection: CAMERA_PERSPECTIVE or CAMERA_ORTHOGRAPHIC
	} Camera3D;
	typedef Camera3D Camera;

	// Camera2D, defines position/orientation in 2d space
	typedef struct Camera2D {
		Vector2 offset;         // Camera offset (displacement from target)
		Vector2 target;         // Camera target (rotation and zoom origin)
		float rotation;         // Camera rotation in degrees
		float zoom;             // Camera zoom (scaling), should be 1.0f by default
	} Camera2D;


}