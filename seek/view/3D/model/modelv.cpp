#include "modelv.h"
#include <math/pr_math.h>
#include <section/enum/material_em.h>
#include <rmd/gl/gl_include.h>
#include <section/enum/gltype_em.h>
#include <section/graph/gl_def.h>
#include <core/pass/draw_pass.h>
#include <core/core.h>
#include <file/sys_text.h>
using namespace PMath;
using namespace DRAW::GL;
using namespace RHICore;
using namespace Loader;
using namespace Text;
 int lightsCount = 0;
namespace ModelView {

	// Create a light and get shader locations
	MLight CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader) {
		MLight light = { 0 };

		if (lightsCount < MAX_LIGHTS)
		{
			light.enabled = true;
			light.type = type;
			light.position = position;
			light.target = target;
			light.color = color;

			// NOTE: Lighting shader naming must be the provided ones
			light.enabledLoc = GetShaderLocation(shader, TextFormat("lights[%i].enabled", lightsCount));
			light.typeLoc = GetShaderLocation(shader, TextFormat("lights[%i].type", lightsCount));
			light.positionLoc = GetShaderLocation(shader, TextFormat("lights[%i].position", lightsCount));
			light.targetLoc = GetShaderLocation(shader, TextFormat("lights[%i].target", lightsCount));
			light.colorLoc = GetShaderLocation(shader, TextFormat("lights[%i].color", lightsCount));

			UpdateLightValues(shader, light);

			lightsCount++;
		}

		return light;
	}

	// NOTE: Light shader locations should be available 
	void UpdateLightValues(Shader shader, MLight light) {
		// Send to shader light enabled state and type
		SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
		SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);

		// Send to shader light position values
		float position[3] = { light.position.x, light.position.y, light.position.z };
		SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);

		// Send to shader light target position values
		float target[3] = { light.target.x, light.target.y, light.target.z };
		SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);

		// Send to shader light color values
		float color[4] = { (float)light.color.r / (float)255, (float)light.color.g / (float)255,
						   (float)light.color.b / (float)255, (float)light.color.a / (float)255 };
		SetShaderValue(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4);
	}

	// Generate plane mesh (with subdivisions)
	Mesh GenMeshPlane(float width, float length, int resX, int resZ) {
		Mesh mesh = { 0 };

#define CUSTOM_MESH_GEN_PLANE
#if defined(CUSTOM_MESH_GEN_PLANE)
		resX++;
		resZ++;

		// Vertices definition
		int vertexCount = resX * resZ; // vertices get reused for the faces

		Vector3* vertices = (Vector3*)MALLOC(vertexCount * sizeof(Vector3));
		for (int z = 0; z < resZ; z++)
		{
			// [-length/2, length/2]
			float zPos = ((float)z / (resZ - 1) - 0.5f) * length;
			for (int x = 0; x < resX; x++)
			{
				// [-width/2, width/2]
				float xPos = ((float)x / (resX - 1) - 0.5f) * width;
				vertices[x + z * resX] ={ xPos, 0.0f, zPos };
			}
		}

		// Normals definition
		Vector3* normals = (Vector3*)MALLOC(vertexCount * sizeof(Vector3));
		for (int n = 0; n < vertexCount; n++) normals[n] = { 0.0f, 1.0f, 0.0f };   // Vector3.up;
		 // TexCoords definition
		Vector2* texcoords = (Vector2*)MALLOC(vertexCount * sizeof(Vector2));

		for (int v = 0; v < resZ; v++)
		{
			for (int u = 0; u < resX; u++)
			{
				texcoords[u + v * resX] = { (float)u / (resX - 1), (float)v / (resZ - 1) };
			}
		}

		// Triangles definition (indices)
		int numFaces = (resX - 1) * (resZ - 1);
		int* triangles = (int*)MALLOC(numFaces * 6 * sizeof(int));
		int t = 0;
		for (int face = 0; face < numFaces; face++)
		{
			// Retrieve lower left corner from face ind
			int i = face + face / (resX - 1);

			triangles[t++] = i + resX;
			triangles[t++] = i + 1;
			triangles[t++] = i;

			triangles[t++] = i + resX;
			triangles[t++] = i + resX + 1;
			triangles[t++] = i + 1;
		}
		mesh.vertexCount = vertexCount;
		mesh.triangleCount = numFaces * 2;
		mesh.vertices = (float*)MALLOC(mesh.vertexCount * 3 * sizeof(float));
		mesh.texcoords = (float*)MALLOC(mesh.vertexCount * 2 * sizeof(float));
		mesh.normals = (float*)MALLOC(mesh.vertexCount * 3 * sizeof(float));
		mesh.indices = (unsigned short*)MALLOC(mesh.triangleCount * 3 * sizeof(unsigned short));

		// Mesh vertices position array
		for (int i = 0; i < mesh.vertexCount; i++)
		{
			mesh.vertices[3 * i] = vertices[i].x;
			mesh.vertices[3 * i + 1] = vertices[i].y;
			mesh.vertices[3 * i + 2] = vertices[i].z;
		}

		// Mesh texcoords array
		for (int i = 0; i < mesh.vertexCount; i++)
		{
			mesh.texcoords[2 * i] = texcoords[i].x;
			mesh.texcoords[2 * i + 1] = texcoords[i].y;
		}
		
		// Mesh normals array
		for (int i = 0; i < mesh.vertexCount; i++)
		{
			mesh.normals[3 * i] = normals[i].x;
			mesh.normals[3 * i + 1] = normals[i].y;
			mesh.normals[3 * i + 2] = normals[i].z;
		}
		// Mesh indices array initialization
		for (int i = 0; i < mesh.triangleCount * 3; i++) mesh.indices[i] = triangles[i];

		FREE(vertices);
		FREE(normals);
		FREE(texcoords);
		FREE(triangles);

#endif
		// Upload vertex data to GPU (static mesh)
		UploadMesh(&mesh, false);

		return mesh;
	}

	void DrawCubeWires(Vector3 position, float width, float height, float length, Color color) {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		PushMatrix();
			Translatef(position.x, position.y, position.z);

			Begin(PL_LINES);
			Color4ub(color.r, color.g, color.b, color.a);
			// Front face
		 //------------------------------------------------------------------
		 // Bottom line
			Vertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom left
			Vertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom right

			// Left line
			Vertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom right
			Vertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top right

			// Top line
			Vertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top right
			Vertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top left

			// Right line
			Vertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top left
			Vertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom left

			// Back face
			//------------------------------------------------------------------
			// Bottom line
			Vertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom left
			Vertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom right

			// Left line
			Vertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom right
			Vertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top right

			// Top line
			Vertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top right
			Vertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top left

			// Right line
			Vertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top left
			Vertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom left

			// Top face
			//------------------------------------------------------------------
			// Left line
			Vertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top left front
			Vertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top left back

			// Right line
			Vertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top right front
			Vertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top right back

			// Bottom face
			//------------------------------------------------------------------
			// Left line
			Vertex3f(x - width / 2, y - height / 2, z + length / 2);  // Top left front
			Vertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top left back

			// Right line
			Vertex3f(x + width / 2, y - height / 2, z + length / 2);  // Top right front
			Vertex3f(x + width / 2, y - height / 2, z - length / 2);  // Top right back
			End();
			PopMatrix();
	}

	void DrawCube(Vector3 position, float width, float height, float length, Color color) {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		PushMatrix();
		// NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
		Translatef(position.x, position.y, position.z);
		//rlRotatef(45, 0, 1, 0);
		//rlScalef(1.0f, 1.0f, 1.0f);   // NOTE: Vertices are directly scaled on definition

		Begin(PL_TRIANGLES);
			Color4ub(color.r, color.g, color.b, color.a);

		// Front face
			Normal3f(0.0f, 0.0f, 1.0f);
			Vertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left
			Vertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
			Vertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left

			Vertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right
			Vertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
			Vertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right

		// Back face
			Normal3f(0.0f, 0.0f, -1.0f);
			Vertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left
			Vertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
			Vertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right

			Vertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
			Vertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
			Vertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left

		// Top face
			Normal3f(0.0f, 1.0f, 0.0f);
			Vertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
			Vertex3f(x - width / 2, y + height / 2, z + length / 2);  // Bottom Left
			Vertex3f(x + width / 2, y + height / 2, z + length / 2);  // Bottom Right

			Vertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
			Vertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
			Vertex3f(x + width / 2, y + height / 2, z + length / 2);  // Bottom Right

		// Bottom face
			Normal3f(0.0f, -1.0f, 0.0f);
			Vertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Left
			Vertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
			Vertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left

			Vertex3f(x + width / 2, y - height / 2, z - length / 2);  // Top Right
			Vertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
			Vertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Left

		// Right face
			Normal3f(1.0f, 0.0f, 0.0f);
			Vertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
			Vertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
			Vertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left

			Vertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left
			Vertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
			Vertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left

		// Left face
			Normal3f(-1.0f, 0.0f, 0.0f);
			Vertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right
			Vertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
			Vertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Right

			Vertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left
			Vertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
			Vertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right
		End();
		PopMatrix();
	}

	void DrawCubeV(Vector3 position, Vector3 size, Color color) {
		 DrawCube(position, size.x, size.y, size.z, color);
	}

	void DrawCubeWiresV(Vector3 position, Vector3 size, Color color) {
		DrawCubeWires(position, size.x, size.y, size.z, color);
	}

	// Generated cuboid mesh
	Mesh GenMeshCube(float width, float height, float length) {
		Mesh mesh = { 0 };
#define CUSTOM_MESH_GEN_CUBE
#if defined(CUSTOM_MESH_GEN_CUBE)
		float vertices[] = {
	   -width / 2, -height / 2, length / 2,
	   width / 2, -height / 2, length / 2,
	   width / 2, height / 2, length / 2,
	   -width / 2, height / 2, length / 2,
	   -width / 2, -height / 2, -length / 2,
	   -width / 2, height / 2, -length / 2,
	   width / 2, height / 2, -length / 2,
	   width / 2, -height / 2, -length / 2,
	   -width / 2, height / 2, -length / 2,
	   -width / 2, height / 2, length / 2,
	   width / 2, height / 2, length / 2,
	   width / 2, height / 2, -length / 2,
	   -width / 2, -height / 2, -length / 2,
	   width / 2, -height / 2, -length / 2,
	   width / 2, -height / 2, length / 2,
	   -width / 2, -height / 2, length / 2,
	   width / 2, -height / 2, -length / 2,
	   width / 2, height / 2, -length / 2,
	   width / 2, height / 2, length / 2,
	   width / 2, -height / 2, length / 2,
	   -width / 2, -height / 2, -length / 2,
	   -width / 2, -height / 2, length / 2,
	   -width / 2, height / 2, length / 2,
	   -width / 2, height / 2, -length / 2
		};

		float texcoords[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};

		float normals[] = {
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f,-1.0f,
			0.0f, 0.0f,-1.0f,
			0.0f, 0.0f,-1.0f,
			0.0f, 0.0f,-1.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f,-1.0f, 0.0f,
			0.0f,-1.0f, 0.0f,
			0.0f,-1.0f, 0.0f,
			0.0f,-1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f
		};

		mesh.vertices = (float*)MALLOC(24 * 3 * sizeof(float));
		memcpy(mesh.vertices, vertices, 24 * 3 * sizeof(float));

		mesh.texcoords = (float*)MALLOC(24 * 2 * sizeof(float));
		memcpy(mesh.texcoords, texcoords, 24 * 2 * sizeof(float));

		mesh.normals = (float*)MALLOC(24 * 3 * sizeof(float));
		memcpy(mesh.normals, normals, 24 * 3 * sizeof(float));

		mesh.indices = (unsigned short*)MALLOC(36 * sizeof(unsigned short));

		int k = 0;

		// Indices can be initialized right now
		for (int i = 0; i < 36; i += 6)
		{
			mesh.indices[i] = 4 * k;
			mesh.indices[i + 1] = 4 * k + 1;
			mesh.indices[i + 2] = 4 * k + 2;
			mesh.indices[i + 3] = 4 * k;
			mesh.indices[i + 4] = 4 * k + 2;
			mesh.indices[i + 5] = 4 * k + 3;

			k++;
		}
		mesh.vertexCount = 24;
		mesh.triangleCount = 12;
#endif
		// Upload vertex data to GPU (static mesh)
		UploadMesh(&mesh, false);
		return mesh;
	}

	// Load model from generated mesh
// WARNING: A shallow copy of mesh is generated, passed by value,
// as long as struct contains pointers to data and some values, we get a copy
// of mesh pointing to same data as original version... be careful!
	Model LoadModelFromMesh(Mesh mesh) {

		Model model = { 0 };

		model.transform = MatrixIdentity();

		model.meshCount = 1;
		model.meshes = (Mesh*)CALLOC(model.meshCount, sizeof(Mesh));
		model.meshes[0] = mesh;

		model.materialCount = 1;
		model.materials = (Material*)CALLOC(model.materialCount, sizeof(Material));
		model.materials[0] = LoadMaterialDefault();

		model.meshMaterial = (int*)CALLOC(model.meshCount, sizeof(int));
		model.meshMaterial[0] = 0;  // First material index

		return model;
	}

	// Draw sphere wires
	void DrawSphereWires(Vector3 centerPos, float radius, int rings, int slices, Color color) {
		PushMatrix();
			Translatef(centerPos.x, centerPos.y, centerPos.z);
				Scalef(radius, radius, radius);
				Begin(PL_LINES);
				Color4ub(color.r, color.g, color.b, color.a);
				for (int i = 0; i < (rings + 2); i++)
				{
					for (int j = 0; j < slices; j++)
					{
						Vertex3f(cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * i)) * sinf(DEG2RAD * (360.0f * j / slices)),
							sinf(DEG2RAD * (270 + (180.0f / (rings + 1)) * i)),
							cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * i)) * cosf(DEG2RAD * (360.0f * j / slices)));
						Vertex3f(cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))) * sinf(DEG2RAD * (360.0f * (j + 1) / slices)),
							sinf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))),
							cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))) * cosf(DEG2RAD * (360.0f * (j + 1) / slices)));

						Vertex3f(cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))) * sinf(DEG2RAD * (360.0f * (j + 1) / slices)),
							sinf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))),
							cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))) * cosf(DEG2RAD * (360.0f * (j + 1) / slices)));
						Vertex3f(cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))) * sinf(DEG2RAD * (360.0f * j / slices)),
							sinf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))),
							cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))) * cosf(DEG2RAD * (360.0f * j / slices)));

						Vertex3f(cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))) * sinf(DEG2RAD * (360.0f * j / slices)),
							sinf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))),
							cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * (i + 1))) * cosf(DEG2RAD * (360.0f * j / slices)));
						Vertex3f(cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * i)) * sinf(DEG2RAD * (360.0f * j / slices)),
							sinf(DEG2RAD * (270 + (180.0f / (rings + 1)) * i)),
							cosf(DEG2RAD * (270 + (180.0f / (rings + 1)) * i)) * cosf(DEG2RAD * (360.0f * j / slices)));
					}
				}
				End();
				PopMatrix();
	}

	// Draw sphere with extended parameters
	void DrawSphereEx(Vector3 centerPos, float radius, int rings, int slices, Color color) {

		PushMatrix();
		// NOTE: Transformation is applied in inverse order (scale -> translate)
		Translatef(centerPos.x, centerPos.y, centerPos.z);
		Scalef(radius, radius, radius);
		Begin(PL_TRIANGLES);
		Color4ub(color.r, color.g, color.b, color.a);

		float ringangle = DEG2RAD * (180.0f / (rings + 1)); // Angle between latitudinal parallels
		float sliceangle = DEG2RAD * (360.0f / slices); // Angle between longitudinal meridians

		float cosring = cosf(ringangle);
		float sinring = sinf(ringangle);
		float cosslice = cosf(sliceangle);
		float sinslice = sinf(sliceangle);

		Vector3 vertices[4] = { 0 }; // Required to store face vertices
		vertices[2] = { 0, 1, 0 };
		vertices[3] = { sinring, cosring, 0 };
		for (int i = 0; i < rings + 1; i++)
		{
			for (int j = 0; j < slices; j++)
			{
				vertices[0] = vertices[2]; // Rotate around y axis to set up vertices for next face
				vertices[1] = vertices[3];
				vertices[2] = { cosslice * vertices[2].x - sinslice * vertices[2].z, vertices[2].y, sinslice * vertices[2].x + cosslice * vertices[2].z }; // Rotation matrix around y axis
				vertices[3] =  { cosslice * vertices[3].x - sinslice * vertices[3].z, vertices[3].y, sinslice * vertices[3].x + cosslice * vertices[3].z };

				Normal3f(vertices[0].x, vertices[0].y, vertices[0].z);
				Vertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
				Normal3f(vertices[3].x, vertices[3].y, vertices[3].z);
				Vertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
				Normal3f(vertices[1].x, vertices[1].y, vertices[1].z);
				Vertex3f(vertices[1].x, vertices[1].y, vertices[1].z);

				Normal3f(vertices[0].x, vertices[0].y, vertices[0].z);
				Vertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
				Normal3f(vertices[2].x, vertices[2].y, vertices[2].z);
				Vertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
				Normal3f(vertices[3].x, vertices[3].y, vertices[3].z);
				Vertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
			}

			vertices[2] = vertices[3]; // Rotate around z axis to set up  starting vertices for next ring
			vertices[3] = { cosring * vertices[3].x + sinring * vertices[3].y, -sinring * vertices[3].x + cosring * vertices[3].y, vertices[3].z }; // Rotation matrix around z axis
		}
		End();
		PopMatrix();
	}

	// Update model animated bones transform matrices for a given frame
// NOTE: Updated data is not uploaded to GPU but kept at model.meshes[i].boneMatrices[boneId],
// to be uploaded to shader at drawing, in case GPU skinning is enabled
	void UpdateModelAnimationBones(Model model, ModelAnimation anim, int frame) {
		if ((anim.frameCount > 0) && (anim.bones != NULL) && (anim.framePoses != NULL))
		{
			if (frame >= anim.frameCount) frame = frame % anim.frameCount;

			// Get first mesh which have bones
			int firstMeshWithBones = -1;

			for (int i = 0; i < model.meshCount; i++)
			{
				if (model.meshes[i].boneMatrices)
				{
					if (firstMeshWithBones == -1)
					{
						firstMeshWithBones = i;
						break;
					}
				}
			}


			if (firstMeshWithBones != -1)
			{
				// Update all bones and boneMatrices of first mesh with bones.
				for (int boneId = 0; boneId < anim.boneCount; boneId++)
				{
					Transform* bindTransform = &model.bindPose[boneId];
					Matrix bindMatrix = MatrixMultiply(MatrixMultiply(
						MatrixScale(bindTransform->scale.x, bindTransform->scale.y, bindTransform->scale.z),
						QuaternionToMatrix(bindTransform->rotation)),
						MatrixTranslate(bindTransform->translation.x, bindTransform->translation.y, bindTransform->translation.z));

					Transform* targetTransform = &anim.framePoses[frame][boneId];
					Matrix targetMatrix = MatrixMultiply(MatrixMultiply(
						MatrixScale(targetTransform->scale.x, targetTransform->scale.y, targetTransform->scale.z),
						QuaternionToMatrix(targetTransform->rotation)),
						MatrixTranslate(targetTransform->translation.x, targetTransform->translation.y, targetTransform->translation.z));

					model.meshes[firstMeshWithBones].boneMatrices[boneId] = MatrixMultiply(MatrixInvert(bindMatrix), targetMatrix);
				}
				// Update remaining meshes with bones
		  // NOTE: Using deep copy because shallow copy results in double free with 'UnloadModel()'
				for (int i = firstMeshWithBones + 1; i < model.meshCount; i++)
				{
					if (model.meshes[i].boneMatrices)
					{
						memcpy(model.meshes[i].boneMatrices,
							model.meshes[firstMeshWithBones].boneMatrices,
							model.meshes[i].boneCount * sizeof(model.meshes[i].boneMatrices[0]));
					}
				}
			}
		}
	}

	// at least 2x speed up vs the old method
// Update model animated vertex data (positions and normals) for a given frame
// NOTE: Updated data is uploaded to GPU
	void UpdateModelAnimation(Model model, ModelAnimation anim, int frame) {
		
		UpdateModelAnimationBones(model, anim, frame);

		for (int m = 0; m < model.meshCount; m++)
		{

			Mesh mesh = model.meshes[m];
			Vector3 animVertex = { 0 };
			Vector3 animNormal = { 0 };
			int boneId = 0;
			int boneCounter = 0;
			float boneWeight = 0.0;
			bool updated = false; // Flag to check when anim vertex information is updated
			const int vValues = mesh.vertexCount * 3;

			// Skip if missing bone data, causes segfault without on some models
			if ((mesh.boneWeights == NULL) || (mesh.boneIds == NULL)) continue;
			for (int vCounter = 0; vCounter < vValues; vCounter += 3)
			{
				mesh.animVertices[vCounter] = 0;
				mesh.animVertices[vCounter + 1] = 0;
				mesh.animVertices[vCounter + 2] = 0;
				if (mesh.animNormals != NULL)
				{
					mesh.animNormals[vCounter] = 0;
					mesh.animNormals[vCounter + 1] = 0;
					mesh.animNormals[vCounter + 2] = 0;
				}
				for (int j = 0; j < 4; j++, boneCounter++)
				{
					boneWeight = mesh.boneWeights[boneCounter];
					boneId = mesh.boneIds[boneCounter];

					// Early stop when no transformation will be applied
					if (boneWeight == 0.0f) continue;
					animVertex ={ mesh.vertices[vCounter], mesh.vertices[vCounter + 1], mesh.vertices[vCounter + 2] };
					animVertex = Vector3Transform(animVertex, model.meshes[m].boneMatrices[boneId]);
					mesh.animVertices[vCounter] += animVertex.x * boneWeight;
					mesh.animVertices[vCounter + 1] += animVertex.y * boneWeight;
					mesh.animVertices[vCounter + 2] += animVertex.z * boneWeight;
					updated = true;

					// Normals processing
					// NOTE: We use meshes.baseNormals (default normal) to calculate meshes.normals (animated normals)
					if ((mesh.normals != NULL) && (mesh.animNormals != NULL))
					{
						animNormal = { mesh.normals[vCounter], mesh.normals[vCounter + 1], mesh.normals[vCounter + 2] };
						animNormal = Vector3Transform(animNormal, MatrixTranspose(MatrixInvert(model.meshes[m].boneMatrices[boneId])));
						mesh.animNormals[vCounter] += animNormal.x * boneWeight;
						mesh.animNormals[vCounter + 1] += animNormal.y * boneWeight;
						mesh.animNormals[vCounter + 2] += animNormal.z * boneWeight;
					}
				}
			
			}

			if (updated)
			{
				UpdateVertexBuffer(mesh.vboId[0], mesh.animVertices, mesh.vertexCount * 3 * sizeof(float), 0); // Update vertex position
				if (mesh.normals != NULL) UpdateVertexBuffer(mesh.vboId[2], mesh.animNormals, mesh.vertexCount * 3 * sizeof(float), 0); // Update vertex normals
			}
		}
	}
	
	// Draw a model (with texture if set)
	void DrawModel(Model model, Vector3 position, float scale, Color tint) {
		Vector3 vScale = { scale, scale, scale };
		Vector3 rotationAxis = { 0.0f, 1.0f, 0.0f };
		DrawModelEx(model, position, rotationAxis, 0.0f, vScale, tint);
	}	

	// Draw a model with extended parameters
	void DrawModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint) {
		// Calculate transformation matrix from function parameters
	// Get transform matrix (rotation -> scale -> translation)
		Matrix matScale = MatrixScale(scale.x, scale.y, scale.z);
		Matrix matRotation = MatrixRotate(rotationAxis, rotationAngle * DEG2RAD);
		Matrix matTranslation = MatrixTranslate(position.x, position.y, position.z);

		Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
		// Combine model transformation matrix (model.transform) with matrix generated by function parameters (matTransform)
		model.transform = MatrixMultiply(model.transform, matTransform);
		
		for (int i = 0; i < model.meshCount; i++)
		{
			Color color = model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color;

			Color colorTint = WHITE;
			colorTint.r = (unsigned char)(((int)color.r * (int)tint.r) / 255);
			colorTint.g = (unsigned char)(((int)color.g * (int)tint.g) / 255);
			colorTint.b = (unsigned char)(((int)color.b * (int)tint.b) / 255);
			colorTint.a = (unsigned char)(((int)color.a * (int)tint.a) / 255);

			model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color = colorTint;
			DrawMesh(model.meshes[i], model.materials[model.meshMaterial[i]], model.transform);
			model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color = color;
		}
	}

	// Draw a 3d mesh with material and transform
	void DrawMesh(Mesh mesh, Material material, Matrix transform) {

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		EnableShader(material.shader.id);
		if (material.shader.locs[SHADER_LOC_COLOR_DIFFUSE] != -1) {
			float values[4] = {
		  (float)material.maps[MATERIAL_MAP_DIFFUSE].color.r / 255.0f,
		  (float)material.maps[MATERIAL_MAP_DIFFUSE].color.g / 255.0f,
		  (float)material.maps[MATERIAL_MAP_DIFFUSE].color.b / 255.0f,
		  (float)material.maps[MATERIAL_MAP_DIFFUSE].color.a / 255.0f
			};
			SetUniform(material.shader.locs[SHADER_LOC_COLOR_DIFFUSE], values, SHADER_UNIFORM_VEC4, 1);
		}

		// Upload to shader material.colSpecular (if location available)
		if (material.shader.locs[SHADER_LOC_COLOR_SPECULAR] != -1)
		{
			float values[4] = {
				(float)material.maps[MATERIAL_MAP_SPECULAR].color.r / 255.0f,
				(float)material.maps[MATERIAL_MAP_SPECULAR].color.g / 255.0f,
				(float)material.maps[MATERIAL_MAP_SPECULAR].color.b / 255.0f,
				(float)material.maps[MATERIAL_MAP_SPECULAR].color.a / 255.0f
			};

			SetUniform(material.shader.locs[SHADER_LOC_COLOR_SPECULAR], values, SHADER_UNIFORM_VEC4, 1);
		}
		// Get a copy of current matrices to work with,
   // just in case stereo render is required, and we need to modify them
   // NOTE: At this point the modelview matrix just contains the view matrix (camera)
   // That's because BeginMode3D() sets it and there is no model-drawing function
   // that modifies it, all use rlPushMatrix() and rlPopMatrix()
		Matrix matModel = MatrixIdentity();
		Matrix matView = GetMatrixModelview();
		Matrix matModelView = MatrixIdentity();
		Matrix matProjection = GetMatrixProjection();
		// Upload view and projection matrices (if locations available)
		if (material.shader.locs[SHADER_LOC_MATRIX_VIEW] != -1) SetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_VIEW], matView);
		if (material.shader.locs[SHADER_LOC_MATRIX_PROJECTION] != -1) SetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_PROJECTION], matProjection);
		
		// Accumulate several model transformations:
		//    transform: model transformation provided (includes DrawModel() params combined with model.transform)
		//    rlGetMatrixTransform(): PLGL internal transform matrix due to push/pop matrix stack
		matModel = MatrixMultiply(transform, GetMatrixTransform());

		// Model transformation matrix is sent to shader uniform location: SHADER_LOC_MATRIX_MODEL
		if (material.shader.locs[SHADER_LOC_MATRIX_MODEL] != -1) SetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MODEL], matModel);

		// Get model-view matrix
		matModelView = MatrixMultiply(matModel, matView);
		// Upload model normal matrix (if locations available)
		if (material.shader.locs[SHADER_LOC_MATRIX_NORMAL] != -1) SetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_NORMAL], MatrixTranspose(MatrixInvert(matModel)));
#ifdef PL_SUPPORT_MESH_GPU_SKINNING
		// Upload Bone Transforms
		if ((material.shader.locs[SHADER_LOC_BONE_MATRICES] != -1) && mesh.boneMatrices)
		{
			SetUniformMatrices(material.shader.locs[SHADER_LOC_BONE_MATRICES], mesh.boneMatrices, mesh.boneCount);
		}
#endif

		// Bind active texture maps (if available)
		for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
		{
			if (material.maps[i].texture.id > 0)
			{
				// Select current shader texture slot
				ActiveTextureSlot(i);

				// Enable texture for active slot
				if ((i == MATERIAL_MAP_IRRADIANCE) ||
					(i == MATERIAL_MAP_PREFILTER) ||
					(i == MATERIAL_MAP_CUBEMAP)) EnableTextureCubemap(material.maps[i].texture.id);
				else EnableTexture(material.maps[i].texture.id);

				SetUniform(material.shader.locs[SHADER_LOC_MAP_DIFFUSE + i], &i, SHADER_UNIFORM_INT, 1);
			}
		}

		// Try binding vertex array objects (VAO) or use VBOs if not possible
		// WARNING: UploadMesh() enables all vertex attributes available in mesh and sets default attribute values
		// for shader expected vertex attributes that are not provided by the mesh (i.e. colors)
		// This could be a dangerous approach because different meshes with different shaders can enable/disable some attributes
		if (!EnableVertexArray(mesh.vaoId)) {
			// Bind mesh VBO data: vertex position (shader-location = 0)
			EnableVertexBuffer(mesh.vboId[PL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION]);
			SetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_POSITION], 3, PL_FLOAT, 0, 0, 0);
			EnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_POSITION]);

			// Bind mesh VBO data: vertex texcoords (shader-location = 1)
			EnableVertexBuffer(mesh.vboId[PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD]);
			SetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01], 2, PL_FLOAT, 0, 0, 0);
			EnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01]);

			if (material.shader.locs[SHADER_LOC_VERTEX_NORMAL] != -1)
			{
				// Bind mesh VBO data: vertex normals (shader-location = 2)
				EnableVertexBuffer(mesh.vboId[PL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL]);
				SetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_NORMAL], 3, PL_FLOAT, 0, 0, 0);
				EnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_NORMAL]);
			}
			// Bind mesh VBO data: vertex colors (shader-location = 3, if available)
			if (material.shader.locs[SHADER_LOC_VERTEX_COLOR] != -1) {
				if (mesh.vboId[PL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR] != 0)
				{
					EnableVertexBuffer(mesh.vboId[PL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR]);
					SetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR], 4, PL_UNSIGNED_BYTE, 1, 0, 0);
					EnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
				}
				else
				{
					// Set default value for defined vertex attribute in shader but not provided by mesh
					// WARNING: It could result in GPU undefined behaviour
					float value[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
					SetVertexAttributeDefault(material.shader.locs[SHADER_LOC_VERTEX_COLOR], value, SHADER_ATTRIB_VEC4, 4);
					DisableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
				}
			}

			// Bind mesh VBO data: vertex tangents (shader-location = 4, if available)
			if (material.shader.locs[SHADER_LOC_VERTEX_TANGENT] != -1)
			{
				EnableVertexBuffer(mesh.vboId[PL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT]);
				SetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TANGENT], 4, PL_FLOAT, 0, 0, 0);
				EnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TANGENT]);
			}
			
			// Bind mesh VBO data: vertex texcoords2 (shader-location = 5, if available)
			if (material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02] != -1)
			{
				EnableVertexBuffer(mesh.vboId[PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2]);
				SetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02], 2, PL_FLOAT, 0, 0, 0);
				EnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02]);
			}

#if defined(RL_SUPPORT_MESH_GPU_SKINNING)
			// Bind mesh VBO data: vertex bone ids (shader-location = 6, if available)
			if (material.shader.locs[SHADER_LOC_VERTEX_BONEIDS] != -1)
			{
				EnableVertexBuffer(mesh.vboId[PL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEIDS]);
				SetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_BONEIDS], 4, PL_UNSIGNED_BYTE, 0, 0, 0);
				EnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_BONEIDS]);
			}
			// Bind mesh VBO data: vertex bone weights (shader-location = 7, if available)
			if (material.shader.locs[SHADER_LOC_VERTEX_BONEWEIGHTS] != -1)
			{
				rlEnableVertexBuffer(mesh.vboId[PL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS]);
				rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_BONEWEIGHTS], 4, PL_FLOAT, 0, 0, 0);
				rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_BONEWEIGHTS]);
			}
#endif
			if (mesh.indices != NULL) EnableVertexBufferElement(mesh.vboId[PL_DEFAULT_SHADER_ATTRIB_LOCATION_INDICES]);


		}

		int eyeCount = 1;
		if (IsStereoRenderEnabled()) eyeCount = 2;
		for (int eye = 0; eye < eyeCount; eye++)
		{
			// Calculate model-view-projection matrix (MVP)
			Matrix matModelViewProjection = MatrixIdentity();
			if (eyeCount == 1) matModelViewProjection = MatrixMultiply(matModelView, matProjection);
			else
			{
				// Setup current eye viewport (half screen width)
				Viewport(eye * GetFramebufferWidth() / 2, 0, GetFramebufferWidth() / 2, GetFramebufferHeight());
				matModelViewProjection = MatrixMultiply(MatrixMultiply(matModelView, GetMatrixViewOffsetStereo(eye)),GetMatrixProjectionStereo(eye));
			}

			// Send combined model-view-projection matrix to shader
			SetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MVP], matModelViewProjection);

			// Draw mesh
			if (mesh.indices != NULL) DrawVertexArrayElements(0, mesh.triangleCount * 3, 0);
			else DrawVertexArray(0, mesh.vertexCount);
		}
		// Unbind all bound texture maps
		for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
		{
			if (material.maps[i].texture.id > 0)
			{
				// Select current shader texture slot
				ActiveTextureSlot(i);

				// Disable texture for active slot
				if ((i == MATERIAL_MAP_IRRADIANCE) ||
					(i == MATERIAL_MAP_PREFILTER) ||
					(i == MATERIAL_MAP_CUBEMAP)) DisableTextureCubemap();
				else DisableTexture();
			}
		}

		// Disable all possible vertex array objects (or VBOs)
		DisableVertexArray();
		DisableVertexBuffer();
		DisableVertexBufferElement();

		// Disable shader program
		DisableShader();

		// Restore plgl internal modelview and projection matrices
		SetMatrixModelview(matView);
		SetMatrixProjection(matProjection);
#endif
	}


	// Get collision info between ray and box
	RayCollision GetRayCollisionBox(Ray ray, BoundingBox box) {
		RayCollision collision = { 0 };
		// Note: If ray.position is inside the box, the distance is negative (as if the ray was reversed)
   // Reversing ray.direction will give use the correct result
		bool insideBox = (ray.position.x > box.min.x) && (ray.position.x < box.max.x) &&
			(ray.position.y > box.min.y) && (ray.position.y < box.max.y) &&
			(ray.position.z > box.min.z) && (ray.position.z < box.max.z);

		if (insideBox) ray.direction = Vector3Negate(ray.direction);

		float t[11] = { 0 };


		t[8] = 1.0f / ray.direction.x;
		t[9] = 1.0f / ray.direction.y;
		t[10] = 1.0f / ray.direction.z;
		
		t[0] = (box.min.x - ray.position.x) * t[8];
		t[1] = (box.max.x - ray.position.x) * t[8];
		t[2] = (box.min.y - ray.position.y) * t[9];
		t[3] = (box.max.y - ray.position.y) * t[9];
		t[4] = (box.min.z - ray.position.z) * t[10];
		t[5] = (box.max.z - ray.position.z) * t[10];
		t[6] = (float)fmax(fmax(fmin(t[0], t[1]), fmin(t[2], t[3])), fmin(t[4], t[5]));
		t[7] = (float)fmin(fmin(fmax(t[0], t[1]), fmax(t[2], t[3])), fmax(t[4], t[5]));

		collision.hit = !((t[7] < 0) || (t[6] > t[7]));
		collision.distance = t[6];
		collision.point = Vector3Add(ray.position, Vector3Scale(ray.direction, collision.distance));

		// Get box center point
		collision.normal = Vector3Lerp(box.min, box.max, 0.5f);
		// Get vector center point->hit point
		collision.normal = Vector3Subtract(collision.point, collision.normal);
		// Scale vector to unit cube
		// NOTE: We use an additional .01 to fix numerical errors
		collision.normal = Vector3Scale(collision.normal, 2.01f);
		collision.normal = Vector3Divide(collision.normal, Vector3Subtract(box.max, box.min));
		// The relevant elements of the vector are now slightly larger than 1.0f (or smaller than -1.0f)
		// and the others are somewhere between -1.0 and 1.0 casting to int is exactly our wanted normal!
		collision.normal.x = (float)((int)collision.normal.x);
		collision.normal.y = (float)((int)collision.normal.y);
		collision.normal.z = (float)((int)collision.normal.z);

		collision.normal = Vector3Normalize(collision.normal);

		if (insideBox)
		{
			// Reset ray.direction
			ray.direction = Vector3Negate(ray.direction);
			// Fix result
			collision.distance *= -1.0f;
			collision.normal = Vector3Negate(collision.normal);
		}

		return collision;

	}
}