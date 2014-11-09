#!/bin/bash
cxxpath="/usr/bin/cxxtest-4.4/cxxtest"
wavepath="../SWE1D/src"

cxxtestgen --error-printer -o dim_split_test.cpp ./src/examples/dim_split_test.h

g++ -o dim_split_test -I$cxxpath -I$wavepath -iquotesrc dim_split_test.cpp

./dim_split_test
