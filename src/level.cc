#include "spaceinvaders/level.h"

#include "graphics/sprites.h"
#include "raylib.h"
#include "raymath.h"
#include "spaceinvaders/cannon.h"
#include "spaceinvaders/invaders.h"
#include "spaceinvaders/screens.h"
#include "spaceinvaders/states.h"
#include <format>
#include <memory>
#include <vector>

const int DOWN_DISTANCE = 32;
const float INVADER_SPEED = 40;

enum InvaderDirection {
  Right,
  DownThenRight,
  DownThenLeft,
  Left,
};

Rectangle translateRec(Rectangle rec, Vector2 offset) {
  return {
      .x = rec.x + offset.x,
      .y = rec.y + offset.y,
      .width = rec.width,
      .height = rec.height,
  };
}

class Level : public Screen {

public:
  Level() {}

  ~Level() {
    UnloadTexture(cannonTexture);
    UnloadTexture(invaderTexture);
    UnloadTexture(bulletTexture);
  }

  void Draw() override {

    auto element = cannon->Element();
    DrawTexturePro(cannonTexture, {0, 0, 32, 32},
                   {element.position.x + 32, element.position.y + 32,
                    element.size.x, element.size.y},
                   {32, 32}, (element.velocity.x / 60) * PI / 2, WHITE);

    for (auto invader : invader_group.Invaders()) {
      invaderAnimation->DrawPro(invader.position, invader.size, 0, WHITE);
    }

    for (auto bullet : bullets) {
      const MovingElement &element = bullet.element;
      bulletAnimation->DrawPro(element.position, element.size, 0, WHITE);
    }

    drawHUD();

    DrawText(std::format("BALLS: {}", bullets.size()).c_str(), 10, 100, 10,
             RED);
  };

  void Init() {
    if (!IsTextureValid(cannonTexture)) {
      cannonTexture = LoadTexture("assets/cannon.png");
    }
    if (!IsTextureValid(invaderTexture)) {
      invaderTexture = LoadTexture("assets/alien-1.png");
    }
    if (!IsTextureValid(bulletTexture)) {
      bulletTexture = LoadTexture("assets/bullet.png");
    }

    dead = false;
    if (cannon == nullptr) {
      cannon = std::make_unique<Cannon>(
          [this](Bullet &b) { return spawnBullet(b); }, Width());
    }
    cannon->Init({
        .position = Vector2{float(Width() - 64) / 2, float(Height()) - 72},
        .size = Vector2{64, 64},
        .velocity = Vector2Zero(),
    });

    invaderAnimation = std::make_unique<AnimatedSprite>(invaderTexture, 4, 4);
    bulletAnimation = std::make_unique<AnimatedSprite>(bulletTexture, 4, 15);

    invader_group.Init(Width());

    score = 0;
    lives = 3;
  }

  void Update() {
    if (dead) {
      return;
    }

    updateCannon();
    updateBullets();
    bulletAnimation->Update();
    updateInvaders();
    invaderAnimation->Update();
  }

  bool Lost() { return dead; }

  void Unload() { UnloadTexture(cannonTexture); }

private:
  Texture2D cannonTexture;
  Texture2D invaderTexture;
  Texture2D bulletTexture;

  std::unique_ptr<Cannon> cannon;

  std::unique_ptr<AnimatedSprite> invaderAnimation;
  InvaderGroup invader_group;

  std::unique_ptr<AnimatedSprite> bulletAnimation;
  std::vector<Bullet> bullets;

  int score;
  int lives;

  void updateCannon() { cannon->Update(GetFrameTime()); }

  void updateBullets() {
    for (auto &bullet : bullets) {
      bullet.element.position += bullet.element.velocity * GetFrameTime();

      if (bullet.element.position.y < -bullet.element.size.y) {
        bullet.state = Dead;
        continue;
      }

      auto bullet_rec =
          translateRec(bullet.element.collision_rec, bullet.element.position);

      for (auto &invader : invader_group.Invaders()) {
        if (!invader.alive) {
          continue;
        }
        auto invader_rec =
            translateRec(invader.collision_rec, invader.position);

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

  void updateInvaders() { invader_group.Update(GetFrameTime()); }

  void spawnBullet(Bullet &b) { bullets.push_back(b); }

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
