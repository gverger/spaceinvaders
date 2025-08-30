#pragma once

#include "raylib.h"

class AnimatedSprite {

public:
  AnimatedSprite(Texture2D texture, int nb_frames, int frame_speed)
      : texture(texture), nb_frames(nb_frames), frame_speed(frame_speed) {
        Reset();
      }

  void Update();

  void DrawPro(Vector2 position, Vector2 size, float rotation, Color tint);

  void Reset();

private:
  float time_in_current;
  int nb_frames;
  int frame_speed; // nb images per second
  int current_frame;
  int texture_width;
  Texture2D texture;
};
