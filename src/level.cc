#include "spaceinvaders/level.h"
#include "raylib.h"
#include "raymath.h"
#include "spaceinvaders/screens.h"
#include "spaceinvaders/states.h"
#include <format>
#include <memory>
#include <vector>

const int MAX_VELOCITY = 500;
const int START_BALL_SPEED = 3;
const int MAX_BALL_SPEED = 10;
const float BALL_ACCELERATION_AFTER_BRICK = 1.02;
const int DEATH_TIMER = 120;

struct MovingElement {
  Vector2 position;
  Vector2 size;
  Vector2 velocity;
};

struct Invader {
  Vector2 position;
  Vector2 size;
  Vector2 velocity;

  bool alive;
};

class AnimatedTexture {

public:
  AnimatedTexture(const char *image_file, int nb_frames, int frame_speed)
      : texture(LoadTexture(image_file)), nb_frames(nb_frames),
        frame_speed(frame_speed) {}

  ~AnimatedTexture() { UnloadTexture(texture); }

  void Update() {
    frame_counter++;
    if (frame_counter >= 60 / frame_speed) {
      frame_counter = 0;
      current_frame++;
      if (current_frame >= nb_frames) {
        current_frame = 0;
      }
    }
  }

  void DrawPro(Vector2 position, Vector2 size, float rotation, Color tint) {
    DrawTexturePro(
        texture,
        {float(current_frame * 16), 0, float(texture.height),
         float(texture.height)},
        {position.x + size.x / 2, position.y + size.y / 2, size.x, size.y},
        {size.x / 2, size.y / 2}, rotation, tint);
  }

  void Init() {
    frame_counter = 0;
    current_frame = 0;
  }

private:
  int frame_counter;
  int nb_frames;
  int frame_speed; // nb images per second
  int current_frame;
  Texture2D texture;
};

class Level : public Screen {

public:
  void Draw() override {

    DrawTexturePro(cannonTexture, {0, 0, 32, 32},
                   {cannon.position.x + 32, cannon.position.y + 32,
                    cannon.size.x, cannon.size.y},
                   {32, 32}, (cannon.velocity.x / 60) * PI / 2, WHITE);

    for (auto invader : invaders) {
      if (!invader.alive) {
        continue;
      }

      invaderTexture->DrawPro(invader.position, invader.size, 0, WHITE);
    }

    drawHUD();
  };

  void Init() {
    dead = false;
    cannon.size = Vector2{64, 64};
    cannon.position =
        Vector2{float(Width() - cannon.size.x) / 2, float(Height()) - 72};
    cannon.velocity = Vector2Zero();

    cannonTexture = LoadTexture("assets/cannon.png");

    invaderTexture =
        std::make_unique<AnimatedTexture>("assets/alien-1.png", 4, 4);
    invaderTexture->Init();

    for (size_t i = 0; i < 16; i++) {
      for (size_t j = 0; j < 5; j++) {
        invaders.push_back({
            .position = {float(20 + i * 40), float(20 + j * 40)},
            .size = {32, 32},
            .velocity = {1, 0},
            .alive = true,
        });
      }
    }

    score = 0;
    lives = 3;
  }

  void Update() {
    if (dead) {
      return;
    }

    updateCannon();
    invaderTexture->Update();
  }

  bool Lost() { return dead; }

  void Unload() { UnloadTexture(cannonTexture); }

private:
  MovingElement cannon;
  MovingElement ball;
  std::vector<Invader> invaders;

  Texture2D cannonTexture;
  std::unique_ptr<AnimatedTexture> invaderTexture;

  int score;
  int lives;

  void updateCannon() {
    bool key_down = false;
    if (IsKeyDown(KEY_H)) {
      key_down = true;
      cannon.velocity.x -= 30;
    }

    if (IsKeyDown(KEY_L)) {
      key_down = true;
      cannon.velocity.x += 30;
    }

    if (!key_down) {
      cannon.velocity = cannon.velocity * 0.9;
    }

    cannon.velocity.x = Clamp(cannon.velocity.x, -MAX_VELOCITY, MAX_VELOCITY);

    cannon.position.x += cannon.velocity.x * GetFrameTime();
    cannon.position.x = Clamp(cannon.position.x, 0, Width() - cannon.size.x);

    if (cannon.position.x == 0 ||
        cannon.position.x == Width() - cannon.size.x) {
      cannon.velocity.x = -cannon.velocity.x / 4;
    }
  }

  void updateBall() {
    ball.position += ball.velocity;

    // Bouncing against the wall
    float right_wall = Width() - ball.size.x;
    if (ball.position.x > right_wall) {
      ball.position.x = 2 * right_wall - ball.position.x;
      ball.velocity.x = -ball.velocity.x;
    }

    if (ball.position.x < 0) {
      ball.position.x = -ball.position.x;
      ball.velocity.x = -ball.velocity.x;
    }

    float bottom_wall = Height() - ball.size.y;
    if (ball.position.y > bottom_wall) {
      ball.position.y = 2 * bottom_wall - ball.position.y;
      ball.velocity.y = -ball.velocity.y;
      dying = true;
    }

    if (ball.position.y < 0) {
      ball.position.y = -ball.position.y;
      ball.velocity.y = -ball.velocity.y;
    }

    // Bouncing against the paddle
    ballCollision = GetCollisionRec(
        Rectangle{cannon.position.x, cannon.position.y, cannon.size.x,
                  cannon.size.y},
        Rectangle{ball.position.x, ball.position.y, ball.size.x, ball.size.y});

    if (ballCollision.height != 0) {

      float velocity_len = Vector2Length(ball.velocity);

      bool collide_top =
          ballCollision.y == cannon.position.y && ball.velocity.y > 0;
      bool collide_down = ballCollision.y + ballCollision.height ==
                              cannon.position.y + cannon.size.y &&
                          ball.velocity.y < 0;
      if (collide_top || collide_down) {
        ball.velocity.y = -ball.velocity.y;
        ball.velocity.x += cannon.velocity.x / 4;
      }

      bool collide_left =
          ballCollision.x == cannon.position.x && ball.velocity.x > 0;
      bool collide_right = ballCollision.x + ballCollision.width ==
                               cannon.position.x + cannon.size.x &&
                           ball.velocity.x < 0;
      if (collide_left || collide_right) {
        ball.velocity.x = -ball.velocity.x;
      }

      ball.velocity = Vector2Normalize(ball.velocity) * velocity_len;
    }

    // Bouncing against the bricks
    for (auto &alien : invaders) {
      if (!alien.alive) {
        continue;
      }
      ballCollision =
          GetCollisionRec(Rectangle{alien.position.x, alien.position.y,
                                    alien.size.x, alien.size.y},
                          Rectangle{ball.position.x, ball.position.y,
                                    ball.size.x, ball.size.y});

      if (ballCollision.height != 0) {
        bool collide_top =
            ballCollision.y == alien.position.y && ball.velocity.y > 0;
        bool collide_down = ballCollision.y + ballCollision.height ==
                                alien.position.y + alien.size.y &&
                            ball.velocity.y < 0;
        if (collide_top || collide_down) {
          ball.velocity.y = -ball.velocity.y;
        }

        bool collide_left =
            ballCollision.x == alien.position.x && ball.velocity.x > 0;
        bool collide_right = ballCollision.x + ballCollision.width ==
                                 alien.position.x + alien.size.x &&
                             ball.velocity.x < 0;
        if (collide_left || collide_right) {
          ball.velocity.x = -ball.velocity.x;
        }

        alien.alive = false;
        ball.velocity *= BALL_ACCELERATION_AFTER_BRICK;
        score++;
      }
    }

    ball.velocity = Vector2ClampValue(ball.velocity, 0.1, MAX_BALL_SPEED);
  }

  void drawHUD() {
    DrawCenteredText(20, "H: move left, L: move right", 20, GRAY);

    if (dying) {
      DrawText(std::format("LIVES: {}", lives).c_str(), 10, 5, 30, RED);
    } else {
      DrawText(std::format("LIVES: {}", lives).c_str(), 10, 5, 30, RAYWHITE);
    }

    DrawText(std::format("SCORE: {}", score).c_str(), Width() - 200, 5, 30,
             RAYWHITE);
  }

  Rectangle ballCollision;

  bool dying;
  bool dead;
  int death_timer;
};

Level Level;

void InitLevel() { Level.Init(); }

void UpdateLevel() {
  if (IsKeyPressed(KEY_SPACE)) {
    PauseGame();
    return;
  }

  if (Level.Lost()) {
    Level.Unload();
    GoToMenu();
    return;
  }

  SetActiveScreen(&Level);

  Level.Update();
}
