#!/bin/bash
make
./main > interp.dat
graph -T svg < interp.dat > interp.svg
xdg-open interp.svg;
