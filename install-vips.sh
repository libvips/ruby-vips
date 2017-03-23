#!/bin/bash 

vips_site=http://www.vips.ecs.soton.ac.uk/supported
version=$VIPS_VERSION_MAJOR.$VIPS_VERSION_MINOR
version_full=$VIPS_VERSION.$VIPS_VERSION_MICRO

set -e

# do we already have the correct vips built? early exit if yes
# we could check the configure params as well I guess
if [ -d "$HOME/vips/bin" ]; then
	installed_version=$($HOME/vips/bin/vips --version)
	escaped_version="$VIPS_VERSION_MAJOR\.$VIPS_VERSION_MINOR\.$VIPS_VERSION_MICRO"
	echo "Need vips-$version_fulL"
	echo "Found $installed_version"
	if [[ "$installed_version" =~ ^vips-$escaped_version ]]; then
		echo "Using cached directory"
		exit 0
	fi
fi

rm -rf $HOME/vips
wget $vips_site/$version/vips-$version_full.tar.gz
tar xf vips-$version_full.tar.gz
cd vips-$version_full
CXXFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0 ./configure --prefix=$HOME/vips $*
make && make install
