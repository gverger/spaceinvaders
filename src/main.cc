#include "spaceinvaders/level.h"
#include "spaceinvaders/pause.h"
#include "spaceinvaders/screens.h"
#include "spaceinvaders/start.h"
#include "spaceinvaders/states.h"
#include "raylib.h"

enum GameState {
  Running,
  Paused,
  Menu,
};
GameState gameState = GameState::Menu;

void PauseGame() { gameState = Paused; }

void ResumeGame() { gameState = Running; }

void GoToMenu() { gameState = Menu; }

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Space Invaders");
  SetExitKey(0);

  SetTargetFPS(60);

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    switch (gameState) {
    case Running:
      UpdateLevel();
      break;
    case Paused:
      UpdatePause();
      break;
    case Menu:
      UpdateStart();
      break;
    default:
      break;
    }

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(BLACK);
    DrawScreen();

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
