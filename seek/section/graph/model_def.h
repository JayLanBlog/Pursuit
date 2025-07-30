#pragma once
#include <section/martrix/cg_def.h>
#include<section/graph/material_def.h>

namespace Seek {
    // Mesh, vertex data and vao/vbo
    typedef struct Mesh {
        int vertexCount;        // Number of vertices stored in arrays
        int triangleCount;      // Number of triangles stored (indexed or not)

        // Vertex attributes data
        float* vertices;        // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
        float* texcoords;       // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
        float* texcoords2;      // Vertex texture second coordinates (UV - 2 components per vertex) (shader-location = 5)
        float* normals;         // Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
        float* tangents;        // Vertex tangents (XYZW - 4 components per vertex) (shader-location = 4)
        unsigned char* colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
        unsigned short* indices;    // Vertex indices (in case vertex data comes indexed)
        // Animation vertex data
        float* animVertices;    // Animated vertex positions (after bones transformations)
        float* animNormals;     // Animated normals (after bones transformations)
        unsigned char* boneIds; // Vertex bone ids, max 255 bone ids, up to 4 bones influence by vertex (skinning) (shader-location = 6)
        float* boneWeights;     // Vertex bone weight, up to 4 bones influence by vertex (skinning) (shader-location = 7)
        Matrix* boneMatrices;   // Bones animated transformation matrices
        int boneCount;          // Number of bones
        // OpenGL identifiers
        unsigned int vaoId;     // OpenGL Vertex Array Object id
        unsigned int* vboId;    // OpenGL Vertex Buffer Objects id (default vertex data)
    } Mesh;

    typedef struct BoneInfo {
        char name[32];          // Bone name
        int parent;             // Bone parent
    } BoneInfo;

    // Model, meshes, materials and animation data
    typedef struct Model {
        Matrix transform;       // Local transform matrix

        int meshCount;          // Number of meshes
        int materialCount;      // Number of materials
        Mesh* meshes;           // Meshes array
        Material* materials;    // Materials array
        int* meshMaterial;      // Mesh material number

        // Animation data
        int boneCount;          // Number of bones
        BoneInfo* bones;        // Bones information (skeleton)
        Transform* bindPose;    // Bones base transformation (pose)
    } Model;

    // ModelAnimation
    typedef struct ModelAnimation {
        int boneCount;          // Number of bones
        int frameCount;         // Number of animation frames
        BoneInfo* bones;        // Bones information (skeleton)
        Transform** framePoses; // Poses array by frame
        char name[32];          // Animation name
    } ModelAnimation;

    // Ray, ray for raycasting
    typedef struct Ray {
        Vector3 position;       // Ray position (origin)
        Vector3 direction;      // Ray direction (normalized)
    } Ray;
    // RayCollision, ray hit information
    typedef struct RayCollision {
        bool hit;               // Did the ray hit something?
        float distance;         // Distance to the nearest hit
        Vector3 point;          // Point of the nearest hit
        Vector3 normal;         // Surface normal of hit
    } RayCollision;

    // BoundingBox
    typedef struct BoundingBox {
        Vector3 min;            // Minimum vertex box-corner
        Vector3 max;            // Maximum vertex box-corner
    } BoundingBox;
}

