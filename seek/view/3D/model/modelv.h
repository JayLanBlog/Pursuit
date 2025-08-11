#pragma once
#include <view/3D/loader/loader_md.h>

extern int lightsCount;

namespace ModelView {

	// Draw cube
// NOTE: Cube position is the center position
	void DrawCube(Vector3 position, float width, float height, float length, Color color);

	void DrawCubeV(Vector3 position, Vector3 size, Color color);

	void DrawCubeWires(Vector3 position, float width, float height, float length, Color color);

	void DrawCubeWiresV(Vector3 position, Vector3 size, Color color);
	// Generated cuboid mesh
	Mesh GenMeshCube(float width, float height, float length);

	// Load model from generated mesh
// WARNING: A shallow copy of mesh is generated, passed by value,
// as long as struct contains pointers to data and some values, we get a copy
// of mesh pointing to same data as original version... be careful!
	Model LoadModelFromMesh(Mesh mesh);

	// Draw a model (with texture if set)
	void DrawModel(Model model, Vector3 position, float scale, Color tint);

	// Draw a model with extended parameters
	void DrawModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint);

	// Draw a 3d mesh with material and transform
	void DrawMesh(Mesh mesh, Material material, Matrix transform);


	// Get collision info between ray and box
	RayCollision GetRayCollisionBox(Ray ray, BoundingBox box);
	// Draw multiple mesh instances with material and different transforms
	//void DrawMeshInstanced(Mesh mesh, Material material, const Matrix* transforms, int instances);

	//// Unload mesh from memory (RAM and VRAM)
	//void UnloadMesh(Mesh mesh);

	//// Export mesh data to file
	//bool ExportMesh(Mesh mesh, const char* fileName);
	// at least 2x speed up vs the old method
// Update model animated vertex data (positions and normals) for a given frame
// NOTE: Updated data is uploaded to GPU
	void UpdateModelAnimation(Model model, ModelAnimation anim, int frame);

	// Update model animated bones transform matrices for a given frame
// NOTE: Updated data is not uploaded to GPU but kept at model.meshes[i].boneMatrices[boneId],
// to be uploaded to shader at drawing, in case GPU skinning is enabled
	void UpdateModelAnimationBones(Model model, ModelAnimation anim, int frame);

	// Draw sphere wires
	void DrawSphereWires(Vector3 centerPos, float radius, int rings, int slices, Color color);

	// Draw sphere with extended parameters
	void DrawSphereEx(Vector3 centerPos, float radius, int rings, int slices, Color color);

	// Generate plane mesh (with subdivisions)
	Mesh GenMeshPlane(float width, float length, int resX, int resZ);

	// Create a light and get shader locations
	MLight CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader);

	// NOTE: Light shader locations should be available 
	void UpdateLightValues(Shader shader, MLight light);
}