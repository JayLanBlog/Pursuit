#pragma once
#include <view/3D/model/modelv.h>
#include <vector>

using namespace ModelView;

namespace GEMO {

	struct TorusParam
	{
		float radius = 1.0f;  // 大环半径
		float tube = 0.4f;  // 小管半径
		int tubularSegments = 64;    // 沿管方向分段
		int radialSegments = 8;     // 管截面分段
		int p = 2;     // 纽结 p 参数
		int q = 3;     // 纽结 q 参数
		float heightScale = 1.0f;
	};

	
	Mesh GenMeshKnot(float radius, float size, int radSeg, int sides);

	Mesh GenMeshCustom(void);

	Mesh generate(const TorusParam& params);
}