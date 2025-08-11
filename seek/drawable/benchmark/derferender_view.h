#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <math/pr_math.h>
#include <core/text/draw_shape.h>
#include <core/text/draw_text.h>

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace PMath;
using namespace PiplineState;
// GBuffer data
typedef struct GBuffer {
	unsigned int framebuffer;

	unsigned int positionTexture;
	unsigned int normalTexture;
	unsigned int albedoSpecTexture;

	unsigned int depthRenderbuffer;
} GBuffer;
// Deferred mode passes
typedef enum {
	DEFERRED_POSITION,
	DEFERRED_NORMAL,
	DEFERRED_ALBEDO,
	DEFERRED_SHADING
} DeferredMode;


namespace MarkView {
	class DeferedRenderView {
	public:

		void LoaderScene(float screenWidth , float screenHeight) {
			width = screenWidth;
			height = screenHeight;
			camera.position = { 5.0f, 4.0f, 5.0f };    // Camera position
			camera.target =	{ 0.0f, 1.0f, 0.0f };      // Camera looking at point
			camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 60.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
			model = LoadModelFromMesh(GenMeshPlane(10.0f, 10.0f, 3, 3));
			cube = LoadModelFromMesh(GenMeshCube(2.0f, 2.0f, 2.0f));

			gbufferShader = LoadShader("resource/res/pr/shaders/gbuffer.vs",
				"resource/res/pr/shaders/gbuffer.fs");

			deferredShader = LoadShader("resource/res/pr/shaders/deferred_shading.vs",
				"resource/res/pr/shaders/deferred_shading.fs");
			deferredShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(deferredShader, "viewPosition");

			gBuffer.framebuffer = LoadFramebuffer();
			if (!gBuffer.framebuffer)
			{
				TraceLog(LOG_WARNING, "Failed to create framebuffer");
				exit(1);
			}
			EnableFramebuffer(gBuffer.framebuffer);
			gBuffer.positionTexture = LoadTexture(NULL, width, height, PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
			gBuffer.normalTexture = LoadTexture(NULL, width, height, PL_PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
			gBuffer.albedoSpecTexture = LoadTexture(NULL, width, height,PL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
			// Activate the draw buffers for our framebuffer
			ActiveDrawBuffers(3);
			// Now we attach our textures to the framebuffer.
			FramebufferAttach(gBuffer.framebuffer, gBuffer.positionTexture, PL_ATTACHMENT_COLOR_CHANNEL0, PL_ATTACHMENT_TEXTURE2D, 0);
			FramebufferAttach(gBuffer.framebuffer, gBuffer.normalTexture, PL_ATTACHMENT_COLOR_CHANNEL1, PL_ATTACHMENT_TEXTURE2D, 0);
			FramebufferAttach(gBuffer.framebuffer, gBuffer.albedoSpecTexture, PL_ATTACHMENT_COLOR_CHANNEL2, PL_ATTACHMENT_TEXTURE2D, 0);
			// Finally we attach the depth buffer.
			gBuffer.depthRenderbuffer = LoadTextureDepth(screenWidth, screenHeight, true);
			FramebufferAttach(gBuffer.framebuffer, gBuffer.depthRenderbuffer, PL_ATTACHMENT_DEPTH, PL_ATTACHMENT_RENDERBUFFER, 0);
			// Make sure our framebuffer is complete.
			// NOTE: rlFramebufferComplete() automatically unbinds the framebuffer, so we don't have
			// to rlDisableFramebuffer() here.
			if (!FramebufferComplete(gBuffer.framebuffer))
			{
				TraceLog(LOG_WARNING, "Framebuffer is not complete");
			}

			// Now we initialize the sampler2D uniform's in the deferred shader.
			// We do this by setting the uniform's values to the texture units that
			// we later bind our g-buffer textures to.
			EnableShader(deferredShader.id);
		
			SetShaderValue(deferredShader, GetLocationUniform(deferredShader.id, "gPosition"), &texUnitPosition, PL_SHADER_UNIFORM_SAMPLER2D);
			SetShaderValue(deferredShader, GetLocationUniform(deferredShader.id, "gNormal"), &texUnitNormal, PL_SHADER_UNIFORM_SAMPLER2D);
			SetShaderValue(deferredShader, GetLocationUniform(deferredShader.id, "gAlbedoSpec"), &texUnitAlbedoSpec, PL_SHADER_UNIFORM_SAMPLER2D);
			DisableShader();

			// Assign out lighting shader to model
			model.materials[0].shader = gbufferShader;
			cube.materials[0].shader = gbufferShader;
			
			// Create lights
		   //--------------------------------------------------------------------------------------			
			lights[0] = CreateLight(LIGHT_POINT, { -2, 1, -2 }, Vector3Zero(), YELLOW, deferredShader);
			lights[1] = CreateLight(LIGHT_POINT, { 2, 1, 2 }, Vector3Zero(), RED, deferredShader);
			lights[2] = CreateLight(LIGHT_POINT, { -2, 1, 2 }, Vector3Zero(), GREEN, deferredShader);
			lights[3] = CreateLight(LIGHT_POINT, { 2, 1, -2 }, Vector3Zero(), BLUE, deferredShader);

			for (int i = 0; i < MAX_CUBES; i++)
			{
				cubePositions[i] = {
					(float)(rand() % 10) - 5,
					(float)(rand() % 5),
					(float)(rand() % 10) - 5,
				};
				cubeRotations[i] = (float)(rand() % 360);
			}	
			EnableDepthTest();
		}

		void Tick() {
			// Update
	   //----------------------------------------------------------------------------------
			UpdateCamera(&camera, CAMERA_ORBITAL);

			// Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
			float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
			SetShaderValue(deferredShader, deferredShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

			// Check key inputs to enable/disable lights
			if (IsKeyPressed(KEY_Y)) { lights[0].enabled = !lights[0].enabled; }
			if (IsKeyPressed(KEY_R)) { lights[1].enabled = !lights[1].enabled; }
			if (IsKeyPressed(KEY_G)) { lights[2].enabled = !lights[2].enabled; }
			if (IsKeyPressed(KEY_B)) { lights[3].enabled = !lights[3].enabled; }

			// Check key inputs to switch between G-buffer textures
			if (IsKeyPressed(KEY_ONE)) mode = DEFERRED_POSITION;
			if (IsKeyPressed(KEY_TWO)) mode = DEFERRED_NORMAL;
			if (IsKeyPressed(KEY_THREE)) mode = DEFERRED_ALBEDO;
			if (IsKeyPressed(KEY_FOUR)) mode = DEFERRED_SHADING;
			//CORE.Input.Mouse.currentPosition
		//	TRACELOG(LOG_INFO,"current mouse : x  %f, y: %f", CORE.Input.Mouse.currentPosition.x, CORE.Input.Mouse.currentPosition.y);
			// Update light values (actually, only enable/disable them)
			for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(deferredShader, lights[i]);
		}

		void Render() {
			// Draw to the geometry buffer by first activating it
			EnableFramebuffer(gBuffer.framebuffer);
			ClearColor(0, 0, 0, 0);
			ClearScreenBuffers();  // Clear color and depth buffer
			DisableColorBlend();
			BeginMode3D(camera);
			// NOTE: We have to use rlEnableShader here. `BeginShaderMode` or thus `rlSetShader`
			// will not work, as they won't immediately load the shader program.
			EnableShader(gbufferShader.id);
			// When drawing a model here, make sure that the material's shaders
			// are set to the gbuffer shader!
			DrawModel(model, Vector3Zero(), 1.0f, WHITE);
			DrawModel(cube,  { 0.0, 1.0f, 0.0 }, 1.0f, WHITE);

			for (int i = 0; i < MAX_CUBES; i++)
			{
				Vector3 position = cubePositions[i];
				DrawModelEx(cube, position, { 1, 1, 1 }, cubeRotations[i],  { CUBE_SCALE, CUBE_SCALE, CUBE_SCALE }, WHITE);
			}

			DisableShader();
			EndMode3D();

			EnableColorBlend();

			// Go back to the default framebuffer (0) and draw our deferred shading.
			DisableFramebuffer();
			ClearScreenBuffers(); // Clear color & depth buffer
			switch (mode)
			{
				case DEFERRED_SHADING: {
					BeginMode3D(camera);
					DisableColorBlend();
					EnableShader(deferredShader.id);
					// Bind our g-buffer textures
					// We are binding them to locations that we earlier set in sampler2D uniforms `gPosition`, `gNormal`,
					// and `gAlbedoSpec`
					ActiveTextureSlot(texUnitPosition);
					EnableTexture(gBuffer.positionTexture);
					ActiveTextureSlot(texUnitNormal);
					EnableTexture(gBuffer.normalTexture);
					ActiveTextureSlot(texUnitAlbedoSpec);
					EnableTexture(gBuffer.albedoSpecTexture);

					// Finally, we draw a fullscreen quad to our default framebuffer
					// This will now be shaded using our deferred shader
					LoadDrawQuad();
					DisableShader();
					EnableColorBlend();
					EndMode3D();

					// As a last step, we now copy over the depth buffer from our g-buffer to the default framebuffer.
					BindFramebuffer(PL_READ_FRAMEBUFFER, gBuffer.framebuffer);
					BindFramebuffer(PL_DRAW_FRAMEBUFFER, 0);
					BlitFramebuffer(0, 0, width, height, 0, 0, width, height, 0x00000100);    // GL_DEPTH_BUFFER_BIT
					DisableFramebuffer();

					// Since our shader is now done and disabled, we can draw spheres
					// that represent light positions in default forward rendering
					BeginMode3D(camera);
					EnableShader(GetShaderIdDefault());
					for (int i = 0; i < MAX_LIGHTS; i++)
					{
						if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
						else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
					}
					DisableShader();
					EndMode3D();
					DrawText("FINAL RESULT", 10,height - 30, 20, DARKGREEN);
				}break;
				case DEFERRED_POSITION:
				{
					Texture2D texture = {
						gBuffer.positionTexture,
						width,
						height,
					};
					DrawTextureRec(texture,  { 0, 0, (float)width, (float)-height }, Vector2Zero(), RAYWHITE);
					DrawText("POSITION TEXTURE", 10, height - 30, 20, DARKGREEN);
				}break;
				case DEFERRED_NORMAL:
				{
					Texture2D texture = {
						gBuffer.normalTexture,
						width,
						height,
					};
					DrawTextureRec(texture, { 0, 0, (float)width, (float)-height }, Vector2Zero(), RAYWHITE);
					DrawText("NORMAL TEXTURE", 10, height - 30, 20, DARKGREEN);
				} break;
				case DEFERRED_ALBEDO:
				{
					Texture2D texture = {
						gBuffer.albedoSpecTexture,
						width,
						height,
					};
					DrawTextureRec(texture,  { 0, 0, (float)width, (float)-height }, Vector2Zero(), RAYWHITE);
					DrawText("ALBEDO TEXTURE", 10, height - 30, 20, DARKGREEN);
				} break;
				default: break;
			}
			DrawText("Toggle lights keys: [Y][R][G][B]", 10, 40, 20, DARKGRAY);
			DrawText("Switch G-buffer textures: [1][2][3][4]", 10, 70, 20, DARKGRAY);

		}

		void Unload() {

			//UnloadModel(model);     // Unload the models
			//UnloadModel(cube);

			UnloadShader(deferredShader); // Unload shaders
			UnloadShader(gbufferShader);

			// Unload geometry buffer and all attached textures
			UnloadFramebuffer(gBuffer.framebuffer);
			UnloadTexture(gBuffer.positionTexture);
			UnloadTexture(gBuffer.normalTexture);
			UnloadTexture(gBuffer.albedoSpecTexture);
			UnloadTexture(gBuffer.depthRenderbuffer);

		
		}

		int texUnitPosition = 0;
		int texUnitNormal = 1;
		int texUnitAlbedoSpec = 2;
		const float CUBE_SCALE = 0.25;
		Vector3 cubePositions[MAX_CUBES] = { 0 };
		float cubeRotations[MAX_CUBES] = { 0 };

		DeferredMode mode = DEFERRED_SHADING;
		MLight lights[MAX_LIGHTS] = { 0 };
		float width, height;
		// Initialize the G-buffer
		GBuffer gBuffer = { 0 };
		Shader gbufferShader;
		Shader deferredShader;
		Model cube;
		Model model;
		Camera camera = { 0 };
	};
}
