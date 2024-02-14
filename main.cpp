#include <array>
#include <iostream>

#include "raylib.h"

struct Ball {
  Rectangle m_rect;
  Vector2 m_velocity;

  Ball() : m_rect({0, 0, 20, 20}), m_velocity({0, 0}) {}
  Ball(Rectangle rect, Vector2 velocity) : m_rect(rect), m_velocity(velocity) {}
};

struct Paddle {
  Rectangle m_rect;
  Vector2 m_velocity;

  Paddle() : m_rect({0, 0, 100, 20}), m_velocity({0, 0}) {}
  Paddle(Rectangle rect, Vector2 velocity)
      : m_rect(rect), m_velocity(velocity) {}
};

int main() {
  // Initialization
  //--------------------------------------------------------------------------------------

  const int screenWidth = 600;
  const int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "PONG");

  // create a ball object with collision box
  Ball ball;
  ball.m_rect.x = screenWidth / 2 - ball.m_rect.width / 2;
  ball.m_rect.y = screenHeight / 2 - ball.m_rect.height / 2;
  ball.m_velocity.x = 0; // GetRandomValue(-3, 3);
  ball.m_velocity.y = (GetRandomValue(0, 1) == 0) ? -10 : 10;

  // create a paddle object with collision box
  Paddle paddle1;
  paddle1.m_rect.x = screenWidth / 2 - paddle1.m_rect.width / 2;
  paddle1.m_rect.y = 20;

  // create a paddle object with collision box
  Paddle paddle2;
  paddle2.m_rect.x = screenWidth / 2 - paddle2.m_rect.width / 2;
  paddle2.m_rect.y = screenHeight - 20 - paddle2.m_rect.height;

  // create a 2d camera object
  Camera2D camera = {{screenWidth / 2.0f, screenHeight / 2.0f},
                     {screenWidth / 2.0f, screenHeight / 2.0f},
                     0,
                     1};

  // create collision barriers on edges of screen
  Rectangle top = {0, screenHeight - 10, screenWidth, 10};
  Rectangle bottom = {0, 0, screenWidth, 10};
  Rectangle left = {0, 0, 10, screenHeight};
  Rectangle right = {screenWidth - 10, 0, 10, screenHeight};

  // keep track of score with two size_t variables
  std::array<size_t, 2> score = {0};

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second

  //--------------------------------------------------------------------------------------

  // Start Title Screen
  // write "PONG" in the middle of the screen
  // write "Press Space to Start" below "PONG"
  // if space is pressed, start game

  while (!IsKeyPressed(KEY_SPACE)) {
    if (WindowShouldClose()) {
      CloseWindow();
      return 0;
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);

    // align text to center of screen and account for text size
    int textX = screenWidth / 2 - MeasureText("PONG", 100) / 2;
    int textY = screenHeight / 2 - 100 / 2;

    int textX2 = screenWidth / 2 - MeasureText("Press Space to Start", 20) / 2;

    DrawText("PONG", textX, textY, 100, BLACK);
    DrawText("Press Space to Start", textX2, textY + 100, 20, BLACK);

    EndDrawing();
  }

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC// key
  {

    // Update
    //----------------------------------------------------------------------------------

    if (IsKeyDown(KEY_RIGHT)) {
      paddle2.m_rect.x += 7;
    }

    if (IsKeyDown(KEY_LEFT)) {
      paddle2.m_rect.x -= 7;
    }

    if (IsKeyDown(KEY_D)) {
      paddle1.m_rect.x += 7;
    }

    if (IsKeyDown(KEY_A)) {
      paddle1.m_rect.x -= 7;
    }

    // prevent the paddles from going off the screen
    if (paddle1.m_rect.x < 0) {
      paddle1.m_rect.x = 0;
    }

    if (paddle1.m_rect.x > screenWidth - paddle1.m_rect.width) {
      paddle1.m_rect.x = screenWidth - paddle1.m_rect.width;
    }

    if (paddle2.m_rect.x < 0) {
      paddle2.m_rect.x = 0;
    }

    if (paddle2.m_rect.x > screenWidth - paddle2.m_rect.width) {
      paddle2.m_rect.x = screenWidth - paddle2.m_rect.width;
    }

    // move the ball

    ball.m_rect.x += ball.m_velocity.x;
    ball.m_rect.y += ball.m_velocity.y;

    // check for collision with the top and bottom of the screen
    bool topCollision = CheckCollisionRecs(ball.m_rect, top);
    bool bottomCollision = CheckCollisionRecs(ball.m_rect, bottom);

    // win condition
    if (topCollision) {
      score[1]++;

      // reset the ball to the center of the screen and give it a random
      // velocity
      ball.m_rect.x = screenWidth / 2 - ball.m_rect.width / 2;
      ball.m_rect.y = screenHeight / 2 - ball.m_rect.height / 2;
      ball.m_velocity.x = GetRandomValue(-3, 3);
      ball.m_velocity.y = (GetRandomValue(0, 1) == 0) ? -5 : 5;
    }

    if (bottomCollision) {
      score[0]++;

      // reset the ball to the center of the screen and give it a random
      // velocity
      ball.m_rect.x = screenWidth / 2 - ball.m_rect.width / 2;
      ball.m_rect.y = screenHeight / 2 - ball.m_rect.height / 2;
      ball.m_velocity.x = GetRandomValue(-3, 3);
      ball.m_velocity.y = (GetRandomValue(0, 1) == 0) ? -5 : 5;
    }

    // check for collision with the walls of the screen
    if (CheckCollisionRecs(ball.m_rect, left) ||
        CheckCollisionRecs(ball.m_rect, right)) {
      ball.m_velocity.x *= -1;
    }

    bool paddle1Collision = CheckCollisionRecs(ball.m_rect, paddle1.m_rect);
    bool paddle2Collision = CheckCollisionRecs(ball.m_rect, paddle2.m_rect);

    // check for collision with the paddles
    if (paddle1Collision || paddle2Collision) {
      // reverse the y velocity
      ball.m_velocity.y *= -1;

      // find point of contact on the paddle
      double contactPoint =
          ball.m_rect.x -
          (paddle1Collision ? paddle1.m_rect.x : paddle2.m_rect.x);

      ball.m_velocity.x =
          std::min((contactPoint - (paddle1.m_rect.width / 2)) / 10, 4.0);
    }

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    BeginMode2D(camera);

    ClearBackground(RAYWHITE);

    // draw the ball round => DrawRectangleRounded(ball.m_rect, 1, 10, BLACK);
    DrawRectangleRec(ball.m_rect, BLACK);
    // draw the paddles
    DrawRectangleRec(paddle1.m_rect, BLACK);
    DrawRectangleRec(paddle2.m_rect, BLACK);

    // draw the collision barriers
    DrawRectangleRec(top, BLACK);
    DrawRectangleRec(bottom, BLACK);
    DrawRectangleRec(left, BLACK);
    DrawRectangleRec(right, BLACK);

    // draw the score board on the right side of the screen with the score for
    // score[0] on the bottom and score[1] on the top
    DrawText(std::to_string(score[1]).c_str(), screenWidth - 50,
             (screenHeight / 2) - 50, 20, BLACK);
    DrawText(std::to_string(score[0]).c_str(), screenWidth - 50,
             (screenHeight / 2) + 50, 20, BLACK);

    EndMode2D();

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  return 0;
}
