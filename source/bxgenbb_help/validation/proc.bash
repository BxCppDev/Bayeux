#!/usr/bin/env bash

g++ $(boost-config --cflags)  $(root-config --cflags) $(datatools-config --cflags) validate.cxx -o validate $(datatools-config --ldflags) $(root-config --libs) -Wl,--no-as-needed -lHist -lCore -lMathCore