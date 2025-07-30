#include "gl_global.h"
#include "gl_vertex.h"
#include <logger/logger.h>

namespace DRAW {
	namespace GL {
		
		// Vertex data management
		//-----------------------------------------------------------------------------------------
		// Load a new attributes buffer
		unsigned int LoadVertexBuffer(const void* buffer, int size, bool dynamic) {
			unsigned int id = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glGenBuffers(1, &id);
			glBindBuffer(GL_ARRAY_BUFFER, id);
			glBufferData(GL_ARRAY_BUFFER, size, buffer, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
#endif

			return id;
		}

		// Load a new attributes element buffer
		unsigned int LoadVertexBufferElement(const void* buffer, int size, bool dynamic) {
			unsigned int id = 0;

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glGenBuffers(1, &id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, buffer, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
#endif

			return id;
		}

		// Enable vertex buffer (VBO)
		void EnableVertexBuffer(unsigned int id) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glBindBuffer(GL_ARRAY_BUFFER, id);
#endif
		}

		// Disable vertex buffer (VBO)
		void DisableVertexBuffer(void) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
		}

		// Enable vertex buffer element (VBO element)
		void EnableVertexBufferElement(unsigned int id) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
#endif
		}

		// Disable vertex buffer element (VBO element)
		void DisableVertexBufferElement(void) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
		}

		// Update vertex buffer with new data
		// NOTE: dataSize and offset must be provided in bytes
		void UpdateVertexBuffer(unsigned int id, const void* data, int dataSize, int offset) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glBindBuffer(GL_ARRAY_BUFFER, id);
			glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, data);
#endif
		}
		// Update vertex buffer elements with new data
		// NOTE: dataSize and offset must be provided in bytes
		void UpdateVertexBufferElements(unsigned int id, const void* data, int dataSize, int offset) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, dataSize, data);
#endif
		}

		// Enable vertex array object (VAO)
		bool EnableVertexArray(unsigned int vaoId) {
			bool result = false;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			if (PLGL.ExtSupported.vao)
			{
				glBindVertexArray(vaoId);
				result = true;
			}
#endif
			return result;
		}

		// Disable vertex array object (VAO)
		void DisableVertexArray(void) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			if (PLGL.ExtSupported.vao) glBindVertexArray(0);
#endif
		}

		// Enable vertex attribute index
		void EnableVertexAttribute(unsigned int index) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glEnableVertexAttribArray(index);
#endif
		}

		// Disable vertex attribute index
		void DisableVertexAttribute(unsigned int index) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glDisableVertexAttribArray(index);
#endif
		}

		// Draw vertex array
		void DrawVertexArray(int offset, int count) {
			glDrawArrays(GL_TRIANGLES, offset, count);
		}

		// Draw vertex array elements
		void DrawVertexArrayElements(int offset, int count, const void* buffer) {
			// NOTE: Added pointer math separately from function to avoid UBSAN complaining
			unsigned short* bufferPtr = (unsigned short*)buffer;
			if (offset > 0) bufferPtr += offset;

			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, (const unsigned short*)bufferPtr);
		}

		// Draw vertex array instanced
		void DrawVertexArrayInstanced(int offset, int count, int instances) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glDrawArraysInstanced(GL_TRIANGLES, offset, count, instances);
#endif
		}
		// Draw vertex array elements instanced
		void DrawVertexArrayElementsInstanced(int offset, int count, const void* buffer, int instances) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			// NOTE: Added pointer math separately from function to avoid UBSAN complaining
			unsigned short* bufferPtr = (unsigned short*)buffer;
			if (offset > 0) bufferPtr += offset;

			glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, (const unsigned short*)bufferPtr, instances);
#endif
		}
		// Load vertex array object (VAO)
		unsigned int LoadVertexArray(void) {
			unsigned int vaoId = 0;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			if (PLGL.ExtSupported.vao)
			{
				glGenVertexArrays(1, &vaoId);
			}
#endif
			return vaoId;
		}
		// Set vertex attribute
		void SetVertexAttribute(unsigned int index, int compSize, int type, bool normalized, int stride, int offset) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			// NOTE: Data type could be: GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT
			// Additional types (depends on OpenGL version or extensions):
			//  - GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE, GL_FIXED,
			//  - GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV, GL_UNSIGNED_INT_10F_11F_11F_REV

			size_t offsetNative = offset;
			glVertexAttribPointer(index, compSize, type, normalized, stride, (void*)offsetNative);
#endif
		}

		// Set vertex attribute divisor
		void SetVertexAttributeDivisor(unsigned int index, int divisor) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glVertexAttribDivisor(index, divisor);
#endif
		}

		// Unload vertex array object (VAO)
		void UnloadVertexArray(unsigned int vaoId) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			if (PLGL.ExtSupported.vao)
			{
				glBindVertexArray(0);
				glDeleteVertexArrays(1, &vaoId);
				TRACELOG(LOG_INFO, "VAO: [ID %i] Unloaded vertex array data from VRAM (GPU)", vaoId);
			}
#endif
		}

		// Unload vertex buffer (VBO)
		void UnloadVertexBuffer(unsigned int vboId) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			glDeleteBuffers(1, &vboId);
			//TRACELOG(RL_LOG_INFO, "VBO: Unloaded vertex data from VRAM (GPU)");
#endif
		}

		void SetVertexAttributeDefault(int locIndex, const void* value, int attribType, int count) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
			switch (attribType)
			{
			case PL_SHADER_ATTRIB_FLOAT: if (count == 1) glVertexAttrib1fv(locIndex, (float*)value); break;
			case PL_SHADER_ATTRIB_VEC2: if (count == 2) glVertexAttrib2fv(locIndex, (float*)value); break;
			case PL_SHADER_ATTRIB_VEC3: if (count == 3) glVertexAttrib3fv(locIndex, (float*)value); break;
			case PL_SHADER_ATTRIB_VEC4: if (count == 4) glVertexAttrib4fv(locIndex, (float*)value); break;
			default: TRACELOG(LOG_WARNING, "SHADER: Failed to set attrib default value, data type not recognized");
			}
#endif
		}
	
	}
}