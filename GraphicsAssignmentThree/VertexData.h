#pragma once
#include "Defines.h"

#ifndef GRID_CUBE_VERTS_DEFINED
#define GRID_CUBE_VERTS_DEFINED

Float4 gridVerts[44];
Float4WithColor cubeVertsUV[36]; // Now using 36 vertices for 6 faces

#endif // GRID_CUBE_VERTS_DEFINED

void InitializeGridVerts()
{
    int index = 0;
    for (int i = 0; i <= 10; ++i)
    {
        float t = -0.5f + i * (1.0f / 10.0f);
        gridVerts[index++] = { -0.5f, 0.0f, t, 1.0f };
        gridVerts[index++] = { 0.5f, 0.0f, t, 1.0f };
        gridVerts[index++] = { t, 0.0f, -0.5f, 1.0f };
        gridVerts[index++] = { t, 0.0f,  0.5f, 1.0f };
    }
}

void InitializeCubeVerts()
{
    int i = 0;
    float s = 0.25f;

    // Front
    cubeVertsUV[i++] = { { -s, -s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s, -s, -s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s, -s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s,  s, -s, 1.0f }, 0, 0.0f, 0.0f };

    // Back
    cubeVertsUV[i++] = { {  s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { { -s, -s,  s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { { -s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { {  s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { { -s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { {  s,  s,  s, 1.0f }, 0, 0.0f, 0.0f };

    // Left
    cubeVertsUV[i++] = { { -s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { { -s, -s, -s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { { -s,  s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { { -s,  s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s,  s,  s, 1.0f }, 0, 0.0f, 0.0f };

    // Right
    cubeVertsUV[i++] = { {  s, -s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s, -s,  s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { {  s, -s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { {  s,  s, -s, 1.0f }, 0, 0.0f, 0.0f };

    // Top
    cubeVertsUV[i++] = { { -s,  s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s, -s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s,  s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s,  s,  s, 1.0f }, 0, 0.0f, 0.0f };

    // Bottom
    cubeVertsUV[i++] = { { -s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s, -s,  s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { {  s, -s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s, -s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s, -s, -s, 1.0f }, 0, 0.0f, 0.0f };
}
