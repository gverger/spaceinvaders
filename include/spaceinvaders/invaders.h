#pragma once

#include "elements.h"
#include "logic/state_machine.h"
#include <functional>
#include <vector>

struct Invader {
  Vector2 position;
  Vector2 size;
  Rectangle collision_rec;

  bool alive;
};

class InvaderGroup {
public:
  InvaderGroup(int screen_width) : screen_width(screen_width) {}
  InvaderGroup() : screen_width(0) {}

  void Init();

  void Update(float dt);

  void Init(int screen_width);

  void SetVelocity(Vector2 velocity) { this->velocity = velocity; }
  Vector2 Velocity() { return velocity; }

  float Speed() { return speed; }
  void Accelerate() { speed += 20; }
  int ScreenWidth() { return screen_width; }

  std::vector<Invader> &Invaders() { return invaders; }

private:
  StateMachine movement;
  int screen_width;
  Vector2 velocity;
  std::vector<Invader> invaders;
  float speed;
};
