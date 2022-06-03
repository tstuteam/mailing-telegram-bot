#!/bin/bash

set -xe

g++ src/main.cpp src/lib.cpp -o bot	\
    --std=c++17 -I/usr/local/include -Wall -Wextra \
    -lTgBot -lboost_system -lssl -lcrypto -lpthread \
    -O2
