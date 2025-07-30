#include "gl_global.h"
#include <helper/mem_help.h>
#include <logger/logger.h>
#include <section/graph/shader_def.h>
#include <section/enum/material_em.h>
#include <math/pr_math.h>
#include <rmd/gl/gl_core.h>
#include <rmd/gl/gl_view.h>

using namespace DRAW::GL;
using namespace PMath;
using namespace RHICore;

namespace Batch {

	plRenderBatch LoadRenderBatch(int numBuffers, int bufferElements) {
		plRenderBatch batch = { 0 };
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		// Initialize CPU (RAM) vertex buffers (position, texcoord, color data and indexes)
  //--------------------------------------------------------------------------------------------
		batch.vertexBuffer = (plVertexBuffer*)MALLOC(numBuffers * sizeof(plVertexBuffer));

		for (int i = 0; i < numBuffers; i++) {
			batch.vertexBuffer[i].elementCount = bufferElements;
			batch.vertexBuffer[i].vertices = (float*)MALLOC(bufferElements * 3 * 4 * sizeof(float));        // 3 float by vertex, 4 vertex by quad
			batch.vertexBuffer[i].texcoords = (float*)MALLOC(bufferElements * 2 * 4 * sizeof(float));       // 2 float by texcoord, 4 texcoord by quad
			batch.vertexBuffer[i].normals = (float*)MALLOC(bufferElements * 3 * 4 * sizeof(float));        // 3 float by vertex, 4 vertex by quad
			batch.vertexBuffer[i].colors = (unsigned char*)MALLOC(bufferElements * 4 * 4 * sizeof(unsigned char));   // 4 float by color, 4 colors by quad
#if defined(GRAPHICS_API_OPENGL_33)
			batch.vertexBuffer[i].indices = (unsigned int*)MALLOC(bufferElements * 6 * sizeof(unsigned int));      // 6 int by quad (indices)
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
			batch.vertexBuffer[i].indices = (unsigned short*)MALLOC(bufferElements * 6 * sizeof(unsigned short));  // 6 int by quad (indices)
#endif
			for (int j = 0; j < (3 * 4 * bufferElements); j++) batch.vertexBuffer[i].vertices[j] = 0.0f;
			for (int j = 0; j < (2 * 4 * bufferElements); j++) batch.vertexBuffer[i].texcoords[j] = 0.0f;
			for (int j = 0; j < (3 * 4 * bufferElements); j++) batch.vertexBuffer[i].normals[j] = 0.0f;
			for (int j = 0; j < (4 * 4 * bufferElements); j++) batch.vertexBuffer[i].colors[j] = 0;

			int k = 0;
			// Indices can be initialized right now
			for (int j = 0; j < (6 * bufferElements); j += 6)
			{
				batch.vertexBuffer[i].indices[j] = 4 * k;
				batch.vertexBuffer[i].indices[j + 1] = 4 * k + 1;
				batch.vertexBuffer[i].indices[j + 2] = 4 * k + 2;
				batch.vertexBuffer[i].indices[j + 3] = 4 * k;
				batch.vertexBuffer[i].indices[j + 4] = 4 * k + 2;
				batch.vertexBuffer[i].indices[j + 5] = 4 * k + 3;

				k++;
			}

			PLGL.State.vertexCounter = 0;
		}
		TRACELOG(LOG_INFO, "PLGL: Render batch vertex buffers loaded successfully in RAM (CPU)");
		//--------------------------------------------------------------------------------------------

		// Upload to GPU (VRAM) vertex data and initialize VAOs/VBOs
		//--------------------------------------------------------------------------------------------
		for (int i = 0; i < numBuffers; i++)
		{
			if (PLGL.ExtSupported.vao)
			{
				// Initialize Quads VAO
				glGenVertexArrays(1, &batch.vertexBuffer[i].vaoId);
				glBindVertexArray(batch.vertexBuffer[i].vaoId);
			}
			// Quads - Vertex buffers binding and attributes enable
			// Vertex position buffer (shader-location = 0)
			glGenBuffers(1, &batch.vertexBuffer[i].vboId[0]);
			glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[0]);
			glBufferData(GL_ARRAY_BUFFER, bufferElements * 3 * 4 * sizeof(float), batch.vertexBuffer[i].vertices, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_POSITION]);
			glVertexAttribPointer(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);

			// Vertex texcoord buffer (shader-location = 1)
			glGenBuffers(1, &batch.vertexBuffer[i].vboId[1]);
			glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[1]);
			glBufferData(GL_ARRAY_BUFFER, bufferElements * 2 * 4 * sizeof(float), batch.vertexBuffer[i].texcoords, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_TEXCOORD01]);
			glVertexAttribPointer(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_TEXCOORD01], 2, GL_FLOAT, 0, 0, 0);



			// Vertex normal buffer (shader-location = 2)
			glGenBuffers(1, &batch.vertexBuffer[i].vboId[2]);
			glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[2]);
			glBufferData(GL_ARRAY_BUFFER, bufferElements * 3 * 4 * sizeof(float), batch.vertexBuffer[i].normals, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_NORMAL]);
			glVertexAttribPointer(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_NORMAL], 3, GL_FLOAT, 0, 0, 0);

			// Vertex color buffer (shader-location = 3)
			glGenBuffers(1, &batch.vertexBuffer[i].vboId[3]);
			glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[3]);
			glBufferData(GL_ARRAY_BUFFER, bufferElements * 4 * 4 * sizeof(unsigned char), batch.vertexBuffer[i].colors, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_COLOR]);
			glVertexAttribPointer(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
			// Fill index buffer
			glGenBuffers(1, &batch.vertexBuffer[i].vboId[4]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.vertexBuffer[i].vboId[4]);
#if defined(GRAPHICS_API_OPENGL_33)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferElements * 6 * sizeof(int), batch.vertexBuffer[i].indices, GL_STATIC_DRAW);
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferElements * 6 * sizeof(short), batch.vertexBuffer[i].indices, GL_STATIC_DRAW);
#endif
		}
		TRACELOG(LOG_INFO, "PLGL: Render batch vertex buffers loaded successfully in VRAM (GPU)");

		// Unbind the current VAO
		if (PLGL.ExtSupported.vao) glBindVertexArray(0);
		//--------------------------------------------------------------------------------------------

		// Init draw calls tracking system
		//--------------------------------------------------------------------------------------------
		batch.draws = (plDrawCall*)MALLOC(PL_DEFAULT_BATCH_DRAWCALLS * sizeof(plDrawCall));

		for (int i = 0; i < PL_DEFAULT_BATCH_DRAWCALLS; i++)
		{
			batch.draws[i].mode = PL_QUADS;
			batch.draws[i].vertexCount = 0;
			batch.draws[i].vertexAlignment = 0;
			//batch.draws[i].vaoId = 0;
			//batch.draws[i].shaderId = 0;
			batch.draws[i].textureId = PLGL.State.defaultTextureId;
			//batch.draws[i].PLGL.State.projection = rlMatrixIdentity();
			//batch.draws[i].PLGL.State.modelview = rlMatrixIdentity();
		}

		batch.bufferCount = numBuffers;    // Record buffer count
		batch.drawCounter = 1;             // Reset draws counter
		batch.currentDepth = -1.0f;         // Reset depth value
		//--------------------------------------------------------------------------------------------

#endif
		return batch;
	}

	void UnloadRenderBatch(plRenderBatch batch) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		// Unbind everything
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Unload all vertex buffers data
		for (int i = 0; i < batch.bufferCount; i++)
		{
			// Unbind VAO attribs data
			if (PLGL.ExtSupported.vao)
			{
				glBindVertexArray(batch.vertexBuffer[i].vaoId);
				glDisableVertexAttribArray(PL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION);
				glDisableVertexAttribArray(PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD);
				glDisableVertexAttribArray(PL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL);
				glDisableVertexAttribArray(PL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR);
				glBindVertexArray(0);
			}
			// Delete VBOs from GPU (VRAM)
			glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[0]);
			glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[1]);
			glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[2]);
			glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[3]);
			glDeleteBuffers(1, &batch.vertexBuffer[i].vboId[4]);
			// Delete VAOs from GPU (VRAM)
			if (PLGL.ExtSupported.vao) glDeleteVertexArrays(1, &batch.vertexBuffer[i].vaoId);
			// Free vertex arrays memory from CPU (RAM)
			FREE(batch.vertexBuffer[i].vertices);
			FREE(batch.vertexBuffer[i].texcoords);
			FREE(batch.vertexBuffer[i].normals);
			FREE(batch.vertexBuffer[i].colors);
			FREE(batch.vertexBuffer[i].indices);
		}
		// Unload arrays
		FREE(batch.vertexBuffer);
		FREE(batch.draws);
#endif
	}
	
	// Draw render batch
	// NOTE: We require a pointer to reset batch and increase current buffer (multi-buffer)
	void DrawRenderBatch(plRenderBatch* batch) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		// Update batch vertex buffers
//------------------------------------------------------------------------------------------------------------
// NOTE: If there is not vertex data, buffers doesn't need to be updated (vertexCount > 0)
// TODO: If no data changed on the CPU arrays --> No need to re-update GPU arrays (use a change detector flag?)
		if (PLGL.State.vertexCounter > 0)
		{
			// Activate elements VAO
			if (PLGL.ExtSupported.vao) glBindVertexArray(batch->vertexBuffer[batch->currentBuffer].vaoId);
			// Vertex positions buffer
			glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, PLGL.State.vertexCounter * 3 * sizeof(float), batch->vertexBuffer[batch->currentBuffer].vertices);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*4*batch->vertexBuffer[batch->currentBuffer].elementCount, batch->vertexBuffer[batch->currentBuffer].vertices, GL_DYNAMIC_DRAW);  // Update all buffer

			// Texture coordinates buffer
			glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, PLGL.State.vertexCounter * 2 * sizeof(float), batch->vertexBuffer[batch->currentBuffer].texcoords);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*4*batch->vertexBuffer[batch->currentBuffer].elementCount, batch->vertexBuffer[batch->currentBuffer].texcoords, GL_DYNAMIC_DRAW); // Update all buffer

			   // Normals buffer
			glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[2]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, PLGL.State.vertexCounter * 3 * sizeof(float), batch->vertexBuffer[batch->currentBuffer].normals);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*4*batch->vertexBuffer[batch->currentBuffer].elementCount, batch->vertexBuffer[batch->currentBuffer].normals, GL_DYNAMIC_DRAW); // Update all buffer

			// Colors buffer
			glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[3]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, PLGL.State.vertexCounter * 4 * sizeof(unsigned char), batch->vertexBuffer[batch->currentBuffer].colors);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*4*batch->vertexBuffer[batch->currentBuffer].elementCount, batch->vertexBuffer[batch->currentBuffer].colors, GL_DYNAMIC_DRAW);    // Update all buffer
			// NOTE: glMapBuffer() causes sync issue
		// If GPU is working with this buffer, glMapBuffer() will wait(stall) until GPU to finish its job
		// To avoid waiting (idle), you can call first glBufferData() with NULL pointer before glMapBuffer()
		// If you do that, the previous data in PBO will be discarded and glMapBuffer() returns a new
		// allocated pointer immediately even if GPU is still working with the previous data

		// Another option: map the buffer object into client's memory
		// Probably this code could be moved somewhere else...
		// batch->vertexBuffer[batch->currentBuffer].vertices = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
		// if (batch->vertexBuffer[batch->currentBuffer].vertices)
		// {
			// Update vertex data
		// }
		// glUnmapBuffer(GL_ARRAY_BUFFER);

		// Unbind the current VAO
			if (PLGL.ExtSupported.vao) glBindVertexArray(0);
		}

		// Draw batch vertex buffers (considering VR stereo if required)
  //------------------------------------------------------------------------------------------------------------
		Matrix matProjection = PLGL.State.projection;
		Matrix matModelView = PLGL.State.modelview;


		int eyeCount = 1;
		if (PLGL.State.stereoRender) eyeCount = 2;

		for (int eye = 0; eye < eyeCount; eye++)
		{
			if (eyeCount == 2)
			{
				// Setup current eye viewport (half screen width)
				Viewport(eye * PLGL.State.framebufferWidth / 2, 0, PLGL.State.framebufferWidth / 2, PLGL.State.framebufferHeight);

				// Set current eye view offset to modelview matrix
				SetMatrixModelview(MatrixMultiply(matModelView, PLGL.State.viewOffsetStereo[eye]));
				// Set current eye projection matrix
				SetMatrixProjection(PLGL.State.projectionStereo[eye]);
			}
			// Draw buffers
			if (PLGL.State.vertexCounter > 0)
			{

				// Set current shader and upload current MVP matrix
				glUseProgram(PLGL.State.currentShaderId);

				// Create modelview-projection matrix and upload to shader
				Matrix matMVP = MatrixMultiply(PLGL.State.modelview, PLGL.State.projection);
			//	MatrixToFloatV(matMVP).v;
				glUniformMatrix4fv(PLGL.State.currentShaderLocs[PL_SHADER_LOC_MATRIX_MVP], 1, false, MatrixToFloatV(matMVP).v);

				if (PLGL.State.currentShaderLocs[PL_SHADER_LOC_MATRIX_PROJECTION] != -1)
				{
					glUniformMatrix4fv(PLGL.State.currentShaderLocs[PL_SHADER_LOC_MATRIX_PROJECTION], 1, false, MatrixToFloatV(PLGL.State.projection).v);
				}

				// WARNING: For the following setup of the view, model, and normal matrices, it is expected that
		   // transformations and rendering occur between rlPushMatrix() and rlPopMatrix()

				if (PLGL.State.currentShaderLocs[PL_SHADER_LOC_MATRIX_VIEW] != -1)
				{
					glUniformMatrix4fv(PLGL.State.currentShaderLocs[PL_SHADER_LOC_MATRIX_VIEW], 1, false, MatrixToFloatV(PLGL.State.modelview).v);
				}

				if (PLGL.State.currentShaderLocs[PL_SHADER_LOC_MATRIX_MODEL] != -1)
				{
					glUniformMatrix4fv(PLGL.State.currentShaderLocs[PL_SHADER_LOC_MATRIX_MODEL], 1, false, MatrixToFloatV(PLGL.State.transform).v);
				}

				if (PLGL.State.currentShaderLocs[PL_SHADER_LOC_MATRIX_NORMAL] != -1)
				{
					glUniformMatrix4fv(PLGL.State.currentShaderLocs[PL_SHADER_LOC_MATRIX_NORMAL], 1, false, MatrixToFloatV(MatrixTranspose(MatrixInvert(PLGL.State.transform))).v);
				}
				if (PLGL.ExtSupported.vao) glBindVertexArray(batch->vertexBuffer[batch->currentBuffer].vaoId);
				else
				{
					// Bind vertex attrib: position (shader-location = 0)
					glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[0]);
					glVertexAttribPointer(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);
					glEnableVertexAttribArray(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_POSITION]);

					// Bind vertex attrib: texcoord (shader-location = 1)
					glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[1]);
					glVertexAttribPointer(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_TEXCOORD01], 2, GL_FLOAT, 0, 0, 0);
					glEnableVertexAttribArray(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_TEXCOORD01]);

					// Bind vertex attrib: normal (shader-location = 2)
					glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[2]);
					glVertexAttribPointer(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_NORMAL], 3, GL_FLOAT, 0, 0, 0);
					glEnableVertexAttribArray(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_NORMAL]);

					// Bind vertex attrib: color (shader-location = 3)
					glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[3]);
					glVertexAttribPointer(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
					glEnableVertexAttribArray(PLGL.State.currentShaderLocs[PL_SHADER_LOC_VERTEX_COLOR]);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[4]);


				}

				// Setup some default shader values
				glUniform4f(PLGL.State.currentShaderLocs[PL_SHADER_LOC_COLOR_DIFFUSE], 1.0f, 1.0f, 1.0f, 1.0f);
				glUniform1i(PLGL.State.currentShaderLocs[PL_SHADER_LOC_MAP_DIFFUSE], 0);  // Active default sampler2D: texture0
				 // Activate additional sampler textures
			// Those additional textures will be common for all draw calls of the batch
				for (int i = 0; i < PL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++)
				{
					if (PLGL.State.activeTextureId[i] > 0)
					{
						glActiveTexture(GL_TEXTURE0 + 1 + i);
						glBindTexture(GL_TEXTURE_2D, PLGL.State.activeTextureId[i]);
					}
				}

				// Activate default sampler2D texture0 (one texture is always active for default batch shader)
		  // NOTE: Batch system accumulates calls by texture0 changes, additional textures are enabled for all the draw calls
				glActiveTexture(GL_TEXTURE0);
				
				for (int i = 0, vertexOffset = 0; i < batch->drawCounter; i++) {
					// Bind current draw call texture, activated as GL_TEXTURE0 and bound to sampler2D texture0 by default
					glBindTexture(GL_TEXTURE_2D, batch->draws[i].textureId);
					if ((batch->draws[i].mode == PL_LINES) || (batch->draws[i].mode == PL_TRIANGLES)) glDrawArrays(batch->draws[i].mode, vertexOffset, batch->draws[i].vertexCount);
					else {

#if defined(GRAPHICS_API_OPENGL_33)
						// We need to define the number of indices to be processed: elementCount*6
						// NOTE: The final parameter tells the GPU the offset in bytes from the
						// start of the index buffer to the location of the first index to process
						glDrawElements(GL_TRIANGLES, batch->draws[i].vertexCount / 4 * 6, GL_UNSIGNED_INT, (GLvoid*)(vertexOffset / 4 * 6 * sizeof(GLuint)));
#endif
#if defined(GRAPHICS_API_OPENGL_ES2)
						glDrawElements(GL_TRIANGLES, batch->draws[i].vertexCount / 4 * 6, GL_UNSIGNED_SHORT, (GLvoid*)(vertexOffset / 4 * 6 * sizeof(GLushort)));
#endif
					}
					vertexOffset += (batch->draws[i].vertexCount + batch->draws[i].vertexAlignment);
				}

				if (!PLGL.ExtSupported.vao)
				{
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				}

				glBindTexture(GL_TEXTURE_2D, 0);    // Unbind textures

			}

			if (PLGL.ExtSupported.vao) glBindVertexArray(0); // Unbind VAO

			glUseProgram(0);    // Unbind shader program

		}

		// Restore viewport to default measures
		if (eyeCount == 2) Viewport(0, 0, PLGL.State.framebufferWidth, PLGL.State.framebufferHeight);
		//------------------------------------------------------------------------------------------------------------

		// Reset batch buffers
		//------------------------------------------------------------------------------------------------------------
		// Reset vertex counter for next frame
		PLGL.State.vertexCounter = 0;

		// Reset depth for next draw
		batch->currentDepth = -1.0f;

		// Restore projection/modelview matrices
		PLGL.State.projection = matProjection;
		PLGL.State.modelview = matModelView;


		// Reset PLGL.currentBatch->draws array
		for (int i = 0; i < PL_DEFAULT_BATCH_DRAWCALLS; i++)
		{
			batch->draws[i].mode = PL_QUADS;
			batch->draws[i].vertexCount = 0;
			batch->draws[i].textureId = PLGL.State.defaultTextureId;
		}

		// Reset active texture units for next batch
		for (int i = 0; i < PL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++) PLGL.State.activeTextureId[i] = 0;

		// Reset draws counter to one draw for the batch
		batch->drawCounter = 1;
		//------------------------------------------------------------------------------------------------------------

		// Change to next buffer in the list (in case of multi-buffering)
		batch->currentBuffer++;
		if (batch->currentBuffer >= batch->bufferCount) batch->currentBuffer = 0;


#endif

	}

	// Set the active render batch for plgl
	void SetRenderBatchActive(plRenderBatch* batch) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		DrawRenderBatch(PLGL.currentBatch);

		if (batch != NULL) PLGL.currentBatch = batch;
		else PLGL.currentBatch = &PLGL.defaultBatch;
#endif
	}

	// Update and draw internal render batch
	void DrawRenderBatchActive(void) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		DrawRenderBatch(PLGL.currentBatch);    // NOTE: Stereo rendering is checked inside
#endif
	}

	// Check internal buffer overflow for a given number of vertex
	// and force a rlRenderBatch draw call if required
	bool CheckRenderBatchLimit(int vCount) {
		bool overflow = false;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		if ((PLGL.State.vertexCounter + vCount) >=
			(PLGL.currentBatch->vertexBuffer[PLGL.currentBatch->currentBuffer].elementCount * 4))
		{
			overflow = true;
			// Store current primitive drawing mode and texture id
			int currentMode = PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].mode;
			int currentTexture = PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].textureId;
			DrawRenderBatch(PLGL.currentBatch);    // NOTE: Stereo rendering is checked inside
			// Restore state of last batch so we can continue adding vertices
			PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].mode = currentMode;
			PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].textureId = currentTexture;
		}
#endif
		return overflow;
	}

}