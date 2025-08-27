#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <math/pr_math.h>

using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace PMath;
namespace MView {
	class GLCubeView {
	public:

        const string BasePath = "E:/gl/resource/res/water/";
        const string RootPath = "E:/gl/resource/";
		GLCubeView() {}
		~GLCubeView() {}

        unsigned int loadCubemap(vector<std::string> faces)
        {
            unsigned int textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            int width, height, nrChannels;
            for (unsigned int i = 0; i < faces.size(); i++)
            {
                unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
                if (data)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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
            camera.position = { 0.0f, 5.0f, 30.0f }; // Camera position
            camera.target = { 0.0f, 10.0f, 0.0f };     // Camera looking at point
            camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
            camera.fovy = 45.0f;                         // Camera field-of-view Y
            camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
            screenWidth = width;
            screenHeight = height;

            string skyVsPath = RootPath + "shader/water/gems/cubemap.vs";
            string skyPsPath = RootPath + "shader/water/gems/cubemap.fs";
            shader = LoadShader(skyVsPath.c_str(), skyPsPath.c_str());

            string vsPath = RootPath + "shader/water/gems/cubesky.vs";
            string fsPath = RootPath + "shader/water/gems/cubesky.fs";
            skyBoxShader = LoadShader(vsPath.c_str(), fsPath.c_str());

            // set up vertex data (and buffer(s)) and configure vertex attributes
            // ------------------------------------------------------------------
            float cubeVertices[] = {
                // positions          // texture Coords
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
                 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
            };
            float skyboxVertices[] = {
                // positions          
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                 1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                 1.0f,  1.0f, -1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                 1.0f, -1.0f,  1.0f
            };
            // cube VAO
            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);
            glBindVertexArray(cubeVAO);
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
          
            glGenVertexArrays(1, &skyboxVAO);
            glGenBuffers(1, &skyboxVBO);
            glBindVertexArray(skyboxVAO);
            glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            
            std::vector<string> faces
            {
                    BasePath + "gems1/sky/right.jpg",
                    BasePath + "gems1/sky/left.jpg",
                    BasePath + "gems1/sky/top.jpg",
                    BasePath + "gems1/sky/bottom.jpg",
                    BasePath + "gems1/sky/front.jpg",
                    BasePath + "gems1/sky/back.jpg",
            };

            string cubePath = BasePath + "container.jpg";
            cubeTexture = LoadTexture(cubePath.c_str());
            cubemapTexture = loadCubemap(faces);
            model = MatrixIdentity();
            int textureIndex = 0;
            SetShaderValue(shader, GetShaderLocation(shader, "texture1"),&textureIndex,SHADER_UNIFORM_UINT);
            SetShaderValue(shader, GetShaderLocation(skyBoxShader, "skybox"),&textureIndex, SHADER_UNIFORM_UINT);
            shaderModelLoc = GetShaderLocation(shader,"model");
            shaderViewLoc = GetShaderLocation(shader, "view");
            shaderProjectLoc = GetShaderLocation(shader, "projection");
            
            skyViewLoc = GetShaderLocation(skyBoxShader, "view");
            skyProjectLoc = GetShaderLocation(skyBoxShader, "projection");
         }

        /* ----------------------------------------------------------
        * 功能：把输入矩阵去掉平移和缩放，只保留旋转
        * 说明： m3/m7/m11/m15 保持不变 (0,0,0,1)
        * ---------------------------------------------------------- */
         Matrix Matrix_RotationOnly(const Matrix* in)
         {
             /* 1. 提取 3x3 部分（旋转+缩放） */
             float r0 = in->m0; float r1 = in->m1; float r2 = in->m2;
             float r3 = in->m4; float r4 = in->m5; float r5 = in->m6;
             float r6 = in->m8; float r7 = in->m9; float r8 = in->m10;

             /* 2. 计算每一列的范数（长度）= 缩放因子 */
             float lenX = sqrtf(r0 * r0 + r1 * r1 + r2 * r2);
             float lenY = sqrtf(r3 * r3 + r4 * r4 + r5 * r5);
             float lenZ = sqrtf(r6 * r6 + r7 * r7 + r8 * r8);

             /* 防除零：如果长度≈0，就当作 1 */
             if (lenX < 1e-6f) lenX = 1.0f;
             if (lenY < 1e-6f) lenY = 1.0f;
             if (lenZ < 1e-6f) lenZ = 1.0f;

             /* 3. 单位化，去掉缩放 */
             Matrix out = {
                 r0 / lenX, r3 / lenY, r6 / lenZ, 0.0f,
                 r1 / lenX, r4 / lenY, r7 / lenZ, 0.0f,
                 r2 / lenX, r5 / lenY, r8 / lenZ, 0.0f,
                 0.0f,      0.0f,      0.0f,      1.0f
             };
             return out;
         }


		void Tick() {
            UpdateCamera(&camera, CAMERA_THIRD_PERSON);
            matView  = MatrixLookAt(camera.position, camera.target, camera.up);
           // glm::lookAt(camera.position, camera.position + camera.target, camera.up);
           //glm::mat4 view = camera.GetViewMatrix();
           //glm::mat4 projection = camera.projection;
            matProj = MatrixIdentity();
            matProj = MatrixPerspective(camera.fovy * DEG2RAD, ((double)screenWidth / (double)screenHeight), GetCullDistanceNear(), GetCullDistanceFar());
		}

		void Render() {
            //SetShaderValueMatrix(shader, shaderModelLoc,model);
            //SetShaderValueMatrix(shader, shaderViewLoc, matView);
            //SetShaderValueMatrix(shader, shaderProjectLoc, matProj);
            //EnableShader(shader.id);
            //// cubes
            //glBindVertexArray(cubeVAO);
            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_2D, cubeTexture.id);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
            //glBindVertexArray(0);
            // 
            glDepthFunc(GL_LEQUAL); 
            SetShaderValueMatrix(skyBoxShader, skyViewLoc, Matrix_RotationOnly(&matView));
            SetShaderValueMatrix(skyBoxShader, skyProjectLoc, matProj);
            EnableShader(skyBoxShader.id);
            // skybox cube
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS); // set depth function back to default
		}

        int shaderModelLoc;
        int shaderViewLoc;
        int shaderProjectLoc;
        int skyViewLoc;
        int skyProjectLoc;

        Matrix model;
        Matrix matProj;
        Matrix matView;
        int screenWidth;
        int screenHeight;
        Shader shader;
        Shader skyBoxShader;

        Texture cubeTexture;
        Camera camera = { 0 };

        unsigned int cubemapTexture;
		// cube VAO
		unsigned int cubeVAO, cubeVBO;
        // skybox VAO
        unsigned int skyboxVAO, skyboxVBO;
	};

}