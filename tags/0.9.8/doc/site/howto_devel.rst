===================
BTG Developer Howto
===================

.. contents:: Table of Contents 
   :depth: 2

Introduction
============

This short howto contains information which could be useful for BTG
developers.

Developer Access to SVN
=======================

Follow the instructions on http://developer.berlios.de/svn/?group_id=3293.

Trunk is being used for developement. Do not use the stable-branch.

Generating the Build System
===========================

Linux
-----

Running the autogen.sh script located in the root of SVN trunk should
be enough.

FreeBSD
-------

On some FreeBSD versions, one has to use the following to generate the build system, since it contains older versions of autotools and no symlinks.

::

 autoheader259 &&
 aclocal19 -I m4 -I /usr/local/share/aclocal &&
 libtoolize -c -f &&
 automake19 --add-missing --copy --gnu &&
 autoconf259 &&
 autoheader259 &&
 rm -f config.cache

OSX
---

Remember to use glibtoolize instead of libtoolize.

List of commands
================

The list of commands is available `here`_ (generated from sources in the
trunk of SVN).

.. _here: commands.html

Patches
=======

Patches can be easily made using "svn diff > file.diff" from trunk.

.. footer:: `BTG home page`_
.. _BTG home page: http://btg.berlios.de/

