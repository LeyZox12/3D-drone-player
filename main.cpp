#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "3DPointEngine.hpp"
#define PI 3.141592654

typedef Vector3 vec3;

const int RECT_SIZE = 100;
const float DRONE_SCALE = 3.0f;

PointEngine pe;

vec3 move = vec3(0, 0, 0);

//
//  0------3
//  |      |
//  |   4  |
//  |      |
//  1------2
//
//


int main(void)
{
    pe.addRectangle(-2.5, -2.5, -2.5, 10.0, 1.5, 10.0);
    float rad = 0.5f;
    pe.addPoint(vec3(-DRONE_SCALE, 0.0, -DRONE_SCALE), false, false, rad, 200.f, RED);
    pe.addPoint(vec3(-DRONE_SCALE, 0.0, DRONE_SCALE), false, false, rad, 200.f, RED);
    pe.addPoint(vec3(DRONE_SCALE, 0.0, DRONE_SCALE), false, false, rad, 200.f, RED);
    pe.addPoint(vec3(DRONE_SCALE, 0.0, -DRONE_SCALE), false, false, rad, 200.f, RED);
    pe.addPoint(vec3(0.0, 0.0, 0.0), false, false, 1.f, 200.f, RED);
    for(int i = 0; i < 5; i++)
    for(int j = 0; j < 5; j++)
    {
        pe.addConstraint(i, j, 2, 0.0);
    }
    
    Point* backLeft,backRight,frontLeft,frontRight,center;
    backLeft =;

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;                   
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = 1.0 / 60.f;
        if(dt < 1.0)
        {
            if(IsKeyDown(KEY_D))
            {
                vec3 acc = vec3(0, 100.f, 0);
                pe.getPoint(0).setAcc(acc);
                pe.getPoint(1).setAcc(acc);
            }
            else 
            if(IsKeyDown(KEY_Q))
            {
                vec3 acc = vec3(0, 100.f, 0);
                pe.getPoint(2).setAcc(acc);
                pe.getPoint(3).setAcc(acc);
            }
            else if(IsKeyDown(KEY_SPACE))
            {
                vec3 acc = vec3(0, 100.f, 0);
                pe.getPoint(0).setAcc(acc);
                pe.getPoint(1).setAcc(acc);
                pe.getPoint(2).setAcc(acc);
                pe.getPoint(3).setAcc(acc);
            }
            pe.updatePointPos(dt);
            pe.applyCollisions(4);
            pe.applyConstraints(4, dt);
            BeginDrawing();
                ClearBackground(RAYWHITE);
                BeginMode3D(camera);
                    DrawCube(cubePosition, 2.0f, 0.5f, 2.0f, RED);
                    pe.display(BLACK);
                    DrawGrid(10, 1.5f);
                EndMode3D();
                DrawFPS(10, 10);
            EndDrawing();
        }
    }
    CloseWindow(); 
    return 0;
}
