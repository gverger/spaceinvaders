#include "spaceinvaders/cannon.h"

#include "logic/state.h"
#include "raymath.h"

const int KEY_FIRE = KEY_F;
const float BULLET_SPEED = 200;
const int MAX_VELOCITY = 500;

void Cannon::Update(float dt) {
  bool key_down = false;
  if (IsKeyDown(KEY_H) || IsKeyDown(KEY_LEFT)) {
    key_down = true;
    element.velocity.x -= 30;
  }

  if (IsKeyDown(KEY_L) || IsKeyDown(KEY_RIGHT)) {
    key_down = true;
    element.velocity.x += 30;
  }

  if (!key_down) {
    element.velocity = element.velocity * 0.9;
  }

  element.velocity.x = Clamp(element.velocity.x, -MAX_VELOCITY, MAX_VELOCITY);

  element.position.x += element.velocity.x * GetFrameTime();
  element.position.x =
      Clamp(element.position.x, 0, float(screen_width - element.size.x));

  if (element.position.x == 0 ||
      element.position.x == screen_width - element.size.x) {
    element.velocity.x = -element.velocity.x / 4;
  }

  cannon_side.Update(GetFrameTime());
}

inline Bullet prepare_bullet(MovingElement cannon, float offset_x) {
  return {
      .element =
          {
              .position = {cannon.position.x + cannon.size.x / 2 - 8 + offset_x,
                           cannon.position.y},
              .size = {16, 16},
              .velocity = {0, -BULLET_SPEED},
              .collision_rec = {5, 0, 6, 16},
          },
      .state = Alive,
  };
}

void Cannon::ShootRight() {
  Bullet b = prepare_bullet(element, 8);

  shoot(b);
}

void Cannon::ShootLeft() {
  Bullet b = prepare_bullet(element, -8);

  shoot(b);
}

const StateID LeftReady = 1;
const StateID RightReady = 2;

class LeftCannonReady : public State {
public:
  LeftCannonReady(StateMachine &state_machine, Cannon &cannon)
      : cannon(cannon), cannon_state(state_machine) {}
  void Update(float dt) override {
    if (!IsKeyPressed(KEY_FIRE)) {
      return;
    }

    cannon.ShootLeft();
    cannon_state.Change(RightReady);
  }

private:
  StateMachine &cannon_state;
  Cannon &cannon;
};

class RightCannonReady : public State {
public:
  RightCannonReady(StateMachine &state_machine, Cannon &cannon)
      : cannon(cannon), cannon_state(state_machine) {}

  void Update(float dt) override {
    if (!IsKeyPressed(KEY_FIRE)) {
      return;
    }

    cannon.ShootRight();
    cannon_state.Change(LeftReady);
  }

private:
  StateMachine &cannon_state;
  Cannon &cannon;
};

Cannon::Cannon(std::function<void(Bullet &)> spawn_bullet, int screen_width)
    : shoot(spawn_bullet), screen_width(screen_width) {
  cannon_side.Add(LeftReady,
                  std::make_unique<LeftCannonReady>(cannon_side, *this));
  cannon_side.Add(RightReady,
                  std::make_unique<RightCannonReady>(cannon_side, *this));
}

void Cannon::Init(const MovingElement &starting_position) {
  element = starting_position;
  cannon_side.Start(LeftReady);
}
