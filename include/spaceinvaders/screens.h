#pragma once

#include "raylib.h"

class Screen {
public:
  virtual void Draw() = 0;

  int Width() { return GetScreenWidth(); }

  int Height() { return GetScreenHeight(); }

protected:
  void Dim(float alpha);
  void DimColor(Color col, float alpha);
  void DrawCenteredText(int y, const char *text, int fontSize, Color color);
};

void SetActiveScreen(Screen *screen);

void DrawScreen();
