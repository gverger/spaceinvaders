#include "spaceinvaders/screens.h"
#include "raylib.h"

Screen *ActiveScreen = nullptr;

void SetActiveScreen(Screen *screen) { ActiveScreen = screen; }

void Screen::Dim(float alpha) {
  DimColor(BLACK, alpha);
}

void Screen::DimColor(Color col, float alpha) {
  DrawRectangle(0, 0, GetScreenWidth(), GetRenderHeight(),
                ColorAlpha(col, alpha));
}

void Screen::DrawCenteredText(int y, const char *text, int fontSize,
                             Color color) {
  int textWidth = MeasureText(text, fontSize);
  DrawText(text, Width() / 2 - textWidth / 2, y - fontSize / 2, fontSize,
           color);
}

void DrawScreen() {
  if (ActiveScreen != nullptr) {
    ActiveScreen->Draw();
  }
}
