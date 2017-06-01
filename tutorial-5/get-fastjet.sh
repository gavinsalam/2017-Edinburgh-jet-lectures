#!/bin/bash

#-- first get fastjet
fastjet=fastjet-3.2.2

fjurl=http://fastjet.fr/repo/$fastjet.tar.gz
wget $fjurl || curl $fjurl -o $fastjet.tar.gz

# unpack it
tar zxf $fastjet.tar.gz

# build it
cd $fastjet
fjprefix=`pwd`-install
./configure --prefix=$fjprefix
make -j install
cd ..


#--- now get fjcontrib
fjc=fjcontrib-1.026
fjcurl=http://fastjet.hepforge.org/contrib/downloads/$fjc.tar.gz
wget $fjcurl || curl $fjcurl -o $fjc.tar.gz
tar zxf $fjc.tar.gz

cd $fjc
./configure --fastjet-config=$fjprefix/bin/fastjet-config
make -j4 install
cd ..


