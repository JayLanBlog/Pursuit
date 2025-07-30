#pragma once
#include <rmd/gl/gl_program.h>
#include <rmd/gl/gl_primitive.h>
using namespace DRAW::GL;

namespace Drawable {
	class Render {
	public:
		Shader shader = {0};
		void  LoadShaderByPath(std::string vs, std::string fs) {
			shader = std::move(DRAW::GL::LoadShader(vs.c_str(), fs.c_str()));
		}
		virtual void MakeGeometry() {}
		virtual void Draw() {}
		virtual void Destroy(){}
	};

	class PrimitiveRender : public Render {
	public:
		int scrWidth = 2000, scrHeight = 1280;
		std::string vertexUri = "resource/shader/hello.vs";
		std::string piexclUri = "resource/shader/hello.fs";
		PrimitiveRender() {
			
		}
		void LoadPritiveShader() {
			LoadShaderByPath(vertexUri, piexclUri);
		}

		virtual void MakeGeometry() {
			float vertices[] = {
					-0.5f, -0.5f, 0.0f, // left  
					0.5f, -0.5f, 0.0f, // right 
					0.0f,  0.5f, 0.0f  // top   
			};
			PrimitiveElement* element = new PrimitiveElement();
			element->element_count = 3;
			element->mType = ElementType::TYPE_VECTOR3;
			element->sematic = ElementSemantic::SEM_POSITION;
			element->mStrade = sizeof(float) * 3;
			UploadVBOToPrimitive(primitive,vertices, sizeof(float) * 3, 3);
			AddElemntToPrimitive(primitive,*element);
			primitive.Generate();
		}
		virtual void Draw() {
			glViewport(0, 0, scrWidth, scrHeight);
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			DRAW::GL::EnableShader(shader.id);
			primitive.Draw();
		}

		Primitive primitive;
	};
}