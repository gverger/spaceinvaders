#pragma once

#include "state.h"
#include <memory>
#include <map>

class StateMachine {
public:
  void Add(StateID state_id, std::unique_ptr<State> state);

  void Start(StateID start_state_id);

  void Change(StateID new_state_id);

  void Update(float dt);

  void HandleInput();

private:
  StateID current = -1;
  std::map<StateID, std::unique_ptr<State>> states;
};
