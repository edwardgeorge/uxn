#!/bin/sh

set -e

# build the thing
./build.sh --no-run

# more roms
./bin/uxncli bin/asma.rom projects/software/piano.tal bin/piano.rom

# get more roms
curl -L https://github.com/randrew/uxn32/releases/latest/download/uxn32-essentials.zip | bsdtar -xvf- -C bin
rm -f bin/*.exe
