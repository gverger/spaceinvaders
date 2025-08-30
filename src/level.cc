#include "spaceinvaders/level.h"
#include "raylib.h"
#include "raymath.h"
#include "spaceinvaders/screens.h"
#include "spaceinvaders/states.h"
#include <format>
#include <iostream>
#include <memory>
#include <vector>

const int MAX_VELOCITY = 500;
const int START_BALL_SPEED = 3;
const int MAX_BALL_SPEED = 10;
const float BALL_ACCELERATION_AFTER_BRICK = 1.02;
const int DEATH_TIMER = 120;
const int DOWN_DISTANCE = 32;
const float INVADER_SPEED = 40;
const int BALL_SPEED = 200;

enum InvaderDirection {
  Right,
  DownThenRight,
  DownThenLeft,
  Left,
};

enum CannonSide {
  RightCannon,
  LeftCannon,
};

struct MovingElement {
  Vector2 position;
  Vector2 size;
  Vector2 velocity;
  Rectangle collision_rec;
};

enum BulletState {
  Alive,
  Exploding,
  Dead,
};

struct Bullet {
  MovingElement element;

  BulletState state;
};

struct Invader {
  Vector2 position;
  Vector2 size;
  Rectangle collision_rec;

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
      // auto invader_rec = Rectangle{
      //     invader.position.x + invader.collision_rec.x,
      //     invader.position.y + invader.collision_rec.y,
      //     invader.collision_rec.width,
      //     invader.collision_rec.height,
      // };
      // DrawRectangleLinesEx(invader_rec, 2, RED);
    }

    for (auto bullet : bullets) {
      // DrawRectangleLinesEx(
      //     {bullet.position.x, bullet.position.y, bullet.size.x,
      //     bullet.size.y}, 2, RED);
      const MovingElement &element = bullet.element;
      bulletTexture->DrawPro(element.position, element.size, 0, WHITE);
    }

    drawHUD();

    DrawText(std::format("BALLS: {}", bullets.size()).c_str(), 10, 100, 10,
             RED);
    // if (!bullets.empty()) {
    //   DrawText(std::format("BALL: {},{} {},{}", bullets[0].position.x,
    //                        bullets[0].position.y, bullets[0].size.x,
    //                        bullets[0].size.y)
    //                .c_str(),
    //            10, 120, 10, RED);
    // }
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

    bulletTexture =
        std::make_unique<AnimatedTexture>("assets/bullet.png", 4, 15);
    bulletTexture->Init();
    next_cannon = RightCannon;

    invaders_direction = Right;
    invaders_straight_distance = 0;
    invaders_velocity = {invaders_speed + INVADER_SPEED, 0};
    invaders_speed = 0;

    for (size_t i = 0; i < 16; i++) {
      for (size_t j = 0; j < 5; j++) {
        invaders.push_back({
            .position = {float(20 + i * 40), float(20 + j * 40)},
            .size = {32, 32},
            .collision_rec = {2, 4, 28, 16},
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
    updateBullets();
    bulletTexture->Update();
    updateInvaders();
    invaderTexture->Update();
  }

  bool Lost() { return dead; }

  void Unload() { UnloadTexture(cannonTexture); }

private:
  MovingElement cannon;
  std::vector<Invader> invaders;

  Texture2D cannonTexture;
  std::unique_ptr<AnimatedTexture> invaderTexture;
  Vector2 invaders_velocity;
  float invaders_speed;
  InvaderDirection invaders_direction;
  float invaders_straight_distance;

  std::unique_ptr<AnimatedTexture> bulletTexture;
  std::vector<Bullet> bullets;
  CannonSide next_cannon;

  int score;
  int lives;

  void updateCannon() {
    bool key_down = false;
    if (IsKeyDown(KEY_H) || IsKeyDown(KEY_LEFT)) {
      key_down = true;
      cannon.velocity.x -= 30;
    }

    if (IsKeyDown(KEY_L) || IsKeyDown(KEY_RIGHT)) {
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

    if (IsKeyPressed(KEY_F)) {
      shoot();
    }
  }

  void updateBullets() {
    for (auto &bullet : bullets) {
      bullet.element.position += bullet.element.velocity * GetFrameTime();

      if (bullet.element.position.y < -bullet.element.size.y) {
        bullet.state = Dead;
        continue;
      }

      auto bullet_rec = Rectangle{
          bullet.element.position.x + bullet.element.collision_rec.x,
          bullet.element.position.y + bullet.element.collision_rec.y,
          bullet.element.collision_rec.width,
          bullet.element.collision_rec.height,
      };

      for (auto &invader : invaders) {
        if (!invader.alive) {
          continue;
        }
        auto invader_rec = Rectangle{
            invader.position.x + invader.collision_rec.x,
            invader.position.y + invader.collision_rec.y,
            invader.collision_rec.width,
            invader.collision_rec.height,
        };
        auto col = GetCollisionRec(bullet_rec, invader_rec);
        if (col.height > 0 && col.width > 0) {
          invader.alive = false;
          bullet.state = Dead;
          score++;
          break;
        }
      }
    }

    int dead_bullets = 0;
    int last = bullets.size() - 1;
    for (size_t i = 0; i < bullets.size(); i++) {
      if (bullets[i].state != Dead) {
        continue;
      }

      bullets[i] = bullets.back();
      bullets.pop_back();
      dead_bullets++;
      last--;
      i--;
    }
  }

  void updateInvaders() {
    Vector2 frame_velocity = invaders_velocity * GetFrameTime();
    invaders_straight_distance += frame_velocity.y;

    for (size_t i = 0; i < invaders.size(); i++) {
      if (invaders[i].alive) {
        continue;
      }

      invaders[i] = invaders.back();
      invaders.pop_back();
      --i;
    }

    for (auto &invader : invaders) {
      invader.position = invader.position + frame_velocity;
    }

    if (invaders_direction == Right) {
      float max_x = 0;
      for (auto &invader : invaders) {
        if (!invader.alive) {
          continue;
        }
        float x = invader.position.x + invader.size.x;
        if (x > max_x) {
          max_x = x;
        }
      }

      if (max_x > Width()) {
        invaders_direction = DownThenLeft;
        invaders_velocity = {0, invaders_speed + INVADER_SPEED};
        invaders_straight_distance = 0;
      }
    } else if (invaders_direction == Left) {
      float min_x = Width();
      for (auto &invader : invaders) {
        if (!invader.alive) {
          continue;
        }
        if (invader.position.x < min_x) {
          min_x = invader.position.x;
        }
      }

      if (min_x < 0) {
        invaders_direction = DownThenRight;
        invaders_velocity = {0, invaders_speed + INVADER_SPEED};
        invaders_straight_distance = 0;
      }
    } else if (invaders_direction == DownThenRight) {

      if (invaders_straight_distance >= DOWN_DISTANCE) {
        invaders_direction = Right;
        invaders_speed += 20;
        invaders_velocity = {invaders_speed + INVADER_SPEED, 0};
        invaders_straight_distance = 0;
      }
    } else if (invaders_direction == DownThenLeft) {

      if (invaders_straight_distance >= DOWN_DISTANCE) {
        invaders_direction = Left;
        invaders_speed += 20;
        invaders_velocity = {-(invaders_speed + INVADER_SPEED), 0};
        invaders_straight_distance = 0;
      }
    }
  }

  void shoot() {
    int offset = -8;
    if (next_cannon == RightCannon) {
      offset = 8;
      next_cannon = LeftCannon;
    } else {
      next_cannon = RightCannon;
    }
    MovingElement mbullet = {
        .position = {cannon.position.x + cannon.size.x / 2 - 8 + offset,
                     cannon.position.y},
        .size = {16, 16},
        .velocity = {0, -BALL_SPEED},
        .collision_rec = {5, 0, 6, 16},
    };
    Bullet bullet = {
        .element = mbullet,
        .state = Alive,
    };

    bullets.push_back(bullet);
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
