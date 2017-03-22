#!/bin/sh 

set -e
if [ ! -d "$HOME/vips/lib" ]; then
	wget $VIPS_SITE/$VIPS_VERSION/vips-$VIPS_VERSION_FULL.tar.gz
	tar xf vips-$VIPS_VERSION_FULL.tar.gz
	cd vips-$VIPS_VERSION_FULL
	CXXFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0 ./configure --prefix=$HOME/vips 
	make && make install
else
	echo 'Using cached directory.'
fi
