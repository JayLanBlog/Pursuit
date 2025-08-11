#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <section/enum/config_em.h>
#include <file/sys_text.h>


using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace Loader;
using namespace Text;

namespace MarkView {
	class PBRView {
	public:
		int lightCount = 0;
		int metallicValueLoc = -1;
		int roughnessValueLoc = -1;
		int emissiveIntensityLoc = -1;
		int emissiveColorLoc = -1;
		int textureTilingLoc = -1;

		// Setup material texture maps usage in shader
		// NOTE: By default, the texture maps are always used
		int usage = 1;
		PBRView() {}
		~PBRView() {}
		// Define the camera to look into our 3d world
		Camera camera = { 0 };

		void EnbleConfig() {
			SetConfigFlags(FLAG_MSAA_4X_HINT);
		}

		void LoadPBR() {
			CreateCamera();
			CreateShaderAndLoadProp();
			LoadModelInToWorld();

		}

		void CreateCamera() {
			camera.position = { 2.0f, 2.0f, 6.0f };    // Camera position
			camera.target = { 0.0f, 0.5f, 0.0f };      // Camera looking at point
			camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
			camera.fovy = 45.0f;                                // Camera field-of-view Y
			camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
		}

		void CreateShaderAndLoadProp() {
			shader = LoadShader("resource/res/pr/shaders/pbr.vs","resource/res/pr/shaders/pbr.fs");
			shader.locs[SHADER_LOC_MAP_ALBEDO] = GetShaderLocation(shader, "albedoMap");
			// WARNING: Metalness, roughness, and ambient occlusion are all packed into a MRA texture
			// They are passed as to the SHADER_LOC_MAP_METALNESS location for convenience,
			// shader already takes care of it accordingly
			shader.locs[SHADER_LOC_MAP_METALNESS] = GetShaderLocation(shader, "mraMap");
			shader.locs[SHADER_LOC_MAP_NORMAL] = GetShaderLocation(shader, "normalMap");
			// WARNING: Similar to the MRA map, the emissive map packs different information 
			// into a single texture: it stores height and emission data
			// It is binded to SHADER_LOC_MAP_EMISSION location an properly processed on shader
			shader.locs[SHADER_LOC_MAP_EMISSION] = GetShaderLocation(shader, "emissiveMap");
			shader.locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(shader, "albedoColor");

			// Setup additional required shader locations, including lights data
			shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
			int lightCountLoc = GetShaderLocation(shader, "numOfLights");
			int maxLightCount = MAX_LIGHTS;
			SetShaderValue(shader, lightCountLoc, &maxLightCount, SHADER_UNIFORM_INT);

			Vector3 ambientColorNormalized ={ ambientColor.r / 255.0f, ambientColor.g / 255.0f, ambientColor.b / 255.0f };
			SetShaderValue(shader, GetShaderLocation(shader, "ambientColor"), &ambientColorNormalized, SHADER_UNIFORM_VEC3);
			SetShaderValue(shader, GetShaderLocation(shader, "ambient"), &ambientIntensity, SHADER_UNIFORM_FLOAT);
			// Get location for shader parameters that can be modified in real time
			metallicValueLoc = GetShaderLocation(shader, "metallicValue");
			roughnessValueLoc = GetShaderLocation(shader, "roughnessValue");
			emissiveIntensityLoc = GetShaderLocation(shader, "emissivePower");
			emissiveColorLoc = GetShaderLocation(shader, "emissiveColor");
			textureTilingLoc = GetShaderLocation(shader, "tiling");
		}

		void LoadModelInToWorld() {
			// Load old car model using PBR maps and shader
			// WARNING: We know this model consists of a single model.meshes[0] and
			// that model.materials[0] is by default assigned to that mesh
			// There could be more complex models consisting of multiple meshes and
			// multiple materials defined for those meshes... but always 1 mesh = 1 material
			car = LoadModel("resource/res/pr/old_car_new.glb");
			// Assign already setup PBR shader to model.materials[0], used by models.meshes[0]
			car.materials[0].shader = shader;
			// Setup materials[0].maps default parameters
			car.materials[0].maps[MATERIAL_MAP_ALBEDO].color = WHITE;
			car.materials[0].maps[MATERIAL_MAP_METALNESS].value = 1.0f;
			car.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value = 0.0f;
			car.materials[0].maps[MATERIAL_MAP_OCCLUSION].value = 1.0f;
			car.materials[0].maps[MATERIAL_MAP_EMISSION].color = { 255, 162, 0, 255 };
			// Setup materials[0].maps default textures
			car.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = LoadTexture("resource/res/pr/old_car_d.png");
			car.materials[0].maps[MATERIAL_MAP_METALNESS].texture = LoadTexture("resource/res/pr/old_car_mra.png");
			car.materials[0].maps[MATERIAL_MAP_NORMAL].texture = LoadTexture("resource/res/pr/old_car_n.png");
			car.materials[0].maps[MATERIAL_MAP_EMISSION].texture = LoadTexture("resource/res/pr/old_car_e.png");

			// Load floor model mesh and assign material parameters
			// NOTE: A basic plane shape can be generated instead of being loaded from a model file
			floor = LoadModel("resource/res/pr/plane.glb");
			//Mesh floorMesh = GenMeshPlane(10, 10, 10, 10);
			//GenMeshTangents(&floorMesh);      // TODO: Review tangents generation
			//Model floor = LoadModelFromMesh(floorMesh);


	// Assign material shader for our floor model, same PBR shader 
			floor.materials[0].shader = shader;
			floor.materials[0].maps[MATERIAL_MAP_ALBEDO].color = WHITE;
			floor.materials[0].maps[MATERIAL_MAP_METALNESS].value = 0.8f;
			floor.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value = 0.1f;
			floor.materials[0].maps[MATERIAL_MAP_OCCLUSION].value = 1.0f;
			floor.materials[0].maps[MATERIAL_MAP_EMISSION].color = BLACK;
			floor.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = LoadTexture("resource/res/pr/road_a.png");
			floor.materials[0].maps[MATERIAL_MAP_METALNESS].texture = LoadTexture("resource/res/pr/road_mra.png");
			floor.materials[0].maps[MATERIAL_MAP_NORMAL].texture = LoadTexture("resource/res/pr/road_n.png");
			


			// Models texture tiling parameter can be stored in the Material struct if required (CURRENTLY NOT USED)
	// NOTE: Material.params[4] are available for generic parameters storage (float)
			Vector2 carTextureTiling = { 0.5f, 0.5f };
			Vector2 floorTextureTiling = { 0.5f, 0.5f };

			lights[0] = CreateLight(LIGHT_POINT,  { -1.0f, 1.0f, -2.0f }, { 0.0f, 0.0f, 0.0f }, YELLOW, 4.0f, shader);
			lights[1] = CreateLight(LIGHT_POINT,  { 2.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, GREEN, 3.3f, shader);
			lights[2] = CreateLight(LIGHT_POINT,  { -2.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, RED, 8.3f, shader);
			lights[3] = CreateLight(LIGHT_POINT,  { 1.0f, 1.0f, -2.0f }, { 0.0f, 0.0f, 0.0f }, BLUE, 2.0f, shader);

			SetShaderValue(shader, GetShaderLocation(shader, "useTexAlbedo"), &usage, SHADER_UNIFORM_INT);
			SetShaderValue(shader, GetShaderLocation(shader, "useTexNormal"), &usage, SHADER_UNIFORM_INT);
			SetShaderValue(shader, GetShaderLocation(shader, "useTexMRA"), &usage, SHADER_UNIFORM_INT);
			SetShaderValue(shader, GetShaderLocation(shader, "useTexEmissive"), &usage, SHADER_UNIFORM_INT);
		}

		Light CreateLight(int type, Vector3 position, Vector3 target, Color color, float intensity, Shader shader) {
			Light light = { 0 };

			if (lightCount < MAX_LIGHTS)
			{
				light.enabled = 1;
				light.type = type;
				light.position = position;
				light.target = target;
				light.color[0] = (float)color.r / 255.0f;
				light.color[1] = (float)color.g / 255.0f;
				light.color[2] = (float)color.b / 255.0f;
				light.color[3] = (float)color.a / 255.0f;
				light.intensity = intensity;

				// NOTE: Shader parameters names for lights must match the requested ones
				light.enabledLoc = GetShaderLocation(shader, TextFormat("lights[%i].enabled", lightCount));
				light.typeLoc = GetShaderLocation(shader, TextFormat("lights[%i].type", lightCount));
				light.positionLoc = GetShaderLocation(shader, TextFormat("lights[%i].position", lightCount));
				light.targetLoc = GetShaderLocation(shader, TextFormat("lights[%i].target", lightCount));
				light.colorLoc = GetShaderLocation(shader, TextFormat("lights[%i].color", lightCount));
				light.intensityLoc = GetShaderLocation(shader, TextFormat("lights[%i].intensity", lightCount));

				UpdateLight(shader, light);

				lightCount++;
			}

			return light;
		}

		void UpdateLight(Shader shader, Light light) {
			SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
			SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);
			// Send to shader light position values
			float position[3] = { light.position.x, light.position.y, light.position.z };
			SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);
			// Send to shader light target position values
			float target[3] = { light.target.x, light.target.y, light.target.z };
			SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);
			SetShaderValue(shader, light.colorLoc, light.color, SHADER_UNIFORM_VEC4);
			SetShaderValue(shader, light.intensityLoc, &light.intensity, SHADER_UNIFORM_FLOAT);

		}

		void Update() {
			//----------------------------------------------------------------------------------
			UpdateCamera(&camera, CAMERA_ORBITAL);
			// Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
			float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
			SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
			// Update light values on shader (actually, only enable/disable them)
			for (int i = 0; i < MAX_LIGHTS; i++) UpdateLight(shader, lights[i]);

		}

		void Draw() {
			
			BeginMode3D(camera);


			// Set floor model texture tiling and emissive color parameters on shader
			SetShaderValue(shader, textureTilingLoc, &floorTextureTiling, SHADER_UNIFORM_VEC2);
			Vector4 floorEmissiveColor = ColorNormalize(floor.materials[0].maps[MATERIAL_MAP_EMISSION].color);
			SetShaderValue(shader, emissiveColorLoc, &floorEmissiveColor, SHADER_UNIFORM_VEC4);

			// Set floor metallic and roughness values
			SetShaderValue(shader, metallicValueLoc, &floor.materials[0].maps[MATERIAL_MAP_METALNESS].value, SHADER_UNIFORM_FLOAT);
			SetShaderValue(shader, roughnessValueLoc, &floor.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value, SHADER_UNIFORM_FLOAT);

			DrawModel(floor,  { 0.0f, 0.0f, 0.0f }, 5.0f, WHITE);   // Draw floor model

			// Set old car model texture tiling, emissive color and emissive intensity parameters on shader
			SetShaderValue(shader, textureTilingLoc, &carTextureTiling, SHADER_UNIFORM_VEC2);
			Vector4 carEmissiveColor = ColorNormalize(car.materials[0].maps[MATERIAL_MAP_EMISSION].color);
			SetShaderValue(shader, emissiveColorLoc, &carEmissiveColor, SHADER_UNIFORM_VEC4);
			float emissiveIntensity = 0.01f;
			SetShaderValue(shader, emissiveIntensityLoc, &emissiveIntensity, SHADER_UNIFORM_FLOAT);

			// Set old car metallic and roughness values
			SetShaderValue(shader, metallicValueLoc, &car.materials[0].maps[MATERIAL_MAP_METALNESS].value, SHADER_UNIFORM_FLOAT);
			SetShaderValue(shader, roughnessValueLoc, &car.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value, SHADER_UNIFORM_FLOAT);

			DrawModel(car, { 0.0f, 0.0f, 0.0f }, 0.25f, WHITE);   // Draw car model

			// Draw spheres to show the lights positions
			for (int i = 0; i < MAX_LIGHTS; i++)
			{
				Color lightColor = { lights[i].color[0] * 255, lights[i].color[1] * 255, lights[i].color[2] * 255, lights[i].color[3] * 255 };

				if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lightColor);
				else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lightColor, 0.3f));
			}

			EndMode3D();
		}
		// Models texture tiling parameter can be stored in the Material struct if required (CURRENTLY NOT USED)
// NOTE: Material.params[4] are available for generic parameters storage (float)
		Vector2 carTextureTiling = { 0.5f, 0.5f };
		Vector2 floorTextureTiling = { 0.5f, 0.5f };
		// Create some lights
		Light lights[MAX_LIGHTS] = { 0 };
		// Setup ambient color and intensity parameters
		float ambientIntensity = 0.02f;
		Color ambientColor = { 26, 32, 135, 255 };
		Model car;
		Model floor;
		// Load PBR shader and setup all required locations
		Shader shader;



		
	};


}