#pragma once

typedef int StateID;

class State {
public:
  virtual ~State() {}

  virtual void Update(float dt) = 0;
  virtual void HandleInput() {};

  virtual void Enter() {};
  virtual void Exit() {};
};
