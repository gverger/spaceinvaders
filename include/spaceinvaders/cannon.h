#pragma once

#include "elements.h"
#include "logic/state_machine.h"
#include <functional>
#include <vector>

enum BulletState {
  Alive,
  Exploding,
  Dead,
};

struct Bullet {
  MovingElement element;

  BulletState state;
};

class BulletSpawner {
public:
  virtual ~BulletSpawner() {};

  virtual void SpawnBullet(Bullet &) = 0;
};

class Cannon {
public:
  Cannon(std::function<void(Bullet &)> spawn_bullet, int screen_width);

  void Init(const MovingElement &starting_position);

  void Update(float dt);

  void ShootLeft();
  void ShootRight();

  const MovingElement &Element() { return element; }

private:
  StateMachine cannon_side;
  MovingElement element;
  std::function<void(Bullet &)> shoot;
  int screen_width;
};
