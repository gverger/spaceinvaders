#include "spaceinvaders/pause.h"
#include "spaceinvaders/screens.h"
#include "spaceinvaders/states.h"

class PauseScreen : public Screen {
  void Draw() override {
    Dim(0.8);

    DrawCenteredText(40, "Space Invaders", 40, BLUE);
    DrawCenteredText(105, "Paused", 60, RED);
  }
};

PauseScreen Pause;

void UpdatePause() {
  SetActiveScreen(&Pause);

  if (IsKeyPressed(KEY_SPACE)) {
    ResumeGame();
  }
}
