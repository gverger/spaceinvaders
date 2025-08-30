#pragma once

#include "raylib.h"

struct MovingElement {
  Vector2 position;
  Vector2 size;
  Vector2 velocity;
  Rectangle collision_rec;
};
