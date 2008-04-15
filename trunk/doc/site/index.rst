===
BTG
===

.. contents:: Table of Contents 
   :depth: 2

What is BTG?
~~~~~~~~~~~~

Bittorrent client implemented in C++ and using the `Rasterbar Libtorrent`_
library.

Provides a Ncurses_, SDL_, Gtkmm_ and WWW GUI, which communicate with a
common backend running the actual bittorrent operation.

Features
--------
- Daemonized backend.
- Number of different clients.
- Ability to change client without stopping downloads.
- Common interface (transport layer) used to access the daemon shared by the clients.
- Portability.
- Session saving.
- Per user event callback.
- IPv4 filter (level1.txt and Emule lists).
- Port forwarding using UPnP.
- Periodic session saving.
- DHT and encryption (since libtorrent supports both).

Clients
-------
- wwwBTG (web based client written in PHP_).
- btgncli (Ncurses_ client, menu driven).
- btgcli (Ncurses_ client, command line).
- btgui (Gtkmm_ GUI).
- btgvs (SDL_ viewer GUI using AGAR_).

Third Party Clients
-------------------
- `BTGFox`_ - a Firefox extension written by Nelson Silva.

.. _BTGFox: http://www.bakanl.net/btgfox/

Platforms
---------
BTG is being used or was tested on:

- OSX (PowerPC-32).
- FreeBSD (PowerPC-32, amd64).
- GNU Linux - Debian (PowerPC-32, amd64, i686) / Gentoo (PowerPC-64, i686) / Slackware (i586) / OpenWrt (XScale-IXP42x).

Screenshots
~~~~~~~~~~~

0.9.7
-----
- `btgui #1`_.
- `btgui #2`_.

.. _btgui #1: http://developer.berlios.de/dbimage.php?id=3971
.. _btgui #2: http://developer.berlios.de/dbimage.php?id=3973

0.9.6
-----

- `WWW UI #1`_.
- `WWW UI #2`_.
- `btgncli - torrent menu`_.
- `btgncli - load dialog`_.
- `btgncli - limit torrent`_.
- `btgncli - main window help`_.
- `btgncli - global limits`_.
- `btgncli - file view`_.

.. _WWW UI #1: http://developer.berlios.de/dbimage.php?id=2916
.. _WWW UI #2: http://developer.berlios.de/dbimage.php?id=2915
.. _btgncli - torrent menu: http://developer.berlios.de/dbimage.php?id=3413
.. _btgncli - load dialog: http://developer.berlios.de/dbimage.php?id=3412
.. _btgncli - limit torrent: http://developer.berlios.de/dbimage.php?id=3411
.. _btgncli - main window help: http://developer.berlios.de/dbimage.php?id=3410
.. _btgncli - global limits: http://developer.berlios.de/dbimage.php?id=3409
.. _btgncli - file view: http://developer.berlios.de/dbimage.php?id=3408

Builds and Releases
~~~~~~~~~~~~~~~~~~~
Two different branches exist at this point. The "stable-branch" and the "devel-branch".

Development is done on the "devel-branch" and the changes are merged
into the "stable-branch" as soon as they are well tested and all
classes are documented.

Stable Releases
---------------

======================= ========================== ======================= ============================================
**Source**              **Release note**           **Changelog**           **Rasterbar Libtorrent version**
----------------------- -------------------------- ----------------------- --------------------------------------------
`btg-0.9.7.tar.gz`_     `0.9.7 release note`_      `0.9.7 changelog`_      0.12.x/0.13rc1
`btg-0.9.6.tar.gz`_     `0.9.6 release note`_      `0.9.6 changelog`_      0.12
`btg-0.9.5.tar.gz`_     `0.9.5 release note`_      `0.9.5 changelog`_      0.9/0.10/0.11/0.12
`btg-0.9.0.tar.gz`_     `0.9.0 release note`_      `0.9.0 changelog`_      0.9/0.10/0.11
`btg-0.8.5.tar.gz`_     `0.8.5 release note`_      `0.8.5 changelog`_      0.9/0.10/0.11
======================= ========================== ======================= ============================================

.. _btg-0.8.5.tar.gz: http://download.berlios.de/btg/btg-0.8.5.tar.gz
.. _0.8.5 release note: http://developer.berlios.de/project/shownotes.php?release_id=12057
.. _0.8.5 changelog: http://developer.berlios.de/project/shownotes.php?release_id=12057

.. _btg-0.9.0.tar.gz: http://download.berlios.de/btg/btg-0.9.0.tar.gz
.. _0.9.0 release note: http://developer.berlios.de/project/shownotes.php?release_id=12316
.. _0.9.0 changelog: http://developer.berlios.de/project/shownotes.php?release_id=12316

.. _btg-0.9.5.tar.gz: http://download.berlios.de/btg/btg-0.9.5.tar.gz
.. _0.9.5 release note: http://developer.berlios.de/project/shownotes.php?release_id=12768
.. _0.9.5 changelog: http://developer.berlios.de/project/shownotes.php?release_id=12768

.. _btg-0.9.6.tar.gz: http://download.berlios.de/btg/btg-0.9.6-p1.tar.gz
.. _0.9.6 release note: http://developer.berlios.de/project/shownotes.php?release_id=13428
.. _0.9.6 changelog: http://developer.berlios.de/project/shownotes.php?release_id=13428

.. _btg-0.9.7.tar.gz: http://download.berlios.de/btg/btg-0.9.7.tar.gz
.. _0.9.7 release note: http://developer.berlios.de/project/shownotes.php?release_id=14349
.. _0.9.7 changelog: http://developer.berlios.de/project/shownotes.php?release_id=14349

Experimental Releases
---------------------

======================= ========================== ======================= ==================================
**Source**              **Release note**           **Changelog**           **Rasterbar Libtorrent version**
----------------------- -------------------------- ----------------------- ----------------------------------
`btg-0.9.8-rc1.tar.gz`_ `0.9.8 RC1 release note`_  `0.9.8 RC1 changelog`_  0.12.1/0.13/SVN
======================= ========================== ======================= ==================================

.. _btg-0.9.8-rc1.tar.gz: http://prdownload.berlios.de/btg/btg-0.9.8-rc1.tar.gz
.. _0.9.8 RC1 release note: https://developer.berlios.de/project/shownotes.php?release_id=14498
.. _0.9.8 RC1 changelog: https://developer.berlios.de/project/shownotes.php?release_id=14498


Packages
~~~~~~~~

See `the list`_ of BTG packages.

.. _the list: packages.html

Documentation
~~~~~~~~~~~~~
- `BTG FAQ`_.
- `BTG howto (current)`_.
- `BTG howto (0.9.7)`_.
- `BTG howto (0.9.6)`_.
- `BTG howto (0.7)`_.
- `BTG howto (0.6)`_.
- `BTG howto (0.5)`_.
- `BTG howto (0.4)`_.
- `BTG howto (0.3)`_.
- `Manual pages`_ contained in the releases.
- `BTG Developer Howto`_.
- `BTG Release Procedure`_.

.. _Manual pages: manpages.html

Authors
~~~~~~~
- Michael Wojciechowski.
- Johan Ström (XML-RPC_ transport and externalization, WWW UI, session saving, porting).
- Roman Rybalko (making btgui look pretty, countless bugfixes, packaging).
- Rodolphe Saugier (bugfix #10588).
- Gabor Tanka (DHT fixes).
- Jesper Nyholm Jensen (Testing).

Getting Help
~~~~~~~~~~~~

Do not use the forums, as they are not monitored. 

Instead use the `mailing list`_.

.. _mailing list: http://developer.berlios.de/mail/?group_id=3293

Links
~~~~~

- `BTG project page`_ / `BTG bugtracker`_ / `BTG blog`_.
- `Rasterbar Libtorrent`_.

.. _BTG bugtracker: http://developer.berlios.de/bugs/?group_id=3293
.. _BTG blog: http://developer.berlios.de/blog/categories/32-BTG
.. _BTG project page: http://developer.berlios.de/projects/btg/ 
.. _BTG FAQ: faq.html
.. _BTG Developer Howto: howto_devel.html
.. _BTG Howto (current): howto.html
.. _BTG Howto (0.3): howto-0.3.html
.. _BTG Howto (0.4): howto-0.4.html
.. _BTG Howto (0.5): howto-0.5.html
.. _BTG Howto (0.6): howto-0.6.html
.. _BTG Howto (0.7): howto-0.7.html
.. _BTG Howto (0.9.6): howto-0.9.6.html
.. _BTG Howto (0.9.7): howto-0.9.7.html
.. _Rasterbar Libtorrent: http://www.rasterbar.com/products/libtorrent.html
.. _Gtkmm: http://www.gtkmm.org/
.. _SDL: http://libsdl.org/
.. _AGAR: http://libagar.org/
.. _Ncurses: http://www.gnu.org/software/ncurses/ncurses.html
.. _PHP: http://php.net/
.. _XML-RPC: http://www.xmlrpc.com/
.. _BTG Release Procedure: release-proc.html

.. image:: http://developer.berlios.de/bslogo.php?group_id=0
   :height: 32
   :width: 124
   :alt: BerliOS Logo
   :align: right
   :target: http://developer.berlios.de
