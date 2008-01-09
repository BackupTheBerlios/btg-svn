#!/bin/sh

# requires cvs2cl.pl
# Used to generate a changelog file.

rm -f Changelog

../../tools/cvs2cl.pl -F devel-branch --hide-filenames --prune --lines-modified --FSF --ignore "Makefile*" --ignore ChangeLog --ignore "configure"
