#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "3DPointEngine.hpp"
#define PI 3.141592654

typedef Vector3 vec3;

const int RECT_SIZE = 100;
const float DRONE_SCALE = 3.0f;
const float AIR_FRICTION = 100.f;
const float MULT = 150.f;

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
    float steer = 0.0;
    float rad = 0.5f;
    pe.addRectangle(0, 0, 0, 0, 0, 0);
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
    
    Point* backLeft;
    Point* backRight;
    Point* frontLeft;
    Point* frontRight;
    Point* center;

    backLeft = &pe.getPoint(1);
    backRight = &pe.getPoint(2);
    frontLeft = &pe.getPoint(0);
    frontRight = &pe.getPoint(3);
    center = &pe.getPoint(4);

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 10.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;                   
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        camera.position = center->getPos() + vec3(0, 10, 10);
        camera.target = center->getPos();
        float dt = 1.0 / 60.f;
        if(dt < 1.0)
        {
            if(IsKeyDown(KEY_D))
            {
                vec3 acc = vec3(0, MULT, 0);
                frontLeft->setAcc(acc);
                backLeft->setAcc(acc);
            }
            else 
            if(IsKeyDown(KEY_Q))
            {
                vec3 acc = vec3(0, MULT, 0);
                frontRight->setAcc(acc);
                backRight->setAcc(acc);
            }
            else if(IsKeyDown(KEY_SPACE))
            {
                vec3 acc = vec3(0, MULT, 0);
                frontRight->setAcc(acc);
                frontLeft->setAcc(acc);
                backLeft->setAcc(acc);
                backRight->setAcc(acc);
            }
            for(int i = 0; i < 4; i++)
            {
                Point* p = &pe.getPoint(i);
                vec3 acc = p->getOldPos() - p->getPos();
                p->addAcc(vec3(acc.x, 0.0, acc.z) * AIR_FRICTION);
            }
            pe.updatePointPos(dt);
            pe.applyCollisions(4);
            pe.applyConstraints(4, dt);
            BeginDrawing();
                ClearBackground(RAYWHITE);
                BeginMode3D(camera);
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
