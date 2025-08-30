#pragma once

#include "state.h"
#include <memory>
#include <map>

class StateMachine {
public:
  void Add(StateID state_id, std::unique_ptr<State> state);

  void Change(StateID new_state_id);

  void Update(float dt);

  void HandleInput();

private:
  StateID current;
  std::map<StateID, std::unique_ptr<State>> states;
  StateID last_state_id = -1;
};
