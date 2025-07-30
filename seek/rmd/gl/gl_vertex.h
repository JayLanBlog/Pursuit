#pragma once
#include <section/martrix/cg_def.h>
#include <section/enum/material_em.h>
using namespace Seek;

namespace DRAW {

	namespace GL {


		// Vertex data management
		//-----------------------------------------------------------------------------------------
		// Load a new attributes buffer
		unsigned int LoadVertexBuffer(const void* buffer, int size, bool dynamic);

		// Load a new attributes element buffer
		unsigned int LoadVertexBufferElement(const void* buffer, int size, bool dynamic);

		// Enable vertex buffer (VBO)
		void EnableVertexBuffer(unsigned int id);

		// Disable vertex buffer (VBO)
		void DisableVertexBuffer(void);

		// Enable vertex buffer element (VBO element)
		void EnableVertexBufferElement(unsigned int id);

		// Disable vertex buffer element (VBO element)
		void DisableVertexBufferElement(void);

		// Update vertex buffer with new data
		// NOTE: dataSize and offset must be provided in bytes
		void UpdateVertexBuffer(unsigned int id, const void* data, int dataSize, int offset);
		// Update vertex buffer elements with new data
		// NOTE: dataSize and offset must be provided in bytes
		void UpdateVertexBufferElements(unsigned int id, const void* data, int dataSize, int offset);

		// Enable vertex array object (VAO)
		bool EnableVertexArray(unsigned int vaoId);

		// Disable vertex array object (VAO)
		void DisableVertexArray(void);

		// Enable vertex attribute index
		void EnableVertexAttribute(unsigned int index);

		// Disable vertex attribute index
		void DisableVertexAttribute(unsigned int index);

		// Draw vertex array
		void DrawVertexArray(int offset, int count);

		// Draw vertex array elements
		void DrawVertexArrayElements(int offset, int count, const void* buffer);

		// Draw vertex array instanced
		void DrawVertexArrayInstanced(int offset, int count, int instances);
		// Draw vertex array elements instanced
		void DrawVertexArrayElementsInstanced(int offset, int count, const void* buffer, int instances);
		// Load vertex array object (VAO)
		unsigned int LoadVertexArray(void);
		// Set vertex attribute
		void SetVertexAttribute(unsigned int index, int compSize, int type, bool normalized, int stride, int offset);
		
		// Set vertex attribute divisor
		void SetVertexAttributeDivisor(unsigned int index, int divisor);

		// Unload vertex array object (VAO)
		void UnloadVertexArray(unsigned int vaoId);

		// Unload vertex buffer (VBO)
		void UnloadVertexBuffer(unsigned int vboId);

		void SetVertexAttributeDefault(int locIndex, const void* value, int attribType, int count);

	}
}