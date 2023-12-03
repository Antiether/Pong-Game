#include <raylib.h>
#include <cstdio>
#include <algorithm>

using namespace std;

struct Ball
{
    float x, y;
    float speedX, speedY;
    float radius;

    void Draw()
    {
        DrawCircle((int)x, (int)y, radius, WHITE);
    }
};

struct Paddle
{
    float x, y;
    float speed;
    float width, height;

    Rectangle GetRect()
    {
        return Rectangle{ x - width / 2, y - height / 2, 10, 100 };
    }

    void Draw()
    {
        DrawRectangleRec(GetRect(), WHITE);
    }
};

void DrawPaddleWithShadow(Paddle paddle)
{
    DrawRectangle(paddle.x - paddle.width / 2 + 4.5, paddle.y - paddle.height / 2 + 4.5, paddle.width, paddle.height, BLACK);
    DrawRectangleRec(paddle.GetRect(), BLACK);
}
void DrawBallWithShadow(Ball ball)
{
    DrawCircle(ball.x + 5, ball.y + 5, ball.radius, BLACK);
    DrawCircle(ball.x + 5, ball.y + 5, ball.radius, BLACK);
}

void DrawBallWithTrail(Ball ball, Vector2 trailPositions[], int trailLength)
{
    const Color trailColor = { 255, 255, 255, 200 };

    for (int i = trailLength - 1; i > 0; --i)
    {
        trailPositions[i] = trailPositions[i - 1];
    }

    trailPositions[0] = { ball.x, ball.y };

    for (int i = 0; i < trailLength; ++i)
    {
        float alpha = (float)(trailLength - i) / trailLength;
        DrawCircleV(trailPositions[i], ball.radius, Fade(trailColor, alpha));
    }

    DrawCircle(ball.x, ball.y, ball.radius, WHITE);
}
const char* SimpleTextFormat(const char* text, int value)
{
    static char buffer[256];
    snprintf(buffer, sizeof(buffer), text, value);
    return buffer;
}


enum GameState
{
    StartMenu,
    Playing,
};

enum ScoreLimit
{
    ScoreLimit5 = 5,
    ScoreLimit10 = 10,
    ScoreLimit15 = 15,
    ScoreLimit20 = 20,
};

int main()
{
    InitWindow(800, 600, "Pong");
    SetWindowState(FLAG_VSYNC_HINT);

    Ball ball;
    ball.x = GetScreenWidth() / 2.0f;
    ball.y = GetScreenHeight() / 2.0f;
    ball.radius = 5;
    ball.speedX = 300;
    ball.speedY = 300;

    Paddle leftPaddle;
    leftPaddle.x = 50;
    leftPaddle.y = GetScreenHeight() / 2;
    leftPaddle.width = 10;
    leftPaddle.height = 100;
    leftPaddle.speed = 500;

    Paddle rightPaddle;
    rightPaddle.x = GetScreenWidth() - 50;
    rightPaddle.y = GetScreenHeight() / 2;
    rightPaddle.width = 10;
    rightPaddle.height = 100;
    rightPaddle.speed = 500;

    int player1Score = 0;
    int player2Score = 0;

    const char* winnerText = nullptr;
    const char* restartGame = nullptr;

    const int trailLength = 10;
    Vector2 trailPositions[trailLength] = { 0 };

    char buffer[256];

    GameState gameState = StartMenu;
    ScoreLimit scoreLimit = ScoreLimit5;

    while (!WindowShouldClose())
    {
        switch (gameState)
        {
        case StartMenu:
            BeginDrawing();
            ClearBackground(DARKPURPLE);

            DrawText("Pong - Start Menu", GetScreenWidth() / 2 - MeasureText("Pong - Start Menu", 30) / 2, GetScreenHeight() / 4, 30, WHITE);
            DrawText("Choose Score Limit Using Z,X,C,V Button", GetScreenWidth() / 2 - MeasureText("Choose Score Limit Using Z,X,C,V Button", 20) / 2, GetScreenHeight() / 2 - 30, 20, WHITE);
            DrawText("Score Limit:", 10, GetScreenHeight() - 50, 20, WHITE);
            DrawText("5 ", 160, GetScreenHeight() - 50, (scoreLimit == ScoreLimit5) ? 30 : 20, (scoreLimit == ScoreLimit5) ? YELLOW : WHITE);
            DrawText("10", 220, GetScreenHeight() - 50, (scoreLimit == ScoreLimit10) ? 30 : 20, (scoreLimit == ScoreLimit10) ? YELLOW : WHITE);
            DrawText("15", 280, GetScreenHeight() - 50, (scoreLimit == ScoreLimit15) ? 30 : 20, (scoreLimit == ScoreLimit15) ? YELLOW : WHITE);
            DrawText("20", 340, GetScreenHeight() - 50, (scoreLimit == ScoreLimit20) ? 30 : 20, (scoreLimit == ScoreLimit20) ? YELLOW : WHITE);

            EndDrawing();

            if (IsKeyPressed(KEY_Z)) scoreLimit = ScoreLimit5;
            else if (IsKeyPressed(KEY_X)) scoreLimit = ScoreLimit10;
            else if (IsKeyPressed(KEY_C)) scoreLimit = ScoreLimit15;
            else if (IsKeyPressed(KEY_V)) scoreLimit = ScoreLimit20;

            if (IsKeyPressed(KEY_ENTER))
            {
                gameState = Playing;
                player1Score = 0;
                player2Score = 0;
                winnerText = nullptr;
                restartGame = nullptr;
            }
            break;

        case Playing:
            ball.x += ball.speedX * GetFrameTime();
            ball.y += ball.speedY * GetFrameTime();
            if (ball.y < 0)
            {
                ball.y = 0;
                ball.speedY *= -1;
            }
            if (ball.y > GetScreenHeight())
            {
                ball.y = GetScreenHeight();
                ball.speedY *= -1;
            }

            if (IsKeyDown(KEY_W) && leftPaddle.y - leftPaddle.speed * GetFrameTime() - leftPaddle.height / 2 > 0)
            {
                leftPaddle.y -= leftPaddle.speed * GetFrameTime();
            }
            if (IsKeyDown(KEY_S) && leftPaddle.y + leftPaddle.speed * GetFrameTime() + leftPaddle.height / 2 < GetScreenHeight())
            {
                leftPaddle.y += leftPaddle.speed * GetFrameTime();
            }
            if (IsKeyDown(KEY_UP) && rightPaddle.y - rightPaddle.speed * GetFrameTime() - rightPaddle.height / 2 > 0)
            {
                rightPaddle.y -= rightPaddle.speed * GetFrameTime();
            }
            if (IsKeyDown(KEY_DOWN) && rightPaddle.y + rightPaddle.speed * GetFrameTime() + rightPaddle.height / 2 < GetScreenHeight())
            {
                rightPaddle.y += rightPaddle.speed * GetFrameTime();
            }

            if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, leftPaddle.GetRect()))
            {
                if (ball.speedX < 0)
                {
                    ball.speedX *= -1.1f;
                    ball.speedY = (ball.y - leftPaddle.y) / (leftPaddle.height / 2) * ball.speedX;
                }
            }
            if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, rightPaddle.GetRect()))
            {
                if (ball.speedX > 0)
                {
                    ball.speedX *= -1.1f;
                    ball.speedY = (ball.y - rightPaddle.y) / (rightPaddle.height / 2) * ball.speedX;
                }
            }

            if (ball.x < 0)
            {
                if (!winnerText)
                {
                    player2Score++;
                    if (player2Score == scoreLimit)
                    {
                        winnerText = "Player 2 Wins!";
                        restartGame = "Press Enter to Return to Start Menu";
                    }
                    else
                    {
                        ball.x = GetScreenWidth() / 2;
                        ball.y = GetScreenHeight() / 2;
                        ball.speedX = 300;
                        ball.speedY = 300;
                    }
                }
            }
            if (ball.x > GetScreenWidth())
            {
                if (!winnerText)
                {
                    player1Score++;
                    if (player1Score == scoreLimit)
                    {
                        winnerText = "Player 1 Wins!";
                        restartGame = "Press Enter to Return to Start Menu";
                    }
                    else
                    {
                        ball.x = GetScreenWidth() / 2;
                        ball.y = GetScreenHeight() / 2;
                        ball.speedX = 300;
                        ball.speedY = 300;
                    }
                }
            }
            if (winnerText && IsKeyPressed(KEY_ENTER))
            {
                winnerText = nullptr;
                restartGame = nullptr;
                player1Score = 0;
                player2Score = 0;
                gameState = StartMenu;
            }

            BeginDrawing();
            ClearBackground(DARKPURPLE);

            DrawBallWithTrail(ball, trailPositions, trailLength);
            DrawPaddleWithShadow(leftPaddle);
            DrawPaddleWithShadow(rightPaddle);

            ball.Draw();
            leftPaddle.Draw();
            rightPaddle.Draw();

            if (winnerText)
            {
                int textWidth = MeasureText(winnerText, 60);
                DrawText(winnerText, GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2 - 30, 60, YELLOW);
            }
            if (restartGame)
            {
                int textWidth = MeasureText(restartGame, 30);
                DrawText(restartGame, GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2 + 80, 30, LIGHTGRAY);
            }

            DrawFPS(10, 10);

             snprintf(buffer, sizeof(buffer), "%d - %d", player1Score, player2Score);
            DrawText(buffer, GetScreenWidth() / 2 - MeasureText("0 - 0", 20) / 2, 10, 20, WHITE);

            DrawText("Score Limit:", 10, GetScreenHeight() - 50, 20, WHITE);
            DrawText("5 ", 130, GetScreenHeight() - 50, (scoreLimit == ScoreLimit5) ? 30 : 20, (scoreLimit == ScoreLimit5) ? YELLOW : WHITE);
            DrawText("10", 190, GetScreenHeight() - 50, (scoreLimit == ScoreLimit10) ? 30 : 20, (scoreLimit == ScoreLimit10) ? YELLOW : WHITE);
            DrawText("15", 250, GetScreenHeight() - 50, (scoreLimit == ScoreLimit15) ? 30 : 20, (scoreLimit == ScoreLimit15) ? YELLOW : WHITE);
            DrawText("20", 310, GetScreenHeight() - 50, (scoreLimit == ScoreLimit20) ? 30 : 20, (scoreLimit == ScoreLimit20) ? YELLOW : WHITE);

            EndDrawing();
            break;
        }
    }

    CloseWindow();

    return 0;
}