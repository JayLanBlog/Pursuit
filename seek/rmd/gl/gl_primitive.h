#pragma once
#include "gl_define.h"
#include <glad/glad.h>
#include <vector>

static const GLenum ELEMENTTYPE[] =
{
	GL_INT,
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE
};

namespace DRAW {
	namespace GL {
		class VBO {
		public:
			void* mData = nullptr;
			GLenum target = GL_TRIANGLES;
			int dataSize = 0;
			int dataCount = 0;
			unsigned int vaoId = 0;
			unsigned int vboId = 0;
			bool drawIntance = false;
			VBO(GLenum vTarget = GL_TRIANGLES): target(vTarget) {}
			void Generate() {
				glGenVertexArrays(1, &vaoId);
				glGenBuffers(1, &vboId);
				glBindVertexArray(vaoId);
				glBindBuffer(GL_ARRAY_BUFFER, vaoId);
				glBufferData(GL_ARRAY_BUFFER, dataSize * dataCount, mData, GL_STATIC_DRAW);
			}
			void upload_data(void* data, int size, int count) {
				dataSize = size;
				dataCount = count;
				mData = data;
			}
			void Bind() { glBindVertexArray(vaoId); }
			void Draw() { 
				glBindVertexArray(vaoId); 
				glDrawArrays(target, 0, dataCount);
			}
			void DrawInstance(int iteamCount) {
				glBindVertexArray(vaoId);
				glDrawArraysInstanced(target, 0, dataCount, iteamCount); 
			}
		};
		class IBO {
		public:
			void* mData = nullptr;
			int dataSize = 0;
			int dataCount = 0;
			IBO(GLenum iTarget = GL_TRIANGLES):target(iTarget) {}
			void Bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboId); }
			void upload_data(void* data, int size, int count) {
				dataSize = size;
				dataCount = count;
				mData = data;
			}
			void Draw() {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
				glDrawElements(target, static_cast<unsigned int>(dataCount), GL_UNSIGNED_INT, 0);
			}
			void DrawInstance(int count) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
				glDrawElementsInstanced(target, static_cast<unsigned int>(dataCount), GL_UNSIGNED_INT, 0, count);
			}
			void Generate() {
				//	glBindVertexArray(VAO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize * dataCount, mData, GL_STATIC_DRAW);
			}
			GLenum target = GL_TRIANGLES;
			unsigned int eboId = 0;
		};

		class Primitive {
		public:
			Primitive() {vbo = new VBO();}

			Primitive(VBO* iVbo, IBO* iIbo = nullptr) {vbo = iVbo;ibo = iIbo;}
			~Primitive() {if (ibo) {delete ibo;}if (vbo) {delete vbo;}}
			void Generate() { 
				vbo->Generate(); 
				if (ibo)
					ibo->Generate();
				int id = 0;
				for (int i = 0; i < elements.size(); i++) {
					GLenum elementType = ELEMENTTYPE[static_cast<int>(elements[i].mType)];
					int eCount = elements[i].element_count;
					elements[i].mType != ElementType::TYPE_INT ?
					glVertexAttribPointer(id, eCount, elementType, elements[i].mType == ElementType::TYPE_UBYTE4_NORM ? GL_TRUE : GL_FALSE, elements[i].mStrade, (void*)elements[i].offset) :
					glVertexAttribIPointer(id, eCount, elementType, elements[i].mStrade, (void*)elements[i].offset);
					glEnableVertexAttribArray(id);
					id++;
				}
				// auto a =	GL_COLOR_ATTACHMENT0 + 1;
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
				// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
				glBindVertexArray(0);
			}
			void Draw() {
				ibo ? ibo->Draw() : vbo->Draw();
			}
			void DrawInstance(int count = 1) {
				ibo ? ibo->DrawInstance(count) : vbo->DrawInstance(count);
			}
			void EnableVBO() { ibo = new IBO(); }
			std::vector<PrimitiveElement> elements;
			VBO* vbo = nullptr;
			IBO* ibo = nullptr;
		};
		void UploadVBOToPrimitive(Primitive& primitve,void* data, int size, int count);
		void UploadEBOToPrimitive(Primitive& primitve, void* data, int size, int count);
		void AddElemntToPrimitive(Primitive& primitve, PrimitiveElement& element);
	}
}
