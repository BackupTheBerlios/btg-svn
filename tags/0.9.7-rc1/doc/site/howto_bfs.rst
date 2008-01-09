========================================================
Building Boost, Rasterbar Libtorrent and BTG from Source
========================================================

.. contents:: Table of Contents 
   :depth: 2

Introduction
============

This short howto describes building Boost, Rasterbar Libtorrent and
BTG from source. 

This procedure was contributed by Andrey <nova2k6@users.berlios.de>.

In the following, setting up CFLAGS, CXXFLAGS and LDFLAGS is left as
an exercise for the reader. Its quite likely that Rasterbar Libtorrent
and BTG needs these enviroment variables, if Boost or Rasterbar
Libtorrent is not installed where they can be found by the configure
scripts.

Building Boost
==============

# ./configure; 

Edit Makefile and add "--layout=system" to the BJAM_CONFIG variable. 

This simplifies libboost-* filenames.

Build Boost.

# make

Install Boost.

# make install

Building Rasterbar Libtorrent
=============================


Execute:

# ./configure --help 

to get the list of options supported.

Find out which Boost libs libtorrent uses, and run configure with:

# ./configure --with-boost-<libname>=mt 

for every needed Boost lib to use multi-threading versions of the boost libs.

Build Rasterbar Libtorrent.

# make

Install Rasterbar Libtorrent.

# make install

Building BTG
============

Execute

# ./configure --help; 

to get the list of supported options.

Find out which Boost libs BTG uses, and run configure with:

# ./configure --with-boost-<libname>=mt

for every needed Boost lib to use multi-threading versions of the boost libs.

Build BTG.

# make

Install BTG.

# make install

.. footer:: `BTG home page`_
.. _BTG home page: http://btg.berlios.de/

