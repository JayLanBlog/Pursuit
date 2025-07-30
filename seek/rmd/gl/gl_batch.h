#pragma once
#ifndef PL_DEFAULT_BATCH_DRAWCALLS
#define PL_DEFAULT_BATCH_DRAWCALLS             256      // Default number of batch draw calls (by state changes: mode, texture)
#endif
#ifndef PL_DEFAULT_BATCH_MAX_TEXTURE_UNITS
#define PL_DEFAULT_BATCH_MAX_TEXTURE_UNITS       4      // Maximum number of textures units that can be activated on batch drawing (SetShaderValueTexture())
#endif


namespace Batch {


	// Draw call type
// NOTE: Only texture changes register a new draw, other state-change-related elements are not
// used at this moment (vaoId, shaderId, matrices), raylib just forces a batch draw call if any
// of those state-change happens (this is done in core module)
	typedef struct plDrawCall {
		int mode;                   // Drawing mode: LINES, TRIANGLES, QUADS
		int vertexCount;            // Number of vertex of the draw
		int vertexAlignment;        // Number of vertex required for index alignment (LINES, TRIANGLES)
		//unsigned int vaoId;       // Vertex array id to be used on the draw -> Using PLGL.currentBatch->vertexBuffer.vaoId
		//unsigned int shaderId;    // Shader id to be used on the draw -> Using PLGL.currentShaderId
		unsigned int textureId;     // Texture id to be used on the draw -> Use to create new draw call if changes

		//Matrix projection;        // Projection matrix for this draw -> Using PLGL.projection by default
		//Matrix modelview;         // Modelview matrix for this draw -> Using PLGL.modelview by default
	} plDrawCall;

}