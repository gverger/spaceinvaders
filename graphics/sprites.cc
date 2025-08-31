#include "graphics/sprites.h"

#include <assert.h>

void AnimatedSprite::Update() {
  time_in_current += GetFrameTime();
  if (time_in_current >= 1. / frame_speed) {
    time_in_current = 0;
    current_frame++;
    if (current_frame >= nb_frames) {
      current_frame = 0;
    }
  }
}

void AnimatedSprite::DrawPro(Vector2 position, Vector2 size, float rotation,
                             Color tint) {
  DrawTexturePro(
      texture,
      {float(current_frame * texture_width), 0, float(texture_width),
       float(texture.height)},
      {position.x + size.x / 2, position.y + size.y / 2, size.x, size.y},
      {size.x / 2, size.y / 2}, rotation, tint);
}

void AnimatedSprite::Reset() {
  time_in_current = 0;
  current_frame = 0;
  texture_width = texture.width / nb_frames;
  assert(texture_width * nb_frames == texture.width);
}
