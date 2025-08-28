# 20 Games Challenge: Space Invaders

Part of the [20 Games Challenge](https://20_games_challenge.gitlab.io/challenge/).

Breakout made with Raylib.

# Run

Runs on Linux. Feel free to run it on Windows, but you're on your own...
Ideally with direnv and nix:
```bash

# When entering the folder, direnv triggers the nix flake which install the required dependencies
cd spaceinvaders
direnv allow # at first time use

just clean # remove ./install and ./build for a clean install
just init # create the ./build directory
just install # build and install the binary in ./install
just run # run the installed binary

just clean init install run # all of the above in the same run
```

# Play the game

Use H and L for moving left and right
