#pragma once

namespace DRAW {
	namespace GL {

		enum class ElementSemantic : int
		{
			SEM_POSITION = 0,
			SEM_NORMAL,
			SEM_BINORMAL,
			SEM_TANGENT,
			SEM_TEXCOORD,
			SEM_COLOR,
			SEM_BLENDWEIGHTS,
			SEM_BLENDINDICES,
			SEM_OBJECTINDEX,
			SEM_BONE_ID,
			SEM_BONE_WEIGHT,
			SEM_BIGTANGENT,
			MAX_VERTEX_ELEMENT_SEMANTICS
		};
		enum class ElementType
		{
			TYPE_INT = 0,
			TYPE_FLOAT,
			TYPE_VECTOR2,
			TYPE_VECTOR3,
			TYPE_VECTOR4,
			TYPE_UBYTE4,
			TYPE_UBYTE4_NORM,
			MAX_VERTEX_ELEMENT_TYPES
		};

		struct PrimitiveElement
		{
			int element_count = 0;
			int mStrade = 0;
			int offset = 0;
			ElementSemantic sematic = ElementSemantic::SEM_POSITION;
			ElementType mType = ElementType::TYPE_INT;
		};
	}
}