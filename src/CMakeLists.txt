add_executable(${PROJECT_NAME}
  cannon.cc
  invaders.cc
  level.cc
  main.cc
  menu.cc
  pause.cc
  screens.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    raylib
    graphics
    logic
)

install(TARGETS ${PROJECT_NAME})
