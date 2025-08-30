add_library(graphics)

target_sources(graphics
  PUBLIC
    FILE_SET graphics_header
    TYPE HEADERS
    BASE_DIRS ./include
    FILES include/graphics/sprites.h
  PRIVATE
    sprites.cc
)

target_link_libraries(graphics raylib)
