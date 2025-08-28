#include "spaceinvaders/level.h"
#include "spaceinvaders/pause.h"
#include "spaceinvaders/screens.h"
#include "spaceinvaders/states.h"

class MenuScreen : public Screen {
  void Draw() override {
    Dim(0.8);

    DrawCenteredText(40, "Space Invaders", 40, BLUE);
    DrawCenteredText(105, "PRESS ENTER", 60, RED);
  }

private:
};

MenuScreen Start;

void UpdateStart() {
  SetActiveScreen(&Start);

  if (IsKeyPressed(KEY_ENTER)) {
    InitLevel();
    ResumeGame();
  }
}
