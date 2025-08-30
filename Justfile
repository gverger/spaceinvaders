default: install

run:
  ./install/bin/space-invaders

install: compile
  cmake --install build --config Debug --prefix install

alias build:=compile
compile:
  cmake --build build --config Debug --parallel 8

init:
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

clean:
  rm -rf build install
