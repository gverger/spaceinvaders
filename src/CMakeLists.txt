add_executable(${PROJECT_NAME}
  level.cc
  main.cc
  pause.cc
  screens.cc
  menu.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} raylib)

install(TARGETS ${PROJECT_NAME})
