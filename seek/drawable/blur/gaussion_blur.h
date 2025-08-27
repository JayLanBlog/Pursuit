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

namespace BLUR {

	class GaussionBlur {
	public:
		
		GaussionBlur() {}

		~GaussionBlur(){}

		void LoadScene(string uri, int width = 1920, int height = 1080) {
			camera.position = { 2.5f, 2.5f, 3.0f };    // Camera position
			camera.target = { 0.0f, 0.0f, 0.7f };      // Camera looking at point
			camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 65.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
			screenHeight = height;
			screenWidth = width;
			shader = LoadShader(0,uri.c_str());
			
			//SetShaderValue(shader, texture0loc, &srcImage, SHADER_UNIFORM_SAMPLER2D);
		}

		void LoadSceneAndTexture(string uri, string img,int width = 1920, int height = 1080) {
			camera.position = { 2.5f, 2.5f, 3.0f };    // Camera position
			camera.target = { 0.0f, 0.0f, 0.7f };      // Camera looking at point
			camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 65.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
			screenHeight = height;
			screenWidth = width;
			srcImage = LoadTexture(img.c_str());
			//DRAW::GL::LoadTexture(img);
			shader = LoadShader(0, uri.c_str());
			blurSizeLoc= GetShaderLocation(shader, "bulrSize");
			SetShaderValue(shader, blurSizeLoc,&blurSize, SHADER_ATTRIB_FLOAT);
			//uniform sampler2D texture0; 
			//uniform vec4 colDiffuse;
		}


		void Tick() {
		
		}

		void Render() {
			BeginShaderMode(shader);
			//DrawRectanglePro (rect, { 0.0f,0.0f }, 0.0f, RED);
			//DrawTextureEx
			//DrawTextureEx(srcImage, { (float)((screenWidth - srcImage.width) / 40), (float)((screenHeight - srcImage.height) / 4.0 )}, 0.0f, 1.5f, DARKGRAY);
			DrawTextureEx(srcImage, { (float)((screenWidth - srcImage.width) * 0.1),(float)((screenWidth - srcImage.height) * 0.1) }, 0.0f, 1.3, DARKGRAY);

			//DrawTexture(srcImage, ((screenWidth - srcImage.width) /2.0), ( (screenHeight - srcImage.height)/2.0), DARKGRAY);
			EndShaderMode();
		}

		float blurSize = 2.;
		Rectangle rect;
		Shader shader;
		// Initialization
		//--------------------------------------------------------------------------------------
		int screenWidth = 1920;
		int screenHeight = 1080;
		Camera camera = { 0 };
		int blurSizeLoc;
		
		Texture2D srcImage;
	};


}