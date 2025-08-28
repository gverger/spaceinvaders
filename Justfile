default: install

run:
  ./install/bin/space-invaders

install: compile
  cmake --install build --config Release --prefix install

alias build:=compile
compile:
  cmake --build build --config Release --parallel 8

init:
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

clean:
  rm -rf build install
