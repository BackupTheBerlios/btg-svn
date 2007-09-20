=======
BTG FAQ
=======

.. contents:: Table of Contents 
   :depth: 2

BTG FAQ
=======

This is a FAQ for BTG.

What does BTG stand for?
------------------------

BTG used to stand for BitTorrent GUI, but it evolved and currently the name has no meaning.

What is BTG?
------------

Its all the following things:

- Daemon (btgdaemon).
- WWW bittorrent client (wwwBTG).
- GUI bittorrent client (btgui).
- Ncurses bittorrent client (btgcli).

BTG consists of a number of different applications. The clients
communicate with the daemon.

What is a BTG session?
----------------------

When a client connects to a daemon for the first time, it creates a
session. 

It can detach from this session, and it will keep on working
(downloading/seeding) in the daemon. 

The client can attach to the session again, and continue using it.

Its possible to attach different clients to a BTG session. Its even
possible to use the three different clients at the same time with a
BTG session.

Why is BTG so feature bloated?
------------------------------

Its not. Most features can be removed at compile time.

Using 

# ./configure

only the daemon will be built. 

One has to use one of the --enable-* flags to enable a feature.

Does BTG support Unicode?
-------------------------

No.

Why does BTG require so many different libraries?
-------------------------------------------------

Because its pointless to create the required functionality from scratch.

Link Check: error: libtorrent/session.hpp: No such file or directory
--------------------------------------------------------------------

Make sure the the `Rasterbar Libtorrent`_ library is installed.
Using `Rakshasa Libtorrent`_ is NOT supported.

Feature X does not work, what now?
----------------------------------

- Read the howto/man pages. Maybe its described in the documentation.
- File a bug rapport.

Libtorrent asserts
------------------

Libtorrent _will_ assert if its compiled with debug. Build a release,
unless you want to test libtorrent.

Kindly report libtorrent bugs to the libtorrent mailing list or bug
tracker. The asserts, unless triggered by BTG will not be handled as
BTG bugs.
 
How about adding feature X?
---------------------------

BTG accepts patches. Make sure that its against the devel-branch.

How can I help?
---------------

Great. 

As always, more/better documentation is needed.

Code gets better, when reviewed by others than the developers.

.. footer:: `BTG home page`_
.. _BTG home page: http://btg.berlios.de/
.. _Rasterbar Libtorrent: http://www.rasterbar.com/products/libtorrent.html
.. _Rakshasa Libtorrent: http://libtorrent.rakshasa.no
