#include "logic/state_machine.h"

#include <assert.h>

void StateMachine::Add(StateID state_id, std::unique_ptr<State> state) {
  assert(!states.contains(state_id));
  states[state_id] = std::move(state);
};

void StateMachine::Change(StateID new_state_id) {
  assert(states.contains(new_state_id));

  current = new_state_id;
}

void StateMachine::HandleInput() { states[current]->HandleInput(); }

void StateMachine::Update(float dt) {
  states[current]->Update(dt);
}
