#!/bin/bash

#generate detecting routines and tables
perl build_detector.pl ../detector.cpp

#compile detector.cpp as a standalone app (only for testing)
#g++ -o detector ../detector.cpp
