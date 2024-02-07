#!/bin/zsh
g++ $(pkg-config --cflags --libs opencv4) -std=c++11  $1 -o $2