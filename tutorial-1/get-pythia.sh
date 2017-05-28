#!/bin/bash

# the name and version number for Pythia
prog=pythia8226

# download it, with wget (if it's around) or curl
wget http://home.thep.lu.se/~torbjorn/pythia8/$prog.tgz \
    || curl http://home.thep.lu.se/~torbjorn/pythia8/$prog.tgz -o $prog.tgz

# unpack it
tar zxvf $prog.tgz

# build it
cd $prog
./configure
make -j

