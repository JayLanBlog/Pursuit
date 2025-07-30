#pragma once


// Default shader vertex attribute locations
#ifndef PL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION    0
#endif
#ifndef PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD    1
#endif
#ifndef PL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL      2
#endif
#ifndef PL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR       3
#endif
#ifndef PL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT     4
#endif
#ifndef PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2   5
#endif
#ifndef PL_DEFAULT_SHADER_ATTRIB_LOCATION_INDICES
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_INDICES     6
#endif
#ifdef PL_SUPPORT_MESH_GPU_SKINNING
#ifndef PL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEIDS
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEIDS     7
#endif
#ifndef PL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS 8
#endif
#endif
#ifndef PL_DEFAULT_SHADER_ATTRIB_LOCATION_INSTANCE_TX
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_INSTANCE_TX 9
#endif


#if defined(PL_SUPPORT_MESH_GPU_SKINNING)
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEIDS     7
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS 8
#endif
#define PL_DEFAULT_SHADER_ATTRIB_LOCATION_INSTANCE_TX 9


// Default shader vertex attribute names to set location points
#ifndef PL_DEFAULT_SHADER_ATTRIB_NAME_POSITION
#define PL_DEFAULT_SHADER_ATTRIB_NAME_POSITION     "vertexPosition"    // Bound by default to shader location: PL__DEFAULT_SHADER_ATTRIB_NAME_POSITION
#endif	   
#ifndef PL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD
#define PL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD     "vertexTexCoord"    // Bound by default to shader location: PL__DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD
#endif	   
#ifndef PL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL
#define PL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL       "vertexNormal"      // Bound by default to shader location: PL__DEFAULT_SHADER_ATTRIB_NAME_NORMAL
#endif	   
#ifndef PL_DEFAULT_SHADER_ATTRIB_NAME_COLOR
#define PL_DEFAULT_SHADER_ATTRIB_NAME_COLOR        "vertexColor"       // Bound by default to shader location: PL__DEFAULT_SHADER_ATTRIB_NAME_COLOR
#endif	   
#ifndef PL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT
#define PL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT      "vertexTangent"     // Bound by default to shader location: PL__DEFAULT_SHADER_ATTRIB_NAME_TANGENT
#endif	   
#ifndef PL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2
#define PL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2    "vertexTexCoord2"   // Bound by default to shader location: PL__DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2
#endif	   
#ifndef PL_DEFAULT_SHADER_ATTRIB_NAME_BONEIDS
#define PL_DEFAULT_SHADER_ATTRIB_NAME_BONEIDS      "vertexBoneIds"     // Bound by default to shader location: PL__DEFAULT_SHADER_ATTRIB_NAME_BONEIDS
#endif	   
#ifndef PL_DEFAULT_SHADER_ATTRIB_NAME_BONEWEIGHTS
#define PL_DEFAULT_SHADER_ATTRIB_NAME_BONEWEIGHTS  "vertexBoneWeights" // Bound by default to shader location: PL__DEFAULT_SHADER_ATTRIB_NAME_BONEWEIGHTS
#endif	   
#ifndef PL_DEFAULT_SHADER_ATTRIB_NAME_INSTANCE_TX
#define PL_DEFAULT_SHADER_ATTRIB_NAME_INSTANCE_TX  "instanceTransform" // Bound by default to shader location: PL__DEFAULT_SHADER_ATTRIB_NAME_INSTANCE_TX
#endif	   
		   
#ifndef PL_DEFAULT_SHADER_UNIFORM_NAME_MVP
#define PL_DEFAULT_SHADER_UNIFORM_NAME_MVP         "mvp"               // model-view-projection matrix
#endif	   
#ifndef PL_DEFAULT_SHADER_UNIFORM_NAME_VIEW
#define PL_DEFAULT_SHADER_UNIFORM_NAME_VIEW        "matView"           // view matrix
#endif	   
#ifndef PL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION
#define PL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION  "matProjection"     // projection matrix
#endif	   
#ifndef PL_DEFAULT_SHADER_UNIFORM_NAME_MODEL
#define PL_DEFAULT_SHADER_UNIFORM_NAME_MODEL       "matModel"          // model matrix
#endif	   
#ifndef PL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL
#define PL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL      "matNormal"         // normal matrix (transpose(inverse(matModelView))
#endif	   
#ifndef PL_DEFAULT_SHADER_UNIFORM_NAME_COLOR
#define PL_DEFAULT_SHADER_UNIFORM_NAME_COLOR       "colDiffuse"        // color diffuse (base tint color, multiplied by texture color)
#endif	   
#ifndef PL_DEFAULT_SHADER_UNIFORM_NAME_BONE_MATRICES
#define PL_DEFAULT_SHADER_UNIFORM_NAME_BONE_MATRICES  "boneMatrices"   // bone matrices
#endif	   
#ifndef PL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0
#define PL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0  "texture0"          // texture0 (texture slot active 0)
#endif	   
#ifndef PL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1
#define PL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1  "texture1"          // texture1 (texture slot active 1)
#endif	   
#ifndef PL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2
#define PL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2  "texture2"          // texture2 (texture slot active 2)
#endif	   
