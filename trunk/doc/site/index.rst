===
BTG
===

.. contents:: Table of Contents 
   :depth: 2

What is BTG?
~~~~~~~~~~~~

Bittorrent client implemented in C++ and using the `Rasterbar Libtorrent`_
library.

Provides a Ncurses_, Gtkmm_ and WWW GUI, which communicate with a
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

Clients
-------
- wwwBTG (web based client written in PHP).
- btgncli (Ncurses client, menu driven).
- btgcli (Ncurses client, command line).
- btgui (Gtkmm GUI).

Platforms
---------
BTG is being used or was tested on:

- OSX (PowerPC).
- FreeBSD (PowerPC).
- GNU Linux - Debian (PowerPC, amd64, i686) / Gentoo (i686) / Slackware (i586).

Screenshots
-----------
- `WWW UI #1`_.
- `WWW UI #2`_.

.. _WWW UI #1: http://developer.berlios.de/dbimage.php?id=2916
.. _WWW UI #2: http://developer.berlios.de/dbimage.php?id=2915

- `btgncli - torrent menu`_.
- `btgncli - load dialog`_.
- `btgncli - limit torrent`_.
- `btgncli - main window help`_.
- `btgncli - global limits`_.
- `btgncli - file view`_.

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

- `btg-0.9.6.tar.gz`_ (`Rasterbar Libtorrent`_ 0.12)
- `btg-0.9.5.tar.gz`_ (`Rasterbar Libtorrent`_ 0.9/0.10/0.11/0.12)
- `btg-0.9.0.tar.gz`_ (`Rasterbar Libtorrent`_ 0.9/0.10/0.11)
- `btg-0.8.5.tar.gz`_ (`Rasterbar Libtorrent`_ 0.9/0.10/0.11)

.. _btg-0.8.5.tar.gz: http://download.berlios.de/btg/btg-0.8.5.tar.gz
.. _btg-0.9.0.tar.gz: http://download.berlios.de/btg/btg-0.9.0.tar.gz
.. _btg-0.9.5.tar.gz: http://download.berlios.de/btg/btg-0.9.5.tar.gz
.. _btg-0.9.6.tar.gz: http://download.berlios.de/btg/btg-0.9.6.tar.gz

Experimental Releases
---------------------
- `btg-0.9.6-rc4.tar.gz`_ (`Rasterbar Libtorrent`_ 0.12/SVN)

.. _btg-0.9.6-rc4.tar.gz: http://prdownload.berlios.de/btg/btg-0.9.6-rc4.tar.gz

Packages
~~~~~~~~

See `the list`_ of BTG packages.

.. _the list: packages.html

Documentation
~~~~~~~~~~~~~
- `BTG FAQ`_.
- `BTG howto (current)`_.
- `BTG howto (0.7)`_.
- `BTG howto (0.6)`_.
- `BTG howto (0.5)`_.
- `BTG howto (0.4)`_.
- `BTG howto (0.3)`_.
- `Manual pages`_ contained in the releases.
- `BTG Release Procedure`_.

.. _Manual pages: manpages.html

Authors
~~~~~~~
- Michael Wojciechowski.
- Johan Ström (XML-RPC_ transport and externalization, WWW UI, session saving, porting).
- Rodolphe Saugier (bugfix #10588).
- Gabor Tanka (DHT fixes).
- Jesper Nyholm Jensen (Testing).

Getting Help
~~~~~~~~~~~~

Do not use the forums, as they are not monitored. 

Instead use the #libtorrent IRC channel on irc.freenode.net.

Links
~~~~~

- `BTG project page`_ / `BTG bugtracker`_ / `BTG blog`_.
- `Rasterbar Libtorrent`_.

.. _BTG bugtracker: http://developer.berlios.de/bugs/?group_id=3293
.. _BTG blog: http://developer.berlios.de/blog/categories/32-BTG
.. _BTG project page: http://developer.berlios.de/projects/btg/ 
.. _BTG FAQ: faq.html
.. _BTG howto (current): howto.html
.. _BTG howto (0.3): howto-0.3.html
.. _BTG howto (0.4): howto-0.4.html
.. _BTG howto (0.5): howto-0.5.html
.. _BTG howto (0.6): howto-0.6.html
.. _BTG howto (0.7): howto-0.7.html
.. _Rasterbar Libtorrent: http://www.rasterbar.com/products/libtorrent.html
.. _Gtkmm: http://www.gtkmm.org/
.. _Ncurses: http://www.gnu.org/software/ncurses/ncurses.html
.. _XML-RPC: http://www.xmlrpc.com/
.. _BTG Release Procedure: release-proc.html

.. image:: http://developer.berlios.de/bslogo.php?group_id=0
   :height: 32
   :width: 124
   :alt: BerliOS Logo
   :align: right
   :target: http://developer.berlios.de
