add_executable(${PROJECT_NAME}
  level.cc
  main.cc
  pause.cc
  screens.cc
  menu.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    raylib
    graphics
)

install(TARGETS ${PROJECT_NAME})
