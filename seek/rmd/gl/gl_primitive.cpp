#include "gl_primitive.h"
namespace DRAW {
	namespace GL {
		void UploadVBOToPrimitive(Primitive& primitve, void* data, int size, int count) {
			VBO * vbo  = new VBO();
			vbo->upload_data(data,size,count);
			primitve.vbo = vbo;
		}
		void UploadEBOToPrimitive(Primitive& primitve, void* data, int size, int count) {
			IBO* ibo = new IBO();
			ibo->upload_data(data, size, count);
			primitve.ibo = ibo;
		}
		void AddElemntToPrimitive(Primitive& primitve, PrimitiveElement& element) {
			primitve.elements.push_back(element);
		}
	}
}