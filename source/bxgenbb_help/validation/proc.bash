#!/usr/bin/env bash

g++ $(root-config --cflags) run.cxx -o run_test $(root-config --libs) -Wl,--no-as-needed -lHist -lCore -lMathCore