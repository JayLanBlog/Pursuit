#include "draw_pass.h"
#include <rmd/gl/gl_include.h>
#include <core/platform/wcore_sdl.h>
#include <math/pr_math.h>
#include <core/core.h>
#include <menu/console.h>
#include <core/gui/draw_ui.h>
#include <section/enum/camera_em.h>
#include <core/text/draw_text.h>
#include <file/sys_text.h>
using namespace PiplineState;
using namespace PMath;
using namespace DRAW::GL;
using namespace Text;
using namespace System;


// Load and draw a cube in NDC
void LoadDrawCube(void) {
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;

    float vertices[] = {
        // Positions          Normals               Texcoords
       -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
        1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
        1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
       -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
       -1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
       -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
        1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
        1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
       -1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
       -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
       -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
       -1.0f,  1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
       -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
       -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
       -1.0f, -1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        1.0f,  1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        1.0f, -1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
        1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
        1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
       -1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
       -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
       -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
        1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
        1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
        1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
       -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
       -1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f
    };

    // Gen VAO to contain VBO
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    // Gen and fill vertex buffer (VBO)
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind vertex attributes (position, normals, texcoords)
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(PL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION);
    glVertexAttribPointer(PL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Positions
    glEnableVertexAttribArray(PL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL);
    glVertexAttribPointer(PL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normals
    glEnableVertexAttribArray(PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD);
    glVertexAttribPointer(PL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texcoords
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Draw cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Delete VBO and VAO
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
#endif
}

void EndTextureMode(void) {

    DrawRenderBatchActive();      // Update and draw internal render batch
    DisableFramebuffer();         // Disable render target (fbo)
    // Set viewport to default framebuffer size
    SetupViewport(CORE.Window.render.width, CORE.Window.render.height);
    // Go back to the modelview state from BeginDrawing since we are back to the default FBO
    MatrixMode(PL_MODELVIEW);     // Switch back to modelview matrix
    LoadIdentity();               // Reset current matrix (modelview)
    MultMatrixf(MatrixToFloatV(CORE.Window.screenScale).v); // Apply screen scaling if required
    // Reset current fbo to screen size
    CORE.Window.currentFbo.width = CORE.Window.render.width;
    CORE.Window.currentFbo.height = CORE.Window.render.height;
    CORE.Window.usingFbo = false;
}

void BeginTextureMode(RenderTexture2D target) {
    DrawRenderBatchActive();      // Update and draw internal render batch

    EnableFramebuffer(target.id); // Enable render target

    // Set viewport and RLGL internal framebuffer size
    Viewport(0, 0, target.texture.width, target.texture.height);
    SetFramebufferWidth(target.texture.width);
    SetFramebufferHeight(target.texture.height);

    MatrixMode(PL_PROJECTION);    // Switch to projection matrix
    LoadIdentity();               // Reset current matrix (projection)

    // Set orthographic projection to current framebuffer size
    // NOTE: Configured top-left corner as (0, 0)
    Ortho(0, target.texture.width, target.texture.height, 0, 0.0f, 1.0f);

    MatrixMode(PL_MODELVIEW);     // Switch back to modelview matrix
    LoadIdentity();               // Reset current matrix (modelview)

    //rlScalef(0.0f, -1.0f, 0.0f);  // Flip Y-drawing (?)

    // Setup current width/height for proper aspect ratio
    // calculation when using BeginTextureMode()
    CORE.Window.currentFbo.width = target.texture.width;
    CORE.Window.currentFbo.height = target.texture.height;
    CORE.Window.usingFbo = true;
}

void BeginDrawing(void) {
    // WARNING: Previously to BeginDrawing() other render textures drawing could happen,
  // consequently the measure for update vs draw is not accurate (only the total frame time is accurate)

    CORE.Time.current = GetTime();      // Number of elapsed seconds since InitTimer()
    CORE.Time.update = CORE.Time.current - CORE.Time.previous;
    CORE.Time.previous = CORE.Time.current;

    LoadIdentity();                   // Reset current matrix (modelview)
    MultMatrixf(MatrixToFloatV(CORE.Window.screenScale).v); // Apply screen scaling
}
void ClearBackground(Color color) {
    ClearColor(color.r, color.g, color.b, color.a);   // Set clear color
    ClearScreenBuffers();
}


#include <logger/logger.h>
#include <core/text/draw_shape.h>

// End canvas drawing and swap buffers (double buffering)
void EndDrawing(void) {
    DrawRenderBatchActive();      // Update and draw internal render batch
#if defined(SUPPORT_GIF_RECORDING)
// Draw record indicator
    if (gifRecording)
    {
#ifndef GIF_RECORD_FRAMERATE
#define GIF_RECORD_FRAMERATE    10
#endif
        gifFrameCounter += (unsigned int)(GetFrameTime() * 1000);

        // NOTE: We record one gif frame depending on the desired gif framerate
        if (gifFrameCounter > 1000 / GIF_RECORD_FRAMERATE)
        {
            // Get image data for the current frame (from backbuffer)
            // NOTE: This process is quite slow... :(
            Vector2 scale = GetWindowScaleDPI();
            unsigned char* screenData = ReadScreenPixels((int)((float)CORE.Window.render.width * scale.x), (int)((float)CORE.Window.render.height * scale.y));

#ifndef GIF_RECORD_BITRATE
#define GIF_RECORD_BITRATE 16
#endif

            // Add the frame to the gif recording, given how many frames have passed in centiseconds
            msf_gif_frame(&gifState, screenData, gifFrameCounter / 10, GIF_RECORD_BITRATE, (int)((float)CORE.Window.render.width * scale.x) * 4);
            gifFrameCounter -= 1000 / GIF_RECORD_FRAMERATE;

            FREE(screenData);    // Free image data
        }

#if defined(SUPPORT_MODULE_RSHAPES) && defined(SUPPORT_MODULE_RTEXT)
        // Display the recording indicator every half-second
        if ((int)(GetTime() / 0.5) % 2 == 1)
        {
            DrawCircle(30, CORE.Window.screen.height - 20, 10, MAROON);                 // WARNING: Module required: rshapes
            DrawText("GIF RECORDING", 50, CORE.Window.screen.height - 25, 10, RED);     // WARNING: Module required: rtext
        }
#endif

        DrawRenderBatchActive();  // Update and draw internal render batch
    }
#endif

#if defined(SUPPORT_AUTOMATION_EVENTS)
    if (automationEventRecording) RecordAutomationEvent();    // Event recording
#endif

#if !defined(SUPPORT_CUSTOM_FRAME_CONTROL)

#ifdef SUPPORT_IMGUI
    NewImGUIFrame();
    Console::i().render();
    RenderPanel();
    EndGUIFrame();
    PresentImGUI();
#endif // ENABLE_LOG_TO_CONSOLE
    SwapScreenBuffer();                  // Copy back buffer to front buffer (screen)

    // Frame time control system
    CORE.Time.current = GetTime();
    CORE.Time.draw = CORE.Time.current - CORE.Time.previous;
    CORE.Time.previous = CORE.Time.current;

    CORE.Time.frame = CORE.Time.update + CORE.Time.draw;

    // Wait for some milliseconds...
    if (CORE.Time.frame < CORE.Time.target)
    {
        WaitTime(CORE.Time.target - CORE.Time.frame);

        CORE.Time.current = GetTime();
        double waitTime = CORE.Time.current - CORE.Time.previous;
        CORE.Time.previous = CORE.Time.current;

        CORE.Time.frame += waitTime;    // Total frame time: update + draw + wait
    }
    PollInputEvents();      // Poll user events (before next frame update)
#endif

#if defined(SUPPORT_SCREEN_CAPTURE)
    if (IsKeyPressed(KEY_F12))
    {
#if defined(SUPPORT_GIF_RECORDING)
        if (IsKeyDown(KEY_LEFT_CONTROL))
        {
            if (gifRecording)
            {
                gifRecording = false;

                MsfGifResult result = msf_gif_end(&gifState);

                SaveFileData(TextFormat("%s/screenrec%03i.gif", CORE.Storage.basePath, screenshotCounter), result.data, (unsigned int)result.dataSize);
                msf_gif_free(result);

                TRACELOG(LOG_INFO, "SYSTEM: Finish animated GIF recording");
            }
            else
            {
                gifRecording = true;
                gifFrameCounter = 0;

                Vector2 scale = GetWindowScaleDPI();
                msf_gif_begin(&gifState, (int)((float)CORE.Window.render.width * scale.x), (int)((float)CORE.Window.render.height * scale.y));
                screenshotCounter++;

                TRACELOG(LOG_INFO, "SYSTEM: Start animated GIF recording: %s", TextFormat("screenrec%03i.gif", screenshotCounter));
            }
        }
        else
#endif  // SUPPORT_GIF_RECORDING
        {
             TakeScreenshot(TextFormat("screenshot%03i.png", screenshotCounter));
             screenshotCounter++;
        }
    }
#endif  // SUPPORT_SCREEN_CAPTURE
    CORE.Time.frameCounter++;
}

void BeginMode3D(Camera3D camera) {
    DrawRenderBatchActive();      // Update and draw internal render batch

    MatrixMode(PL_PROJECTION);    // Switch to projection matrix
    PushMatrix();                 // Save previous matrix, which contains the settings for the 2d ortho projection
    LoadIdentity();               // Reset current matrix (projection)
    float aspect = (float)CORE.Window.currentFbo.width / (float)CORE.Window.currentFbo.height;
    // NOTE: zNear and zFar values are important when computing depth buffer values
    if (camera.projection == CAMERA_PERSPECTIVE)
    {
        // Setup perspective projection
        double top = GetCullDistanceNear() * tan(camera.fovy * 0.5 * DEG2RAD);
        double right = top * aspect;

        Frustum(-right, right, -top, top, GetCullDistanceNear(), GetCullDistanceFar());
    }
    else if (camera.projection == CAMERA_ORTHOGRAPHIC)
    {
        // Setup orthographic projection
        double top = camera.fovy / 2.0;
        double right = top * aspect;
        Ortho(-right, right, -top, top, GetCullDistanceNear(), GetCullDistanceFar());
    }
    MatrixMode(PL_MODELVIEW);     // Switch back to modelview matrix
    LoadIdentity();               // Reset current matrix (modelview)
    // Setup Camera view
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);
    MultMatrixf(MatrixToFloatV(matView).v);      // Multiply modelview matrix by view matrix (camera)
    EnableDepthTest();            // Enable DEPTH_TEST for 3D
}

void EndMode3D(void) {
    DrawRenderBatchActive();      // Update and draw internal render batch
    MatrixMode(PL_PROJECTION);    // Switch to projection matrix
    PopMatrix();                  // Restore previous matrix (projection) from matrix stack
    MatrixMode(PL_MODELVIEW);     // Switch back to modelview matrix
    LoadIdentity();               // Reset current matrix (modelview)
    if (GetActiveFramebuffer() == 0) MultMatrixf(MatrixToFloatV(CORE.Window.screenScale).v); // Apply screen scaling if required
    DisableDepthTest();           // Disable DEPTH_TEST for 2D
}

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
// Initialize drawing mode (how to organize vertex)
void Begin(int mode) {
    if (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].mode != mode)
    {
        if (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount > 0)
        {
            // Make sure current PLGL.currentBatch->draws[i].vertexCount is aligned a multiple of 4,
           // that way, following QUADS drawing will keep aligned with index processing
           // It implies adding some extra alignment vertex at the end of the draw,
           // those vertex are not processed but they are considered as an additional offset
           // for the next set of vertex to be drawn
            if (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].mode == PL_LINES) PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexAlignment = ((PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount < 4) ? PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount : PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount % 4);
            else if (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].mode == PL_TRIANGLES) PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexAlignment = ((PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount < 4) ? 1 : (4 - (PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount % 4)));
            else PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexAlignment = 0;

            if (!CheckRenderBatchLimit(PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexAlignment))
            {
                PLGL.State.vertexCounter += PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexAlignment;
                PLGL.currentBatch->drawCounter++;
            }
        }

        if (PLGL.currentBatch->drawCounter >= PL_DEFAULT_BATCH_DRAWCALLS) DrawRenderBatch(PLGL.currentBatch);
        PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].mode = mode;
        PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].vertexCount = 0;
        PLGL.currentBatch->draws[PLGL.currentBatch->drawCounter - 1].textureId = PLGL.State.defaultTextureId;
    }
}

// Finish vertex providing
void End(void)
{
    // NOTE: Depth increment is dependant on rlOrtho(): z-near and z-far values,
    // as well as depth buffer bit-depth (16bit or 24bit or 32bit)
    // Correct increment formula would be: depthInc = (zfar - znear)/pow(2, bits)
    PLGL.currentBatch->currentDepth += (1.0f / 20000.0f);
}

#endif

// Begin custom shader mode
void BeginShaderMode(Shader shader)
{
    SetShader(shader.id, shader.locs);
}


// End custom shader mode (returns to default shader)
void EndShaderMode(void)
{
    SetShader(GetShaderIdDefault(), GetShaderLocsDefault());
}

void DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Color tint) {
    Rectangle dest = { position.x, position.y, fabsf(source.width), fabsf(source.height) };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(texture, source, dest, origin, 0.0f, tint);
}

// Draw a texture
void DrawTexture(Texture2D texture, int posX, int posY, Color tint) {

    DrawTextureEx(texture, { (float)posX, (float)posY }, 0.0f, 1.0f, tint);
}

void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint) {
    Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(texture, source, dest, origin, rotation, tint);
}


void DrawTextureFx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint) {
    Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
    Vector2 origin = { 0.0f, 0.0f };

   DrawTextureToFrameBuffer(texture, source, dest, origin, rotation, tint);
}



RenderTexture2D LoadRenderTextureDepthTex(int wd, int ht) {
    RenderTexture2D target = { 0 };

    target.id = LoadFramebuffer(); // Load an empty framebuffer

    if (target.id > 0)
    {
        EnableFramebuffer(target.id);

        // Create color texture (default to RGBA)
        target.texture.id = LoadTexture(0, wd, ht, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
        target.texture.width = wd;
        target.texture.height = ht;
        target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texture.mipmaps = 1;

        // Create depth texture buffer (instead of raylib default renderbuffer)
        target.depth.id = LoadTextureDepth(wd, ht, false);
        target.depth.width = wd;
        target.depth.height = ht;
        target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
        target.depth.mipmaps = 1;

        // Attach color texture and depth texture to FBO
        FramebufferAttach(target.id, target.texture.id, PL_ATTACHMENT_COLOR_CHANNEL0, PL_ATTACHMENT_TEXTURE2D, 0);
        FramebufferAttach(target.id, target.depth.id, PL_ATTACHMENT_DEPTH, PL_ATTACHMENT_TEXTURE2D, 0);

        // Check if fbo is complete with attachments (valid)
        if (FramebufferComplete(target.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

        DisableFramebuffer();
    }
    else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

    return target;
}
