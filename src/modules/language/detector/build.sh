#!/bin/bash

perl build_detector.pl ../detector.cpp
gcc -o detector ../detector.cpp
