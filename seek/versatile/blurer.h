#pragma once
#include <rmd/gl/gl_include.h>
#include <core/core.h>
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <core/pass/draw_pass2d.h>

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;

namespace Versatile {
	/*
		{
			shader 
			shader param
			render
		}
	*/
	struct Gassin
	{
		float blursize;
	};

	struct BlurBox
	{
		float blurKernelSize = 3.0;
		float texelWidthOffset;
		float texelHeightOffset;
		float blurDirect = 0.0;
	};
	
	Texture2D GaussionBlur(Texture src,Gassin assin);

	//Texture2D KawaseBlur(Gassin assin);

	Texture2D RaidalBlur(Texture src);

	Texture2D GrainyBlur(Texture src);

	Texture2D IrisBlur(Texture src);

	Texture2D RadialBlur(Texture src);

	Texture2D TiltShiftBlur(Texture src);

	Texture2D DiectionalBlur(Texture src);

	Texture2D BoxBlur(Texture src, BlurBox blurBox);

	Texture2D RadialBlur(Texture src);
}