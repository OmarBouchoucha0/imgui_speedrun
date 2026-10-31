#!/bin/sh

gcc -DDEBUG_MODE=1 src/main.c -o out/app -g -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
