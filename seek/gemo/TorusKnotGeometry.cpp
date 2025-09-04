#include "TorusKnotGeometry.h"
#include <glm/detail/type_vec.hpp>
#include <glm/detail/func_geometric.hpp>
#include <logger/logger.h>


using namespace Loader;

#define PAR_MALLOC(T, N) ((T *)MALLOC(N*sizeof(T)))
#define PAR_CALLOC(T, N) ((T *)CALLOC(N*sizeof(T), 1))
#define PAR_REALLOC(T, BUF, N) ((T *)REALLOC(BUF, sizeof(T)*(N)))
#define PAR_FREE FREE

#if defined(_MSC_VER)           // Disable some MSVC warning
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)
#endif

#define PAR_SHAPES_IMPLEMENTATION
#include <par_shapes.h>

#if defined(_MSC_VER)
#pragma warning(pop)        // Disable MSVC warning suppression
#endif


#if defined(_WIN32)
#include <direct.h>     // Required for: _chdir() [Used in LoadOBJ()]
#define CHDIR _chdir
#else
#include <unistd.h>     // Required for: chdir() (POSIX) [Used in LoadOBJ()]
#define CHDIR chdir
#endif



namespace GEMO {
#define M_PI  3.1415926




	Mesh GenMeshKnot(float radius, float size, int radSeg, int sides)
	{
		Mesh mesh = { 0 };

		if ((sides >= 3) && (radSeg >= 3))
		{
			if (radius > 3.0f) radius = 3.0f;
			else if (radius < 0.5f) radius = 0.5f;

			par_shapes_mesh* knot = par_shapes_create_trefoil_knot(radSeg, sides, radius);
			par_shapes_scale(knot, size, size, size);

			mesh.vertices = (float*)MALLOC(knot->ntriangles * 3 * 3 * sizeof(float));
			mesh.texcoords = (float*)MALLOC(knot->ntriangles * 3 * 2 * sizeof(float));
			mesh.normals = (float*)MALLOC(knot->ntriangles * 3 * 3 * sizeof(float));

			mesh.vertexCount = knot->ntriangles * 3;
			mesh.triangleCount = knot->ntriangles;

			for (int k = 0; k < mesh.vertexCount; k++)
			{
				mesh.vertices[k * 3] = knot->points[knot->triangles[k] * 3];
				mesh.vertices[k * 3 + 1] = knot->points[knot->triangles[k] * 3 + 1];
				mesh.vertices[k * 3 + 2] = knot->points[knot->triangles[k] * 3 + 2];

				mesh.normals[k * 3] = knot->normals[knot->triangles[k] * 3];
				mesh.normals[k * 3 + 1] = knot->normals[knot->triangles[k] * 3 + 1];
				mesh.normals[k * 3 + 2] = knot->normals[knot->triangles[k] * 3 + 2];

				mesh.texcoords[k * 2] = knot->tcoords[knot->triangles[k] * 2];
				mesh.texcoords[k * 2 + 1] = knot->tcoords[knot->triangles[k] * 2 + 1];
			}

			par_shapes_free_mesh(knot);

			// Upload vertex data to GPU (static mesh)
			UploadMesh(&mesh, false);
		}
		else TRACELOG(LOG_WARNING, "MESH: Failed to generate mesh: knot");

		return mesh;
	}

	Mesh GenMeshCustom(void) {
		Mesh mesh = { 0 };
		mesh.triangleCount = 1;
		mesh.vertexCount = mesh.triangleCount * 3;
		mesh.vertices = (float*)MALLOC(mesh.vertexCount * 3 * sizeof(float));    // 3 vertices, 3 coordinates each (x, y, z)
		mesh.texcoords = (float*)MALLOC(mesh.vertexCount * 2 * sizeof(float));   // 3 vertices, 2 coordinates each (x, y)
		mesh.normals = (float*)MALLOC(mesh.vertexCount * 3 * sizeof(float));     // 3 vertices, 3 coordinates each (x, y, z)

		// Vertex at (0, 0, 0)
		mesh.vertices[0] = 0;
		mesh.vertices[1] = 0;
		mesh.vertices[2] = 0;
		mesh.normals[0] = 0;
		mesh.normals[1] = 1;
		mesh.normals[2] = 0;
		mesh.texcoords[0] = 0;
		mesh.texcoords[1] = 0;

		// Vertex at (1, 0, 2)
		mesh.vertices[3] = 1;
		mesh.vertices[4] = 0;
		mesh.vertices[5] = 2;
		mesh.normals[3] = 0;
		mesh.normals[4] = 1;
		mesh.normals[5] = 0;
		mesh.texcoords[2] = 0.5f;
		mesh.texcoords[3] = 1.0f;

		// Vertex at (2, 0, 0)
		mesh.vertices[6] = 2;
		mesh.vertices[7] = 0;
		mesh.vertices[8] = 0;
		mesh.normals[6] = 0;
		mesh.normals[7] = 1;
		mesh.normals[8] = 0;
		mesh.texcoords[4] = 1;
		mesh.texcoords[5] = 0;

		// Upload mesh data from CPU (RAM) to GPU (VRAM) memory
		UploadMesh(&mesh, false);
		return mesh;
	}

	Mesh generate(const TorusParam& params) {
		Mesh mesh = { 0 };
		std::vector<float> vertexs;
		std::vector<float> normals;
		std::vector<float> textures;
		std::vector<int> inds;
		
		int faceCount = 0;


		for (int i = 0; i <= params.tubularSegments; ++i) {
			float u = float(i) / params.tubularSegments * 2.0f * M_PI * params.q;
			// 纽结中心曲线
			float r = params.radius;
			float cu = cos(u);
			float su = sin(u);

			float x1 = r * (2.0f + cos(params.p * u / 2.0f)) * 0.5f * cu;
			float y1 = r * (2.0f + cos(params.p * u / 2.0f)) * 0.5f * su;
			float z1 = r * sin(params.p * u / 2.0f) * 0.5f;

			// 曲线切线（近似）
			float u2 = u + 0.01f;
			float cu2 = cos(u2);
			float su2 = sin(u2);
			float x2 = r * (2.0f + cos(params.p * u2 / 2.0f)) * 0.5f * cu2;
			float y2 = r * (2.0f + cos(params.p * u2 / 2.0f)) * 0.5f * su2;
			float z2 = r * sin(params.p * u2 / 2.0f) * 0.5f;
			glm::vec3 tang = glm::normalize(glm::vec3{ x2 - x1, y2 - y1, z2 - z1 });
			// 副法线（随便找一个与切线不共线的向量叉乘）
			glm::vec3 up{ 0, 0, 1 };
			glm::vec3 binormal = glm::normalize(glm::cross(tang, up));
			glm::vec3 normal = glm::normalize(glm::cross(binormal, tang));

			for (int j = 0; j <= params.radialSegments; ++j) {
				float v = float(j) / params.radialSegments * 2.0f * M_PI;
				float cv = cos(v);
				float sv = sin(v);

				glm::vec3 pos =
					glm::vec3{ x1, y1, z1 * params.heightScale } +
					(binormal * cv + normal * sv) * params.tube;

				vertexs.push_back(pos.x);
				vertexs.push_back(pos.y);
				vertexs.push_back(pos.z);
				glm::vec3 nrm = binormal * cv + normal * sv;


				normals.push_back(nrm.x);
				normals.push_back(nrm.y);
				normals.push_back(nrm.z);
				float uTex = float(i) / params.tubularSegments;
				float vTex = float(j) / params.radialSegments;

				textures.push_back(uTex);
				textures.push_back(vTex);
				faceCount++;
			}
		}

		mesh.vertices = (float*)MALLOC(faceCount * 3 * sizeof(float));
		memcpy(mesh.vertices, vertexs.data(), faceCount * 3 * sizeof(float));

		mesh.texcoords = (float*)MALLOC(faceCount * 2 * sizeof(float));
		memcpy(mesh.texcoords, textures.data(), faceCount * 2 * sizeof(float));

		mesh.normals = (float*)MALLOC(faceCount * 3 * sizeof(float));
		memcpy(mesh.normals, normals.data(), faceCount * 3 * sizeof(float));

		
		int stride = params.radialSegments + 1;
		for (int i = 0; i < params.tubularSegments; ++i) {
			for (int j = 0; j < params.radialSegments; ++j) {
				int a = i * stride + j;
				int b = a + stride;
				int c = b + 1;
				int d = a + 1;

				//int index = i * params.tubularSegments + j;
				inds.insert(inds.end(), { a, b, d, b, c, d });
			}
		}
		mesh.indices = (unsigned short*)MALLOC((inds.size()) * sizeof(unsigned short));
		for (int i = 0; i < inds.size(); i++) {
			mesh.indices[i] = inds[i];
		}

		mesh.vertexCount = faceCount;
		mesh.triangleCount = inds.size() / 3;

		// Upload vertex data to GPU (static mesh)
		UploadMesh(&mesh, false);
		return mesh;
	}
}