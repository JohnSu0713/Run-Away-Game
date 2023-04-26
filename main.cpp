#include <iostream>
#include <cstdio>
#include <vector>
#include <random>
#include "raylib.h"

// Global Variables for width and height of window
const int width{1200};
const int height{800};
const int rectSize{20};
const int circleRadius{20};

// Random Engine
struct RectangleObject
{
    int x;
    int y;
    int size;
    float xSpeed;
    float ySpeed;
};

// Detect collision
bool CheckCollision(int leftEdgeX, int rightEdgeX, int topEdgeY, int bottomEdgeY, int leftEdgeRectX, int rightEdgeRectX, int topEdgeRectY, int bottomEdgeRectY)
{
    if (leftEdgeX < rightEdgeRectX && rightEdgeX > leftEdgeRectX && topEdgeY < bottomEdgeRectY && bottomEdgeY > topEdgeRectY)
    {
        return true;
    }
    return false;
}

// Generate position
int GeneratePosition(int size)
{
    std::default_random_engine randomEngine(std::random_device{}());
    std::uniform_int_distribution<int> positionDistribution(size, height - size); // 設定方塊的隨機位置範圍
    return positionDistribution(randomEngine);
}

// Generate speed with non-zero value
float GenerateSpeed()
{
    std::default_random_engine randomEngine(std::random_device{}());
    std::uniform_real_distribution<float> speedDistribution(-20.0f, 20.0f);
    float speed = speedDistribution(randomEngine);
    while (speed == 0.0f)
    {
        speed = speedDistribution(randomEngine);
    }
    return speed;
}

// Create Rectangle Object in rectangle vector
void CreateRectangle(std::vector<RectangleObject> &rectangles, int size)
{
    RectangleObject rect;
    rect.x = GeneratePosition(size);
    rect.y = GeneratePosition(size);
    rect.size = size;
    rect.xSpeed = 2;
    rect.ySpeed = 2;
    rectangles.push_back(rect);
}

// Main Function
int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT); // NOTE: Try to enable MSAA 4X

    // Initialize score
    int score{0};

    // Count time
    int frameCounter{0};

    // Speed multiplier
    float speedMultiplier{1.0f};

    // Spawn Timer
    float spawnTimer = 0.0f;    // 用於計時生成新方塊的計時器
    float spawnInterval = 5.0f; // 新方塊生成的間隔，可以根據需要調整

    InitWindow(width, height, "Hello World");

    // Background Music
    InitAudioDevice(); // 初始化音頻設備
    Music bgSound = LoadMusicStream("bg.mp3");
    Music overSound = LoadMusicStream("over.mp3");
    PlayMusicStream(bgSound); // 開始播放背景音樂

    // Circle Coordinates
    int circleX{width / 2};
    int circleY{height / 2};
    // Circle edges
    int leftEdgeX{circleX - circleRadius};
    int rightEdgeX{circleX + circleRadius};
    int topEdgeY{circleY - circleRadius};
    int bottomEdgeY{circleY + circleRadius};

    // Random Engine for rectangle
    std::vector<RectangleObject> rectangles;
    CreateRectangle(rectangles, rectSize);

    // Set Target FPS
    SetTargetFPS(60);

    // Flag to check collision
    bool collision{false};

    // Game Loop
    while (!WindowShouldClose())
    {
        // Update Frame Counter
        frameCounter++;

        // Update speed multiplier
        if (frameCounter % 600 == 0 && !collision) speedMultiplier += 0.1f;

        // Update Music Stream
        UpdateMusicStream(bgSound);

        // Draw Game Objects
        BeginDrawing();
        ClearBackground(WHITE);

        // Update Score
        if (frameCounter % 120 == 0 && !collision) score++;

        if (collision)
        {
            DrawText("Game Over", width / 2 - 50, height / 2, 20, RED);
            StopMusicStream(bgSound);
            PlayMusicStream(overSound);   // 開始播放背景音樂
            UpdateMusicStream(overSound); // 更新背景音樂流
        }
        else
        {
            // Draw Circle
            DrawCircle(circleX, circleY, circleRadius, RED);

            for (auto &rect : rectangles)
            {
                DrawRectangle(rect.x, rect.y, rectSize, rectSize, PURPLE);
                
                // Update rectangle edges
                rect.x += rect.xSpeed * speedMultiplier;
                rect.y += rect.ySpeed * speedMultiplier;

                // Check screen boundaries and bounce
                if (rect.x < 0 || rect.x > width - rect.size)
                {
                    rect.xSpeed = -rect.xSpeed;
                }
                if (rect.y < 0 || rect.y > height - rect.size)
                {
                    rect.ySpeed = -rect.ySpeed;
                }
                // Check collision
                if (CheckCollision(leftEdgeX, rightEdgeX, topEdgeY, bottomEdgeY, rect.x, rect.x + rect.size, rect.y, rect.y + rect.size))
                {
                    collision = true;
                }
            }

            // Game Logic
            if (IsKeyDown(KEY_LEFT) && circleX > 0)
            {
                circleX -= 10;
            }
            if (IsKeyDown(KEY_RIGHT) && circleX < width)
            {
                circleX += 10;
            }
            if (IsKeyDown(KEY_UP) && circleY > 0)
            {
                circleY -= 10;
            }
            if (IsKeyDown(KEY_DOWN) && circleY < height)
            {
                circleY += 10;
            }

            // Update circle edges
            leftEdgeX = circleX - circleRadius;
            rightEdgeX = circleX + circleRadius;
            topEdgeY = circleY - circleRadius;
            bottomEdgeY = circleY + circleRadius;
        }

        // Spawn new rectangles
        spawnTimer += GetFrameTime();
        if (spawnTimer > spawnInterval)
        {
            // Create new rectangle
            CreateRectangle(rectangles, rectSize);
            spawnTimer = 0;
        }
        char scoreText[50];
        snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
        int scoreTextWidth = MeasureText(scoreText, 20);
        DrawText(scoreText, width - scoreTextWidth - 10, 10, 20, GREEN);

        // Game Logic End
        EndDrawing();
    }
    // release sound
    UnloadMusicStream(bgSound);   // 釋放背景音樂資源
    UnloadMusicStream(overSound); // 釋放背景音樂資源
    CloseAudioDevice();           // 關閉音頻設備
}