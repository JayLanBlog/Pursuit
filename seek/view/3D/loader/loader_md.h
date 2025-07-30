#pragma once
#include <section/graph/model_def.h>
#include <file/sys_file.h>
#include <section/graph/material_def.h>

#define GLTF_ANIMDELAY 17 
using namespace Seek;
namespace Loader{

	// Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
	Material LoadMaterialDefault(void);

	void UploadMesh(Mesh* mesh, bool dynamic);

	Model LoadIQM(const char* fileName);

	Model LoadOBJ(const char* fileName);

	Model LoadGLTF(const char* fileName);

	Model LoadVOX(const char* fileName);

	Model LoadM3D(const char* fileName);

	Model LoadModel(const char * fileName);

	ModelAnimation* LoadModelAnimationsGLTF(const char* fileName, int* animCount);

	// Load model animations from file
	ModelAnimation* LoadModelAnimations(const char* fileName, int* animCount);
}