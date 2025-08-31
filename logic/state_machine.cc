#include "logic/state_machine.h"

#include <assert.h>

void StateMachine::Add(StateID state_id, std::unique_ptr<State> state) {
  assert(!states.contains(state_id));

  states[state_id] = std::move(state);
};

void StateMachine::Change(StateID new_state_id) {
  assert(states.contains(new_state_id));
  assert(states.contains(current));

  states[current]->Exit();
  current = new_state_id;
  states[current]->Enter();
}

void StateMachine::Start(StateID start_state_id) {
  assert(states.contains(start_state_id));

  current = start_state_id;
  states[current]->Enter();
}

void StateMachine::HandleInput() { states[current]->HandleInput(); }

void StateMachine::Update(float dt) {
  assert(states.contains(current));
  states[current]->Update(dt);
}
