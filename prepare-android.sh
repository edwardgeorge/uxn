#!/bin/sh

set -e

# need launcher.rom to be included
./build.sh --no-run
# duplicate? already have a piano
rm -f bin/asma-piano.rom

# get more roms
curl https://rabbits.srht.site/uxn-rompack.tar.gz | tar -C bin -xzf -
mv bin/uxn/*.rom bin
rm -r bin/uxn
