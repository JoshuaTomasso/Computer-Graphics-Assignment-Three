#pragma once
#include "Shader.h"
#include "greendragon.h"
#include "fire_02.h"
#include "tiles_12.h"
#include "XTime.h"

unsigned int MyAlphaBlend(unsigned int DestinationColor, unsigned int SourceColor)
{
    unsigned int destA = (DestinationColor >> 24) & 0xFF;
    unsigned int destR = (DestinationColor >> 16) & 0xFF;
    unsigned int destG = (DestinationColor >> 8) & 0xFF;
    unsigned int destB = (DestinationColor >> 0) & 0xFF;

    unsigned int srcA = (SourceColor >> 24) & 0xFF;
    unsigned int srcR = (SourceColor >> 16) & 0xFF;
    unsigned int srcG = (SourceColor >> 8) & 0xFF;
    unsigned int srcB = (SourceColor >> 0) & 0xFF;

    float alpha = srcA / 255.0f;

    unsigned int outA = static_cast<unsigned int>(destA * (1.0f - alpha) + srcA * alpha);
    unsigned int outR = static_cast<unsigned int>(destR * (1.0f - alpha) + srcR * alpha);
    unsigned int outG = static_cast<unsigned int>(destG * (1.0f - alpha) + srcG * alpha);
    unsigned int outB = static_cast<unsigned int>(destB * (1.0f - alpha) + srcB * alpha);

    return (outA << 24) | (outR << 16) | (outG << 8) | outB;
}

// Pixel draw
void drawPixel(int x, int y, float z, unsigned int color) 
{
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return;

    int index = convertDimCoords(x, y);

    // Z-buffer test
    if (z >= DEPTH_BUFFER[index])
        return;

    DEPTH_BUFFER[index] = z;

    if ((color >> 24) == 0xFF) // Alpha is 255 (fully opaque)
    {
        SCREEN_ARRAY[index] = color;
    }
    else
    {
        SCREEN_ARRAY[index] = MyAlphaBlend(SCREEN_ARRAY[index], color);
    }
}

// Draw line using float interpolation
void DrawLine(const Float4WithColor& start, const Float4WithColor& end)
{
    Float4WithColor vStart = start;
    Float4WithColor vEnd = end;

    if (VertexShader)
    {
        VertexShader(vStart.pos);
        VertexShader(vEnd.pos);
    }

    ScreenXY screenStart = NDCtoScreen(vStart.pos);
    ScreenXY screenEnd = NDCtoScreen(vEnd.pos);

    int dx = std::abs(screenEnd.x - screenStart.x);
    int dy = std::abs(screenEnd.y - screenStart.y);
    int steps = max(dx, dy);
    if (steps == 0)
    {
        drawPixel(screenStart.x, screenStart.y, 0.999f, vStart.color);
        return;
    }

    float xStep = (screenEnd.x - screenStart.x) / float(steps);
    float yStep = (screenEnd.y - screenStart.y) / float(steps);

    float x = float(screenStart.x);
    float y = float(screenStart.y);

    for (int i = 0; i <= steps; ++i)
    {
        drawPixel(int(round(x)), int(round(y)), 0.999f, vStart.color);
        x += xStep;
        y += yStep;
    }
}

int RandomNumber100to400() 
{
    return rand() % 301 + 100;
}

void blockImageTransfer(int startX, int startY, int endX, int endY, int worldX, int worldY, const unsigned int* array, int arrayWidth) 
{
    for (int y = startY; y < endY; ++y)
    {
        for (int x = startX; x < endX; ++x)
        {
            int srcIndex = y * arrayWidth + x;

            if (worldX + (x - startX) < 0 || worldX + (x - startX) >= SCREEN_WIDTH ||
                worldY + (y - startY) < 0 || worldY + (y - startY) >= SCREEN_HEIGHT)
                continue;

            unsigned int color = C2C(array[srcIndex]);
            drawPixel(worldX + (x - startX), worldY + (y - startY), 0.005f, color);
        }
    }
}

void clearScreen() 
{
    for (int i = 0; i < NUM_PIXELS; ++i)
    {
        SCREEN_ARRAY[i] = 0xFF000000;
        DEPTH_BUFFER[i] = FLT_MAX;
    }
}

float EdgeFunction(const Float4& a, const Float4& b, const Float4& c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}


bool IsTopLeft(const Float4& a, const Float4& b)
{
    return (a.y == b.y && a.x > b.x) || (a.y > b.y);
}

unsigned int SampleTexture(float u, float v)
{
    u = Clamp(u, 0.0f, 1.0f);
    v = Clamp(v, 0.0f, 1.0f);

    int x = static_cast<int>(u * (greendragon_width - 1));
    int y = static_cast<int>(v * (greendragon_height - 1));
    int index = y * greendragon_width + x;

    return C2C(greendragon_pixels[index]);
}

void DrawFilledTriangle(const Float4WithColor& v0, const Float4WithColor& v1, const Float4WithColor& v2)
{
    Float4WithColor p0 = v0;
    Float4WithColor p1 = v1;
    Float4WithColor p2 = v2;

    if (VertexShader)
    {
        VertexShader(p0.pos);
        VertexShader(p1.pos);
        VertexShader(p2.pos);
    }

    ScreenXY s0 = NDCtoScreen(p0.pos);
    ScreenXY s1 = NDCtoScreen(p1.pos);
    ScreenXY s2 = NDCtoScreen(p2.pos);

    Float4 f0 = { static_cast<float>(s0.x), static_cast<float>(s0.y), p0.pos.z / p0.pos.w, 1.0f };
    Float4 f1 = { static_cast<float>(s1.x), static_cast<float>(s1.y), p1.pos.z / p1.pos.w, 1.0f };
    Float4 f2 = { static_cast<float>(s2.x), static_cast<float>(s2.y), p2.pos.z / p2.pos.w, 1.0f };

    int minX = MyMinimum(s0.x, s1.x, s2.x);
    int maxX = MyMaximum(s0.x, s1.x, s2.x);
    int minY = MyMinimum(s0.y, s1.y, s2.y);
    int maxY = MyMaximum(s0.y, s1.y, s2.y);

    float area = EdgeFunction(f0, f1, f2);
    if (area == 0.0f) return;

    bool e0TopLeft = IsTopLeft(f1, f2);
    bool e1TopLeft = IsTopLeft(f2, f0);
    bool e2TopLeft = IsTopLeft(f0, f1);

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            Float4 p = { static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f, 0, 1 };
            float w0 = EdgeFunction(f1, f2, p);
            float w1 = EdgeFunction(f2, f0, p);
            float w2 = EdgeFunction(f0, f1, p);

            if ((w0 > 0 || (w0 == 0 && e0TopLeft)) &&
                (w1 > 0 || (w1 == 0 && e1TopLeft)) &&
                (w2 > 0 || (w2 == 0 && e2TopLeft)))
            {
                float invArea = 1.0f / area;
                w0 *= invArea;
                w1 *= invArea;
                w2 *= invArea;

                float z = w0 * f0.z + w1 * f1.z + w2 * f2.z;

                // === Interpolate UVs ===
                float u = w0 * p0.u + w1 * p1.u + w2 * p2.u;
                float v = w0 * p0.v + w1 * p1.v + w2 * p2.v;

                // === Clamp UVs (use Clamp() if C++17 not available) ===
                u = Clamp(u, 0.0f, 1.0f);
                v = Clamp(v, 0.0f, 1.0f);

                // === Sample Texture ===
                int texX = static_cast<int>(u * (greendragon_width - 1));
                int texY = static_cast<int>(v * (greendragon_height - 1));
                int texIndex = texY * greendragon_width + texX;

                unsigned int texColor = C2C(greendragon_pixels[texIndex]);

                // Optional: run pixel shader
                if (PixelShader)
                    PixelShader(texColor);

                // Final draw
                drawPixel(x, y, z, texColor);
            }
        }
    }
}


void LabOneStuff(XTime& timer, bool& treesInitialized, Tree* treeList, int numOfTrees, int& fireX, int& fireY, double& fireTimeAccumulator, const double fireFrameDuration)
{
    timer.Signal();
    fireTimeAccumulator += timer.Delta();

    if (fireTimeAccumulator >= fireFrameDuration) 
    {
        for (int y = 0; y < SCREEN_HEIGHT; y += 30)
        {
            for (int x = 0; x < SCREEN_WIDTH; x += 30)
            {
                blockImageTransfer(290, 130, 320, 160, x, y, tiles_12_pixels, tiles_12_width);
            }
        }

        if (!treesInitialized)
        {
            for (int i = 0; i < numOfTrees; i++)
            {
                treeList[i].x = RandomNumber100to400();
                treeList[i].y = RandomNumber100to400();
            }
            treesInitialized = true;
        }

        for (int i = 0; i < numOfTrees; i++) 
        {
            blockImageTransfer(320, 15, 385, 95, treeList[i].x, treeList[i].y, tiles_12_pixels, tiles_12_width);
        }

        blockImageTransfer(
            fireX, fireY,
            fireX + 128, fireY + 128,
            SCREEN_WIDTH / 2 - 64,
            SCREEN_HEIGHT / 2 - 64,
            fire_02_pixels,
            fire_02_width
        );

        fireTimeAccumulator -= fireFrameDuration;
        fireX += 128;
        if (fireX >= fire_02_width)
        {
            fireX = 0;
            fireY += 128;
            if (fireY >= fire_02_height)
                fireY = 0;
        }
    }
}

void LabTwoStuff()
{
    auto ScreenToNDC = [](int x, int y) -> Float4 {
        Float4 f;
        f.x = (2.0f * x / SCREEN_WIDTH) - 1.0f;
        f.y = 1.0f - (2.0f * y / SCREEN_HEIGHT); // invert Y
        f.z = 0.0f;
        f.w = 1.0f;
        return f;
        };

    auto MakeVertex = [&](int x, int y, unsigned int color) -> Float4WithColor {
        return { ScreenToNDC(x, y), color };
        };

    // Red star
    DrawLine(MakeVertex(356, 356, 0xffff0000), MakeVertex(250, 400, 0xffff0000));
    DrawLine(MakeVertex(144, 144, 0xffff0000), MakeVertex(250, 100, 0xffff0000));
    DrawLine(MakeVertex(400, 250, 0xffff0000), MakeVertex(356, 356, 0xffff0000));
    DrawLine(MakeVertex(100, 250, 0xffff0000), MakeVertex(144, 144, 0xffff0000));
    DrawLine(MakeVertex(250, 400, 0xffff0000), MakeVertex(144, 356, 0xffff0000));
    DrawLine(MakeVertex(250, 100, 0xffff0000), MakeVertex(356, 144, 0xffff0000));
    DrawLine(MakeVertex(144, 356, 0xffff0000), MakeVertex(100, 250, 0xffff0000));
    DrawLine(MakeVertex(356, 144, 0xffff0000), MakeVertex(400, 250, 0xffff0000));

    // Extra lines
    DrawLine(MakeVertex(250, 100, 0xff00ff00), MakeVertex(250, 400, 0xff00ff00)); // Green vertical
    DrawLine(MakeVertex(356, 356, 0xff0000ff), MakeVertex(144, 144, 0xff0000ff)); // Blue diagonal
    DrawLine(MakeVertex(144, 356, 0xff00ffff), MakeVertex(356, 144, 0xff00ffff)); // Cyan diagonal
    DrawLine(MakeVertex(400, 250, 0xffffff00), MakeVertex(100, 250, 0xffffff00)); // Yellow horizontal
}

void LabThreeStuff(XTime& timer, Float4* gridVerts, int gridVertCount, Float4WithColor* cubeVertsUV, int cubeVertCount)
{
    timer.Signal();
    float elapsed = static_cast<float>(timer.TotalTime());

    Matrix4x4 rotationY = CreateRotationYMatrix(elapsed);
    Matrix4x4 translation = CreateTranslationMatrix(0.0f, 0.25f, 0.0f);
    Matrix4x4 worldCube;
    MatrixMatrixMultiply(worldCube, rotationY, translation);

    Matrix4x4 viewRotation = CreateRotationXMatrix(18.0f * DEG2RAD);
    Matrix4x4 viewTranslation = CreateTranslationMatrix(0.0f, 0.25f, -1.0f);
    Matrix4x4 viewCombined;
    MatrixMatrixMultiply(viewCombined, viewRotation, viewTranslation);
    Matrix4x4 view = InverseRigidBodyMatrix(viewCombined);

    Matrix4x4 proj = CreatePerspectiveMatrix(90.0f * DEG2RAD, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 10.0f);

    VertexShader = VS_World;
    SV_ViewMatrix = view;
    SV_ProjectionMatrix = proj;

    // Draw grid
    SV_WorldMatrix = CreateIdentityMatrix();
    for (int i = 0; i < gridVertCount; i += 2)
    {
        Float4WithColor v0 = { gridVerts[i], 0xFFFFFFFF };
        Float4WithColor v1 = { gridVerts[i + 1], 0xFFFFFFFF };
        DrawLine(v0, v1);
    }

    // Draw cube
    SV_WorldMatrix = worldCube;
    for (int i = 0; i < cubeVertCount; i += 3)
    {
        DrawFilledTriangle(cubeVertsUV[i], cubeVertsUV[i + 1], cubeVertsUV[i + 2]);
    }

    PixelShader = nullptr;
}
