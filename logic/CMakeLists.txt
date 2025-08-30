add_library(logic)

target_sources(logic
  PUBLIC
    FILE_SET logic_header
    TYPE HEADERS
    BASE_DIRS ./include
    FILES
      include/logic/state_machine.h
      include/logic/state.h
  PRIVATE
    state_machine.cc
)
