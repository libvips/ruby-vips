#!/bin/sh 

set -e
if [ ! -d "$HOME/vips/lib" ]; then
	wget $VIPS_SITE/$VIPS_MAJOR.$VIPS_MINOR/vips-$VIPS_VERSION.tar.gz
	tar xvf vips-$VIPS_VERSION.tar.gz
	cd vips-$VIPS_VERSION
	CXXFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0 ./configure --prefix=$HOME/vips 
	make && make install
else
	echo 'Using cached directory.'
fi
