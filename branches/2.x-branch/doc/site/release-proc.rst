=====================
BTG Release Procedure
=====================

.. contents:: Table of Contents 
   :depth: 2

Release Procedure
=================

This document describes the release procedure which shall be used for
every BTG release.

Build System
------------

Update the BTG version contained in configure.in, in the call to AM_INIT_AUTOMAKE.

Documentation
-------------

======================= ==================================================================
Action                  Description 
----------------------- ------------------------------------------------------------------
Generate doxygen        Generate doxygen by running "make doxygen". 
                        Doxygen will complain about missing documentation in header files.

Update site docs        Update site documentation. Update the howto.

Update man pages        Update the man pages, if any new switches were added.

Update Changelog        Update and commit the changelog file in the root of SVN. It must 
                        contain all the bugs which were fixed since last release.
======================= ==================================================================

SVN Tagging and Merging
-----------------------

The following is done using the "stable-branch" SVN branch.

======================= =====================================================================================
Action                  Description 
----------------------- -------------------------------------------------------------------------------------
Update to stable branch Update the current branch to stable branch. This could be done using:
                        "svn switch https://user@svn.berlios.de/svnroot/repos/btg/branches/stable-branch".

Merge from trunk        Merge the changes from trunk into the stable branch. This could be done using:
                        "svn merge https://user@svn.berlios.de/svnroot/repos/btg/branches/stable-branch \
                        https://user@svn.berlios.de/svnroot/repos/btg/trunk"

Tag                     Add a relase tag. This could be done using:
                        "svn copy https://user@svn.berlios.de/svnroot/repos/btg/trunk \ 
                        https://user@svn.berlios.de/svnroot/repos/btg/tags/0.9.6 \
                        -m 'Added 0.9.6 tag.'"

======================= =====================================================================================

Generating a Release
--------------------

Execute "make dist", copy the resulting .tar.gz file to /tmp from
where it will be built in the following step. Its assumed that the
setup.sh script which resides in the root of the BTG SVN repository is
used.

Building
--------

======================= ==================================================================
Action                  Description 
----------------------- ------------------------------------------------------------------
Build on Debian         Make sure that building on PPC and x86 works. Both debian stable 
                        and unstable shall be used.

Build on FBSD           Make sure that building on x86 works.
======================= ==================================================================

The most recent release of libtorrent shall be used for building as
well as a (development) SVN version of libtorrent.

Update Project Pages and Mailing Lists
--------------------------------------

============================== ==================================================================
Action                         Description 
------------------------------ ------------------------------------------------------------------
Add a new file to BTG releases In the berlios admin interface, use "Admin/Edit/Release Files". 
                               Remember to add a release note and to copy the list of fixed bugs 
                               from the Changelog in SVN.

Submit a news entry            The news are shown on the `BTG berlios.de page`_.

Update freshmeat entry         Update the `BTG freshmeat entry`_. 

Update btg.berlios.de          Add the files released on berlios to the list of BTG releases on 
                               the main page of the site.

Mailing list                   Write a short e-mail to the `BTG mailing list`_. It should contain the 
                               release note and the list of fixed bugs from step one. 
============================== ==================================================================

.. footer:: `BTG home page`_

.. _BTG home page: http://btg.berlios.de/
.. _BTG freshmeat entry: http://freshmeat.net/projects/btg
.. _BTG mailing list: http://developer.berlios.de/mail/?group_id=3293
.. _BTG berlios.de page: http://btg.berlios.de
