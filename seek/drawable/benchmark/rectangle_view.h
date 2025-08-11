#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <section/enum/config_em.h>
#include <file/sys_text.h>
#include <core/pass/draw_pass.h>
#include <core/pass/draw_pass2d.h>
#include <core/text/draw_text.h>
// Rounded rectangle data
typedef struct {
    Vector4 cornerRadius; // Individual corner radius (top-left, top-right, bottom-left, bottom-right)

    // Shadow variables
    float shadowRadius;
    Vector2 shadowOffset;
    float shadowScale;

    // Border variables
    float borderThickness; // Inner-border thickness

    // Shader locations
    int rectangleLoc;
    int radiusLoc;
    int colorLoc;
    int shadowRadiusLoc;
    int shadowOffsetLoc;
    int shadowScaleLoc;
    int shadowColorLoc;
    int borderThicknessLoc;
    int borderColorLoc;
} RoundedRectangle;


using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace Loader;
using namespace Text;


namespace MarkView {
    class RoundedRectView {
    public:
        RoundedRectView() {}
        ~RoundedRectView() {}


        // Update rounded rectangle uniforms
        void UpdateRoundedRectangle(RoundedRectangle rec, Shader shader) {
            float temp[] = { rec.cornerRadius.x, rec.cornerRadius.y, rec.cornerRadius.z, rec.cornerRadius.w };
            SetShaderValue(shader, rec.radiusLoc, temp, SHADER_UNIFORM_VEC4);
            SetShaderValue(shader, rec.shadowRadiusLoc, &rec.shadowRadius, SHADER_UNIFORM_FLOAT);
            float tempShadow[] = { rec.shadowOffset.x, rec.shadowOffset.y };
            SetShaderValue(shader, rec.shadowOffsetLoc, tempShadow, SHADER_UNIFORM_VEC2);
            SetShaderValue(shader, rec.shadowScaleLoc, &rec.shadowScale, SHADER_UNIFORM_FLOAT);
            SetShaderValue(shader, rec.borderThicknessLoc, &rec.borderThickness, SHADER_UNIFORM_FLOAT);
        }


        RoundedRectangle CreateRoundedRectangle(Vector4 cornerRadius, float shadowRadius, Vector2 shadowOffset, float shadowScale, float borderThickness, Shader shader) {
            RoundedRectangle rec;
            rec.cornerRadius = cornerRadius;
            rec.shadowRadius = shadowRadius;
            rec.shadowOffset = shadowOffset;
            rec.shadowScale = shadowScale;
            rec.borderThickness = borderThickness;
            // Get shader uniform locations
            rec.rectangleLoc = GetShaderLocation(shader, "rectangle");
            rec.radiusLoc = GetShaderLocation(shader, "radius");
            rec.colorLoc = GetShaderLocation(shader, "color");
            rec.shadowRadiusLoc = GetShaderLocation(shader, "shadowRadius");
            rec.shadowOffsetLoc = GetShaderLocation(shader, "shadowOffset");
            rec.shadowScaleLoc = GetShaderLocation(shader, "shadowScale");
            rec.shadowColorLoc = GetShaderLocation(shader, "shadowColor");
            rec.borderThicknessLoc = GetShaderLocation(shader, "borderThickness");
            rec.borderColorLoc = GetShaderLocation(shader, "borderColor");
    
            return rec;
        }

        void CreateRectange() {
            shader = LoadShader("resource/res/pr/shaders/base.vs", "resource/res/pr/shaders/rounded_rectangle.fs");
            roundedRectangle = CreateRoundedRectangle({ 125.0f, 125.0f,125.0f, 120.0f},     // Corner radius
                60.0f,                                      // Shadow radius
                { 0.0f, -5.0f },                   // Shadow offset
                 0.95f,                                      // Shadow scale
                  15.0f,                                       // Border thickness
                  shader                                      // Shader
                 );

            UpdateRoundedRectangle(roundedRectangle, shader);
        }

        void Tick() {
            // Draw rectangle box with rounded corners using shader
          //  Rectangle rec = { 50, 70, 110, 60 };

        }

        void Draw(const int screenWidth, const int screenHeight) {
            // Draw rectangle box with rounded corners using shader
            Rectangle rec = { 50, 70, 110, 60 };
            DrawRectangleLines((int)rec.x - 20, (int)rec.y - 20, (int)rec.width + 40, (int)rec.height + 40, DARKGRAY);
            DrawText("Rounded rectangle", (int)rec.x - 20, (int)rec.y - 35, 10, DARKGRAY);
            // Flip Y axis to match shader coordinate system
            rec.y = screenHeight - rec.y - rec.height;
            float recData[] = {rec.x, rec.y, rec.width, rec.height};
            SetShaderValue(shader, roundedRectangle.rectangleLoc, recData, SHADER_UNIFORM_VEC4);

            // Only rectangle color
            float colorData[] ={ rectangleColor.r / 255.0f, rectangleColor.g / 255.0f, rectangleColor.b / 255.0f, rectangleColor.a / 255.0f };
            SetShaderValue(shader, roundedRectangle.colorLoc, colorData, SHADER_UNIFORM_VEC4);
            float color2Data[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            SetShaderValue(shader, roundedRectangle.shadowColorLoc, color2Data, SHADER_UNIFORM_VEC4);
            float color3Data[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            SetShaderValue(shader, roundedRectangle.borderColorLoc, color3Data, SHADER_UNIFORM_VEC4);

            BeginShaderMode(shader);
              DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
            EndShaderMode();

            // Draw rectangle shadow using shader
            rec = { 50, 200, 110, 60 };

            DrawRectangleLines((int)rec.x - 20, (int)rec.y - 20, (int)rec.width + 40, (int)rec.height + 40, DARKGRAY);
            DrawText("Rounded rectangle shadow", (int)rec.x - 20, (int)rec.y - 35, 10, DARKGRAY);
            rec.y = screenHeight - rec.y - rec.height;

            float t1[] = {rec.x, rec.y, rec.width, rec.height};
            SetShaderValue(shader, roundedRectangle.rectangleLoc,t1 , SHADER_UNIFORM_VEC4);
            float t2[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            float t3[] = { shadowColor.r / 255.0f, shadowColor.g / 255.0f, shadowColor.b / 255.0f, shadowColor.a / 255.0f };
            // Only shadow color
            SetShaderValue(shader, roundedRectangle.colorLoc,t2, SHADER_UNIFORM_VEC4);
            SetShaderValue(shader, roundedRectangle.shadowColorLoc,t3, SHADER_UNIFORM_VEC4);
            SetShaderValue(shader, roundedRectangle.borderColorLoc,t2, SHADER_UNIFORM_VEC4);

            BeginShaderMode(shader);
            DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
            EndShaderMode(); 
            // Draw rectangle's border using shader
          
            rec ={ 50, 330, 110, 60 };
            DrawRectangleLines((int)rec.x - 20, (int)rec.y - 20, (int)rec.width + 40, (int)rec.height + 40, DARKGRAY);
            DrawText("Rounded rectangle border", (int)rec.x - 20, (int)rec.y - 35, 10, DARKGRAY);
          
            rec.y = screenHeight - rec.y - rec.height;
            float t11[] = { rec.x, rec.y, rec.width, rec.height };
            SetShaderValue(shader, roundedRectangle.rectangleLoc, t11, SHADER_UNIFORM_VEC4);

            float tepC[] = {0.0f, 0.0f, 0.0f, 0.0f};
            float tepS[] ={ 0.0f, 0.0f, 0.0f, 0.0f };
            float t12[] = { borderColor.r / 255.0f, borderColor.g / 255.0f, borderColor.b / 255.0f, borderColor.a / 255.0f };
            // Only border color
            SetShaderValue(shader, roundedRectangle.colorLoc, tepC, SHADER_UNIFORM_VEC4);
            SetShaderValue(shader, roundedRectangle.shadowColorLoc, tepS, SHADER_UNIFORM_VEC4);
            SetShaderValue(shader, roundedRectangle.borderColorLoc, t12, SHADER_UNIFORM_VEC4);
            BeginShaderMode(shader);
            DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
            EndShaderMode();

            // Draw one more rectangle with all three colors
            float ratio = 2.0;
            float boderRatio = 1.0;
            rec = { 240 * ratio, 80 * ratio, 500 * ratio, 300 * ratio };
          //  DrawRectangleLines((int)rec.x - 30 * boderRatio, (int)rec.y - 30 * boderRatio, (int)rec.width + 60 * boderRatio, (int)rec.height + 60 * boderRatio, DARKGRAY);
          //  DrawText("Rectangle with all three combined", (int)rec.x - 30, (int)rec.y - 45, 10, DARKGRAY);

            rec.y = screenHeight - rec.y - rec.height -10;
            float t21[] = { rec.x, rec.y, rec.width, rec.height };
            SetShaderValue(shader, roundedRectangle.rectangleLoc, t21, SHADER_UNIFORM_VEC4);


            float t22[] = { rectangleColor.r / 255.0f, rectangleColor.g / 255.0f, rectangleColor.b / 255.0f, rectangleColor.a / 255.0f };
            float t23[] = { shadowColor.r / 255.0f, shadowColor.g / 255.0f, shadowColor.b / 255.0f, shadowColor.a / 255.0f };
            float t33[] = { borderColor.r / 255.0f, borderColor.g / 255.0f, borderColor.b / 255.0f, borderColor.a / 255.0f } ;

            // All three colors
            SetShaderValue(shader, roundedRectangle.colorLoc, t22, SHADER_UNIFORM_VEC4);
            SetShaderValue(shader, roundedRectangle.shadowColorLoc, t23, SHADER_UNIFORM_VEC4);
            SetShaderValue(shader, roundedRectangle.borderColorLoc, t33, SHADER_UNIFORM_VEC4);
            BeginShaderMode(shader);
            DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
            EndShaderMode();

        }
         Color rectangleColor = BLUE;
         Color shadowColor = DARKBLUE;
         Color borderColor = SKYBLUE;
        // Create a rounded rectangle
        RoundedRectangle roundedRectangle;
        // Load the shader
        Shader shader;
    };

}