#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <gemo/TorusKnotGeometry.h>

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace GEMO;

namespace Acr {
	class SkyBox {
	public:
		const string BasePath = "E:/gl/resource/res/water/";
		const string RootPath = "E:/gl/resource/";
     
		SkyBox() {}
		~SkyBox() {}

        std::vector<string> faces
        {
                BasePath + "gems1/sky/right.jpg",
                BasePath + "gems1/sky/left.jpg",
                BasePath + "gems1/sky/top.jpg",
                BasePath + "gems1/sky/bottom.jpg",
                BasePath + "gems1/sky/front.jpg",
                BasePath + "gems1/sky/back.jpg",
        };

        std::vector<string> faces2
        {
                BasePath + "gems1/ely_hills/hills_rt.tga",
                BasePath + "gems1/ely_hills/hills_lf.tga",
                BasePath + "gems1/ely_hills/hills_up.tga",
                BasePath + "gems1/ely_hills/hills_dn.tga",
                BasePath + "gems1/ely_hills/hills_ft.tga",
                BasePath + "gems1/ely_hills/hills_bk.tga",
        };


        std::vector<string> faces3
        {
                BasePath + "gems1/cubeMap2/posx.png",
                BasePath + "gems1/cubeMap2/negx.png",
                BasePath + "gems1/cubeMap2/posy.png",
                BasePath + "gems1/cubeMap2/negy.png",
                BasePath + "gems1/cubeMap2/posz.png",
                BasePath + "gems1/cubeMap2/negz.png",
        };


        unsigned int loadCubemap(vector<std::string> faces)
        {
            unsigned int textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            int w, h, nrChannels;
            for (unsigned int i = 0; i < faces.size(); i++)
            {
                unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &nrChannels, 0);
                if (data)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                    stbi_image_free(data);
                }
                else
                {
                    std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                    stbi_image_free(data);
                }
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            return textureID;
        }
        

        void LoadScene(int width, int height) {
            // Define the camera to look into our 3d world
            camera = { 0 };
            camera.position ={ 1.0f, 1.0f, 1.0f };    // Camera position
            camera.target = { 0.0f, 45.0f,0.0f };      // Camera looking at point
            camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
            camera.fovy = 45.0f;                                // Camera field-of-view Y
            camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
             // Camera projection type
            screenWidth = width;
            screenHeight = height;
            cube = GenMeshCube(1.0f, 1.0f, 1.0f);
 
            skybox = LoadModelFromMesh(cube);
            string skyVsPath = RootPath + "shader/water/gems/skybox.vs";
            string skyPsPath = RootPath + "shader/water/gems/skybox.fs";

            skybox.materials[0].shader = LoadShader(skyVsPath.c_str(),skyPsPath.c_str());
            int cubMapV = MATERIAL_MAP_CUBEMAP;
            int doGama = 0;
            int doVflipped = 0;
            SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "environmentMap"),  &cubMapV, SHADER_UNIFORM_INT);
            SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "doGamma"), &doGama, SHADER_UNIFORM_INT);
            SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "vflipped"), &doVflipped, SHADER_UNIFORM_INT);
            unsigned int cubemapTexture = loadCubemap(faces3);
            Texture cubText = { cubemapTexture,screenWidth,screenHeight,1};
            skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubText;
        }

        void Tick() {
            UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        }

        void Render() {
            BeginMode3D(camera);
            // We are inside the cube, we need to disable backface culling!
            DisableBackfaceCulling();
            DisableDepthMask();
            DrawModel(skybox, { 0, 0, 0 }, 1.0f, WHITE);
            EnableBackfaceCulling();
            EnableDepthMask();
            EndMode3D();
        }



        TorusParam tParam;
        Mesh cube;
        
        Model skybox;
        Camera camera = { 0 };
        int screenWidth;
        int screenHeight;

	};
}