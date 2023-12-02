#include <raylib.h>

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
    DrawRectangle(paddle.x - paddle.width / 2 + 4, paddle.y - paddle.height / 2 + 4, paddle.width, paddle.height, BLACK);
    DrawRectangleRec(paddle.GetRect(), WHITE);
}
void DrawBallWithShadow(Ball ball)
{
    DrawCircle(ball.x + 4, ball.y + 4, ball.radius, BLACK);
    DrawCircle(ball.x, ball.y, ball.radius, WHITE);
}

enum GameState
{
    StartMenu,
    Playing,
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

    const char* winnerText = nullptr;
    const char* restartGame = nullptr;

    GameState gameState = StartMenu;

    while (!WindowShouldClose())
    {
        switch (gameState)
        {
        case StartMenu:
            BeginDrawing();
            ClearBackground(DARKGRAY);

            DrawText("Pong - Start Menu", GetScreenWidth() / 2 - MeasureText("Pong - Start Menu", 30) / 2, GetScreenHeight() / 4, 30, WHITE);
            DrawText("Press Enter to Start", GetScreenWidth() / 2 - MeasureText("Press Enter to Start", 20) / 2, GetScreenHeight() / 2, 20, WHITE);

            EndDrawing();

            if (IsKeyPressed(KEY_ENTER))
            {
                gameState = Playing;
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
                winnerText = "Player 2 Wins!";
                restartGame = "press space to restart";
            }
            if (ball.x > GetScreenWidth())
            {
                winnerText = "Player 1 Wins!";
                restartGame = "press space to restart";
            }
            if (winnerText && IsKeyPressed(KEY_SPACE))
            {
                ball.x = GetScreenWidth() / 2;
                ball.y = GetScreenHeight() / 2;
                ball.speedX = 300;
                ball.speedY = 300;
                winnerText = nullptr;
                restartGame = nullptr;
            }

            BeginDrawing();
            ClearBackground(DARKGRAY);

            DrawBallWithShadow(ball);
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
            EndDrawing();
            break;
        }
    }

    CloseWindow();

    return 0;
}