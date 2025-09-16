#pragma once
#include <rmd/gl/gl_include.h>
#include "core/core.h"
#include <core/pass/draw_pass.h>
#include <logger/logger.h>
#include <section/enum/camera_em.h>
#include <view/3D/model/modelv.h>
#include <core/pcarmera.h>
#include <gemo/TorusKnotGeometry.h>
#include <core/pass/draw_pass2d.h>
#include <core/gestures.h>
#include <core/text/draw_text.h>
#include <file/sys_text.h>
#include <core/text/draw_shape.h>
#include <math/pr_math.h>

using namespace PMath;
using namespace Loader;
using namespace ModelView;
using namespace Seek;
using namespace DRAW::GL;
using namespace System;
using namespace PiplineState;
using namespace GEMO;
using namespace Text;

#define TILE_REQUIRED_CLEAN_TIME        2           // Frames it takes to clean a dirt level
#define TILE_SCORE_BY_CLEANED_LEVEL     100         // Score by cleanied dirt level
#define TILE_REQUIRED_CLEAN_AREA        28*28       // Required are for actually cleaning tile

#define TILE_SIZE               36      // Tile size, it should match texture
#define MAX_TILES_X             32
#define MAX_TILES_Y             17

#define CAT_TARGET_RADIUS       3       // Target proximity radius
#define CAT_DIRT_CELL_RADIUS    2       // Cells around cat for dirt spreading

#define TIME_LIMIT_SECONDS      180     // Time to complete the level in seconds

#define MAX_SCORE_POPUPS        60      // Maximum simultaneous score pop-ups!

//----------------------------------------------------------------------------------
// Module types
//----------------------------------------------------------------------------------
// One dirt tile type
typedef struct {
    Vector2 position;       // Relative to top-left corner
    int level;              // Dirtiness: 0-Clean, 1-2-3-Dirt levels
    int state;              // Current dirtiness state
    int counter;            // Frames counter for cleaning
    //int time;               // Time it takes to make it clean --> Depends on level
    //int score;              // It depends on the dirt level
    bool cleaned;           // If it was cleaned (not clean by default)
} Dirtile;

// Score poping-up type
typedef struct {
    Vector2 position;
    int value;
    float alpha;
    bool enabled;
} ScorePopup;


// Furniture tile set
typedef struct {
    int id;             // Furniture tile id
    int posX;           // Position X on tileset
    int posY;           // Position Y on tileset
    int width;          // Furniture piece width
    int height;         // Furniture piece height
} FurSet;

// Furniture type
typedef struct {
    int furId;          // Tileset id
    int cellX;          // Cell position X
    int cellY;          // Cell position Y
    int state;          // 0-Block, 1-Alpha, 2-Breakable
    int counter;        // Counter in case of break
} Furniture;



namespace Game {
	class GamePlay {
	public:
        int framesCounter = 0;
        int timeLevelSeconds = 0;
        bool levelFinished = false;

        int finishScreen = 0;
        int score = 0;
        int result = -1;
        Font font2 = { 0 };
        Font font = { 0 };

        const Vector2 roomOffset = { 70, 70 };

        Texture2D roomba = { 0 };
        Texture2D cat = { 0 };
        Texture2D dirtiles = { 0 };
        Texture2D furniture = { 0 };

         Vector2 roombaPosition = { 100, 100 };
         Vector2 roombaSpeed = { 4, 4 };
         int roombaTilePosX = 0, roombaTilePosY = 0;

         Vector2 catPosition = { 0, 0 };
         Vector2 catTargetPosition = { 0, 0 };
         Vector2 catSpeed = { 3, 3 };
         int catTilePosX = 0, catTilePosY = 0;
         bool catShouldMove = false;

         Vector2 mousePosition = { 0, 0 };
         int mouseTileX = -1, mouseTileY = -1;


         Dirtile tiles[MAX_TILES_X * MAX_TILES_Y] = { 0 };

         ScorePopup popup[MAX_SCORE_POPUPS] = { 0 };

         FurSet furset[32] = { -1 };
         Furniture furmap[40] = { -1 };
         int furnitureCount = 0;
         int furcolmap[MAX_TILES_X * MAX_TILES_Y] = {
    1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,3,3,1,1,1,0,0,1,1,1,1,1,1,1,1,
    1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,3,3,1,1,1,0,0,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,1,1,1,1,2,2,2,1,1,1,0,0,1,1,1,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,1,1,1,1,2,2,2,1,1,1,0,0,1,1,1,2,2,2,1,1,1,1,3,3,1,1,1,1,0,0,
    3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,1,1,1,3,3,1,1,1,1,0,0,
    3,3,1,1,2,2,2,2,2,2,2,2,1,1,1,1,2,2,2,2,2,1,1,1,0,0,0,0,0,0,0,0,
    1,1,1,1,2,2,2,2,2,2,2,2,1,1,1,1,1,2,2,2,1,1,1,1,0,0,0,0,0,0,0,0,
    1,1,1,1,2,2,2,2,2,2,2,2,1,1,1,1,1,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,0,0,0,0,0,0,0,0,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,0,0,0,0,0,0,0,0,3,3,1,1,1,1,1,1,1,0,0,1,2,2,2,2,2,2,1,1,
    0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,2,2,2,2,2,2,1,1,
    0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,1,1,
    0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,1,1,
    0,0,0,0,0,0,0,0,0,0,2,2,2,3,3,3,3,1,1,0,0,1,2,2,2,2,2,2,2,2,0,0,
    0,0,0,0,0,0,0,0,0,0,2,2,2,3,3,3,3,1,1,0,0,1,2,2,2,2,2,2,2,2,0,0 };

         bool showObjective = false;   

         const string RootPath = "E:/gl/resource/";

         float GetTileCleanPercent(void) {
             float value = 0;

             int tileLevelsToClean = 0;
             int tileLevelsCleaned = 0;

             for (int y = 0; y < MAX_TILES_Y; y++)
             {
                 for (int x = 0; x < MAX_TILES_X; x++)
                 {
                     if (tiles[y * MAX_TILES_X + x].level > 0)
                     {
                         tileLevelsToClean += tiles[y * MAX_TILES_X + x].level;
                         tileLevelsCleaned += tiles[y * MAX_TILES_X + x].state;
                     }
                 }
             }

             value = ((float)(tileLevelsToClean - tileLevelsCleaned) / tileLevelsToClean) * 100.0f;

             return value;
         }

         void InitGameplayScreen() {
             // Initialize GAMEPLAY screen variables here!
             framesCounter = 0;
             finishScreen = 0;
             timeLevelSeconds = TIME_LIMIT_SECONDS;
             levelFinished = false;

             string fontPath = RootPath + "res/cat/star.fnt";

             font = LoadFont(fontPath.c_str());
             string font2Path = RootPath + "res/cat/star2.fnt";
             font2 = LoadFont(font2Path.c_str());

             string roombaPath = RootPath + "res/cat/roomba.png";

             roomba = LoadTexture(roombaPath.c_str());

             string catPath = RootPath + "res/cat/cat.png";

             cat = LoadTexture(catPath.c_str());

             string dirtPath = RootPath + "res/cat/dirtiles.png";
             dirtiles = LoadTexture(dirtPath.c_str());
            
             string furnPath = RootPath + "res/cat/furniture.png";
             furniture = LoadTexture(furnPath.c_str());


             int furCount = 0;

             string furnFilatPath = RootPath + "res/cat/furset.txt";

             FILE* fursetFile = fopen(furnFilatPath.c_str(), "rt");


             if (fursetFile != NULL) {
                 char buffer[512] = { 0 };
                 while (!feof(fursetFile)) {
                     fgets(buffer, 512, fursetFile);
                     switch (buffer[0]) 
                     {
                     case 'f':
                     {
                         sscanf(buffer, "f %i %i %i %i %i",
                             &furset[furCount].id,
                             &furset[furCount].posX,
                             &furset[furCount].posY,
                             &furset[furCount].width,
                             &furset[furCount].height);
                         furCount++;
                     } break;
                     case '.': // This is a comment
                     default: break;
                     }
                 }
                 fclose(fursetFile);
             }


             // Position and size come in cell form, not pixels
             for (int i = 0; i < furCount; i++)
             {
                 furset[i].posX *= TILE_SIZE;
                 furset[i].posY *= TILE_SIZE;
                 furset[i].width *= TILE_SIZE;
                 furset[i].height *= TILE_SIZE;
             }
            

             printf("Furniture SET elements read: %i\n", furCount);

             string furnitureFilePath = RootPath + "res/cat/furmap.txt";

             // Init furniture elements
             FILE* furnitureFile = fopen(furnitureFilePath.c_str(), "rt");
                

             if (furnitureFile != NULL)
             {
                 char buffer[512] = { 0 };
                 while (!feof(furnitureFile))
                 {
                     fgets(buffer, 512, furnitureFile);

                     switch (buffer[0])
                     {
                     case 'f':
                     {
                         sscanf(buffer, "f %i %i %i %i %i",
                             &furmap[furnitureCount].furId,
                             &furmap[furnitureCount].cellX,
                             &furmap[furnitureCount].cellY,
                             &furmap[furnitureCount].state,
                             &furmap[furnitureCount].counter);
                         furnitureCount++;
                     } break;
                     case '.': // This is a comment
                     default: break;
                     }
                 }

                 fclose(furnitureFile);
             }

             printf("Furniture MAP elements read: %i\n", furnitureCount);
            

             // Init dirt tiles
             for (int y = 0; y < MAX_TILES_Y; y++)
             {
                 for (int x = 0; x < MAX_TILES_X; x++)
                 {
                     tiles[y * MAX_TILES_X + x].position = { roomOffset.x + TILE_SIZE * x, roomOffset.y + TILE_SIZE * y };

                     if ((furcolmap[y * MAX_TILES_X + x] != 0) &&
                         (furcolmap[y * MAX_TILES_X + x] != 3))
                     {
                         // TODO: Level of dirtiness depends on difficulty level
                         // Adjust probability of every tile dirt level
                         int dirt = GetRandomValue(0, 100);

                         if (dirt < 50) tiles[y * MAX_TILES_X + x].level = 0;          // 50% probability
                         else if (dirt < 70) tiles[y * MAX_TILES_X + x].level = 1;     // 20% probability
                         else if (dirt < 90) tiles[y * MAX_TILES_X + x].level = 2;     // 10% probability
                         else if (dirt < 100) tiles[y * MAX_TILES_X + x].level = 3;    // 10% probability
                     }
                     else tiles[y * MAX_TILES_X + x].level = 0;

                     tiles[y * MAX_TILES_X + x].state = tiles[y * MAX_TILES_X + x].level;
                     tiles[y * MAX_TILES_X + x].counter = (tiles[y * MAX_TILES_X + x].level == 0) ? 0 : TILE_REQUIRED_CLEAN_TIME;
                     tiles[y * MAX_TILES_X + x].cleaned = (tiles[y * MAX_TILES_X + x].level == 0) ? true : false;
                 }
             }

             // Init score popups
             for (int i = 0; i < MAX_SCORE_POPUPS; i++)
             {
                 popup[i].position = { 0, 0 };
                 popup[i].value = TILE_SCORE_BY_CLEANED_LEVEL;
                 popup[i].enabled = false;
                 popup[i].alpha = 1.0f;
             }
             // Init cat position
             catPosition = { 30 * TILE_SIZE + roomOffset.x, TILE_SIZE + roomOffset.y };
             catTargetPosition = catPosition;

             showObjective = true;

             // Load music and sounds
             result = 0;     // Global variable: screens.h
         }

         // Gameplay Screen Update logic
         void UpdateGameplayScreen(void) {

             if (showObjective)
             {
                 if (IsKeyPressed(KEY_ENTER))
                 {
                     showObjective = false;
                    // PlaySound(fxCoin);
                 }

                 return;
             }

             framesCounter++;

             if (framesCounter == 60)
             {
                 timeLevelSeconds--;

                 if (timeLevelSeconds == 0)
                 {
                     levelFinished = true;
                     finishScreen = 1;
                 //    PlaySound(fxCoin);

                     if (GetTileCleanPercent() >= 80) result = 1;
                 }

                 framesCounter = 0;
             }

             mousePosition = GetMousePosition();
             mouseTileX = (int)floorf((mousePosition.x - roomOffset.x) / TILE_SIZE);
             mouseTileY = (int)floorf((mousePosition.y - roomOffset.y) / TILE_SIZE);

             // Roomba movement logic
             Vector2 prevPosition = roombaPosition;

             if (IsKeyDown(KEY_D)) roombaPosition.x += roombaSpeed.x;
             else if (IsKeyDown(KEY_A)) roombaPosition.x -= roombaSpeed.x;
             if (IsKeyDown(KEY_W)) roombaPosition.y -= roombaSpeed.y;
             else if (IsKeyDown(KEY_S)) roombaPosition.y += roombaSpeed.y;

             // Verify current player position is valid or reset
             roombaTilePosX = (int)(floorf(roombaPosition.x - roomOffset.x) / TILE_SIZE);
             roombaTilePosY = (int)(floorf(roombaPosition.y - roomOffset.y) / TILE_SIZE);
             if ((roombaPosition.x - roomba.width / 2 < roomOffset.x) ||
                 ((roombaPosition.x + roomba.width / 2) >= (roomOffset.x + MAX_TILES_X * TILE_SIZE)) ||
                 (roombaPosition.y - roomba.height / 2 < roomOffset.y) ||
                 ((roombaPosition.y + roomba.height / 2) >= (roomOffset.y + MAX_TILES_Y * TILE_SIZE)) ||
                 (furcolmap[roombaTilePosY * MAX_TILES_X + roombaTilePosX] == 0) ||
                 (furcolmap[roombaTilePosY * MAX_TILES_X + roombaTilePosX] == 3)) roombaPosition = prevPosition;
             for (int y = 0; y < MAX_TILES_Y; y++)
             {
                 for (int x = 0; x < MAX_TILES_X; x++)
                 {
                     // Check if tile requires cleaning
                     if (tiles[y * MAX_TILES_X + x].state > 0)
                     {
                         // TODO: Get better collision area measure, considering round roomba
                         Rectangle cleanRec = GetCollisionRec( { tiles[y * MAX_TILES_X + x].position.x, tiles[y * MAX_TILES_X + x].position.y, 36, 36 },
                             {
                             roombaPosition.x - roomba.width / 2, roombaPosition.y - roomba.height / 2,(float) roomba.width, (float)roomba.height
                         });

                         // Check Roomba is covering at least half of the tile
                         if ((cleanRec.width * cleanRec.height) > TILE_REQUIRED_CLEAN_AREA)
                         {
                             // Start cleaning tile
                             tiles[y * MAX_TILES_X + x].counter--;

                             if (tiles[y * MAX_TILES_X + x].counter < 0)
                             {
                                 tiles[y * MAX_TILES_X + x].state--;

                                 if (tiles[y * MAX_TILES_X + x].state == 0)
                                 {
                                     tiles[y * MAX_TILES_X + x].counter = 0;
                                     score += tiles[y * MAX_TILES_X + x].level * TILE_SCORE_BY_CLEANED_LEVEL;

                                     // Show scoring popup, enable first ready!
                                     for (int i = 0; i < MAX_SCORE_POPUPS; i++)
                                     {
                                         if (!popup[i].enabled)
                                         {
                                             popup[i].position = tiles[y * MAX_TILES_X + x].position;
                                             popup[i].value = TILE_SCORE_BY_CLEANED_LEVEL * tiles[y * MAX_TILES_X + x].level;
                                             popup[i].enabled = true;
                                             popup[i].alpha = 1.0f;
                                             break;
                                         }
                                     }
                                 }
                                 else tiles[y * MAX_TILES_X + x].counter = TILE_REQUIRED_CLEAN_TIME;
                             }
                         }
                     }
                 }
             }
             // Update enabled popups!
             for (int i = 0; i < MAX_SCORE_POPUPS; i++)
             {
                 if (popup[i].enabled)
                 {
                     popup[i].position.y -= 2;
                     popup[i].alpha -= 0.015f;

                     if (popup[i].alpha < 0.0f) popup[i].enabled = false;
                 }
             }
             // Cat movement logic
             if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
             {
                 // Check for a valid cell to move on
                 if ((mousePosition.x > roomOffset.x) && (mousePosition.x < (roomOffset.x + MAX_TILES_X * TILE_SIZE)) &&
                     (mousePosition.y > roomOffset.y) && (mousePosition.y < (roomOffset.y + MAX_TILES_Y * TILE_SIZE)) &&
                     furcolmap[mouseTileY * MAX_TILES_X + mouseTileX] != 0)
                 {
                     catTargetPosition = GetMousePosition();
                     catShouldMove = true;
                 }
             }
            

           //  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) PlaySound(fxCat[GetRandomValue(0, 1)]);
            // if (IsKeyPressed(KEY_SPACE)) PlaySound(fxRoomba[GetRandomValue(0, 2)]);

              // Check if cat should move
             if (catShouldMove)
             {
                 if (CheckCollisionPointCircle(catPosition, catTargetPosition, CAT_TARGET_RADIUS))
                 {
                     catShouldMove = false;

                     // Spread dirt all around selected cell!
                     // NOTE: We consider cat drawing offset
                     catTilePosX = (int)floorf((catPosition.x - cat.width / 2 - roomOffset.x) / TILE_SIZE) + 1;
                     catTilePosY = (int)floorf((catPosition.y - cat.height / 2 - 10 - roomOffset.y) / TILE_SIZE) + 1;
                
                     // Check if tile includes a dirt element
                     if (furcolmap[mouseTileY * MAX_TILES_X + mouseTileX] == 3)
                     {
                         for (int y = (catTilePosY - CAT_DIRT_CELL_RADIUS); y < (catTilePosY + CAT_DIRT_CELL_RADIUS + 1); y++)
                         {
                             for (int x = (catTilePosX - CAT_DIRT_CELL_RADIUS); x < (catTilePosX + CAT_DIRT_CELL_RADIUS + 1); x++)
                             {
                                 if (((y >= 0) && (y < MAX_TILES_Y) && (x >= 0) && (x < MAX_TILES_X)) &&
                                     (tiles[y * MAX_TILES_X + x].state == 0) &&
                                     (furcolmap[y * MAX_TILES_X + x] != 0) &&
                                     (furcolmap[y * MAX_TILES_X + x] != 3))
                                 {
                                     int dirt = GetRandomValue(0, 100);

                                     if (dirt < 50) tiles[y * MAX_TILES_X + x].level = 0;          // 50% probability
                                     else if (dirt < 70) tiles[y * MAX_TILES_X + x].level = 1;     // 20% probability
                                     else if (dirt < 90) tiles[y * MAX_TILES_X + x].level = 2;     // 10% probability
                                     else if (dirt < 100) tiles[y * MAX_TILES_X + x].level = 3;    // 10% probability

                                     tiles[y * MAX_TILES_X + x].state = tiles[y * MAX_TILES_X + x].level;
                                     tiles[y * MAX_TILES_X + x].counter = (tiles[y * MAX_TILES_X + x].level == 0) ? 0 : TILE_REQUIRED_CLEAN_TIME;
                                     tiles[y * MAX_TILES_X + x].cleaned = (tiles[y * MAX_TILES_X + x].level == 0) ? true : false;
                                 }
                             }
                         }
                     }
                 }
                 else
                 {
                     Vector2 dir = Vector2Subtract(catTargetPosition, catPosition);
                     Vector2 dirnorm = Vector2Normalize(dir);

                     catPosition.x += catSpeed.x * dirnorm.x;
                     catPosition.y += catSpeed.y * dirnorm.y;
                 }
             }


             if (levelFinished)
             {
                 // TODO: Check level finished
             }
         }

         // Gameplay Screen Draw logic
         void DrawGameplayScreen(void) {
             DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), GetColor(0x57374cff));

             // Draw tiles
             for (int y = 0; y < MAX_TILES_Y; y++)
             {
                 for (int x = 0; x < MAX_TILES_X; x++)
                 {
                     // Draw dirty tiles
                     DrawTextureRec(dirtiles,{(float) tiles[y * MAX_TILES_X + x].state* TILE_SIZE, 0, TILE_SIZE, TILE_SIZE },
                         {
                         roomOffset.x + TILE_SIZE * x, roomOffset.y + TILE_SIZE * y
                     }, WHITE);

                     // TODO: Draw possible walls
                 }
             }

             // Draw starting point for roomba and cat
             DrawTextureRec(furniture,{ (float)furset[30].posX, (float)furset[30].posY, (float)furset[30].width,  (float)furset[30].height }, roomOffset, WHITE);
             DrawTextureRec(furniture,{ (float)furset[29].posX, (float)furset[29].posY, (float)furset[29].width,  (float)furset[29].height },  { roomOffset.x + 29 * 36, roomOffset.y }, WHITE);

             DrawTexture(roomba, roombaPosition.x - roomba.width / 2, roombaPosition.y - roomba.height / 2, WHITE);
             DrawTexture(cat, catPosition.x - cat.width / 2, catPosition.y - cat.height / 2 - 10, WHITE);
             float furAlpha = 1.0f;
             

             // Draw home objects
             for (int i = 0; i < furnitureCount; i++)
             {
                 if (CheckCollisionCircleRec( { roombaPosition.x - roomba.width / 2, roombaPosition.y - roomba.height / 2 }, roomba.width,
                     {
                     roomOffset.x + furmap[i].cellX * TILE_SIZE, roomOffset.y + furmap[i].cellY * TILE_SIZE,
                       (float)  furset[furmap[i].furId].width, (float)furset[furmap[i].furId].height
                 }) && (furmap[i].state == 1))
                 {
                     DrawTextureRec(furniture, { (float)furset[furmap[i].furId].posX, (float)furset[furmap[i].furId].posY, (float)furset[furmap[i].furId].width, (float)furset[furmap[i].furId].height },
                        {
                         roomOffset.x + furmap[i].cellX * TILE_SIZE, roomOffset.y + furmap[i].cellY * TILE_SIZE
                     }, Fade(WHITE, 0.5f));
                 }
                 else
                 {
                     DrawTextureRec(furniture, { (float)furset[furmap[i].furId].posX,  (float)furset[furmap[i].furId].posY,  (float)furset[furmap[i].furId].width,  (float)furset[furmap[i].furId].height },
                         {
                         roomOffset.x + furmap[i].cellX * TILE_SIZE, roomOffset.y + furmap[i].cellY * TILE_SIZE
                     }, Fade(WHITE, furAlpha));
                 }
             }


             // TODO: If an object has been used by cat, draw it in gray
    // Maybe add a tempo bar for reusing?

    // Draw UI
             DrawTextEx(font2, "SCORE:",  { 80, 10 }, font2.baseSize, 2, WHITE);
             DrawTextEx(font, TextFormat("%i", score),  { 260, 10 }, font.baseSize, 2, WHITE);
             DrawTextEx(font2, "CLEAN:", { 500, 10 }, font2.baseSize, 2, WHITE);
             DrawTextEx(font, TextFormat("%.2f%%", GetTileCleanPercent()),  { 690, 10 }, font.baseSize, 2, WHITE);
             DrawTextEx(font2, "TIME:", { 950, 10 }, font2.baseSize, 2, WHITE);
             DrawTextEx(font, TextFormat("%i:%02is", timeLevelSeconds / 60, timeLevelSeconds % 60), { 1100, 10 }, font.baseSize, 2, WHITE);

             // Debug information
             //DrawText(TextFormat("CatTilePos: [ %i, %i ]", catTilePosX, catTilePosY), roomOffset.x, 690, 20, RAYWHITE);
             //DrawText(TextFormat("MousePos: [ %i, %i ]", mouseTileX, mouseTileY), 400, 690, 20, RED);
             //DrawText(TextFormat("RoombaPos: [ %i, %i ]", roombaTilePosX, roombaTilePosY), 600, 690, 20, GREEN);

             if ((mouseTileY >= 0) && (mouseTileY < MAX_TILES_Y) && (mouseTileX >= 0) && (mouseTileX < MAX_TILES_X))
             {
                 DrawRectangleLinesEx( {
                     tiles[mouseTileY * MAX_TILES_X + mouseTileX].position.x,
                         tiles[mouseTileY * MAX_TILES_X + mouseTileX].position.y, TILE_SIZE, TILE_SIZE
                 }, 2, RED);
             }
             // Draw enabled popups!
             for (int i = 0; i < MAX_SCORE_POPUPS; i++)
             {
                 if (popup[i].enabled) DrawText(TextFormat("+%i", popup[i].value), popup[i].position.x, popup[i].position.y, 20, Fade(RED, popup[i].alpha));
             }

             // Show objective
             if (showObjective)
             {
                 DrawRectangle(0, 150, GetScreenWidth(), GetScreenHeight() - 300, Fade(DARKGRAY, 0.7f));
                 DrawTextEx(font2, "OBJECTIVE:", { 500, 240 }, font2.baseSize, 2, WHITE);
                 DrawTextEx(font, "CLEAN 80% OF THE ROOM",  { 300, 320 }, font.baseSize, 2, WHITE);
             }
         }

         // Gameplay Screen Unload logic
         void UnloadGameplayScreen(void)
         {
             // Unload GAMEPLAY screen variables here!
             UnloadTexture(roomba.id);
             UnloadTexture(cat.id);
             UnloadTexture(dirtiles.id);
             UnloadTexture(furniture.id);
         }

         // Gameplay Screen should finish?
         int FinishGameplayScreen(void)
         {
             return finishScreen;
         }
	};


}