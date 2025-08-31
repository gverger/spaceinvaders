#include "spaceinvaders/invaders.h"

#include "raymath.h"

const float INVADER_SPEED = 40;
const float DOWN_DISTANCE = 32;

const int MovingLeftID = 1;
const int MovingRightID = 2;
const int MovingDownLeftSideID = 3;
const int MovingDownRightSideID = 4;

class MovingLeft : public State {
public:
  MovingLeft(StateMachine &state_machine, InvaderGroup &group)
      : movement_state(state_machine), group(group) {}

  void Update(float dt) override {
    Vector2 frame_velocity = group.Velocity() * dt;

    for (auto &invader : group.Invaders()) {
      if (!invader.alive) {
        continue;
      }
      if (invader.position.x < 0) {
        movement_state.Change(MovingDownLeftSideID);
        return;
      }
    }
  }

  void Enter() override {
    group.SetVelocity({-(group.Speed() + INVADER_SPEED), 0});
  }

private:
  StateMachine &movement_state;
  InvaderGroup &group;
};

class MovingRight : public State {
public:
  MovingRight(StateMachine &state_machine, InvaderGroup &group)
      : movement_state(state_machine), group(group) {}

  void Update(float dt) override {
    Vector2 frame_velocity = group.Velocity() * dt;

    for (auto &invader : group.Invaders()) {
      if (!invader.alive) {
        continue;
      }
      if (invader.position.x + invader.size.x > group.ScreenWidth()) {
        movement_state.Change(MovingDownRightSideID);
        return;
      }
    }
  }

  void Enter() override {
    group.SetVelocity({group.Speed() + INVADER_SPEED, 0});
  }

private:
  StateMachine &movement_state;
  InvaderGroup &group;
};

class MovingDownLeftSide : public State {
public:
  MovingDownLeftSide(StateMachine &state_machine, InvaderGroup &group)
      : movement_state(state_machine), group(group) {}

  void Update(float dt) override {
    Vector2 frame_velocity = group.Velocity() * dt;
    distance += frame_velocity.y;

    if (distance > DOWN_DISTANCE) {
      movement_state.Change(MovingRightID);
      return;
    }
  }

  void Enter() override {
    distance = 0;
    group.SetVelocity({0, group.Speed() + INVADER_SPEED});
  }

  void Exit() override { group.Accelerate(); }

private:
  StateMachine &movement_state;
  InvaderGroup &group;
  float distance;
};

class MovingDownRightSide : public State {
public:
  MovingDownRightSide(StateMachine &state_machine, InvaderGroup &group)
      : movement_state(state_machine), group(group) {}

  void Update(float dt) override {
    Vector2 frame_velocity = group.Velocity() * dt;
    distance += frame_velocity.y;

    if (distance > DOWN_DISTANCE) {
      movement_state.Change(MovingLeftID);
      return;
    }
  }

  void Enter() override {
    distance = 0;
    group.SetVelocity({0, group.Speed() + INVADER_SPEED});
  }

  void Exit() override { group.Accelerate(); }

private:
  StateMachine &movement_state;
  InvaderGroup &group;
  float distance;
};

void InvaderGroup::Update(float dt) {
  Vector2 frame_velocity = velocity * dt;

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

  movement.Update(dt);
}

void InvaderGroup::Init(int screen_width) {
  this->screen_width = screen_width;
  this->speed = 0;

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

  movement.Add(MovingRightID, std::make_unique<MovingRight>(movement, *this));
  movement.Add(MovingLeftID, std::make_unique<MovingLeft>(movement, *this));
  movement.Add(MovingDownLeftSideID,
               std::make_unique<MovingDownLeftSide>(movement, *this));
  movement.Add(MovingDownRightSideID,
               std::make_unique<MovingDownRightSide>(movement, *this));

  movement.Start(MovingRightID);
}
