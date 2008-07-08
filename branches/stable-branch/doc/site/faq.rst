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

BTG used to stand for BitTorrent GUI, but it evolved and currently the
name has no meaning.

What is BTG?
------------

Its all the following things:

- Daemon (btgdaemon).
- WWW bittorrent client (wwwBTG).
- GUI bittorrent client (btgui).
- Ncurses bittorrent client (btgcli).
- SDL viewer application (btgvs).

BTG consists of a number of different applications. The clients
communicate with the daemon.

Why do I have to read the documentation?
----------------------------------------

Some people expect software to just work, without any
configuration. If you expect this, BTG is _not_ for you. Give up now.

Building and setting up BTG requires you to read the `howto`_ and
possibly the `man pages`_.

What is a BTG session?
----------------------

When a client connects to a daemon for the first time, it creates a
session. 

It can detach from this session, and it will keep on working
(downloading/seeding) in the daemon. 

The client can attach to the session again, and continue using it.

Its possible to attach different clients to a BTG session. Its even
possible to use all the provided clients at the same time with a
BTG session.

A session belongs to an user - the user is not able to see other users
sessions, unless it has an control flag set (see description in the
`howto`_).

.. _howto: howto.html
.. _man pages: manpages.html

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

Because its pointless to create the required functionality from
scratch.

Link Check: error: libtorrent/session.hpp: No such file or directory
--------------------------------------------------------------------

Make sure the `Rasterbar Libtorrent`_ library is installed.
Using `Rakshasa Libtorrent`_ is NOT supported.

Error while loading shared libraries
------------------------------------
If you see errors like 

::

 btgpasswd: error while loading shared libraries: libtorrent-0.12.1.so: 
 cannot open shared object file: No such file or directory

You should really read the ld.so man page.

Feature X does not work, what now?
----------------------------------

- Read the `howto`_/`man pages`_. Maybe its described in the documentation.
- File a bug rapport (or send a patch against trunk in SVN).

Libtorrent asserts
------------------

`Rasterbar Libtorrent`_ _will_ assert if its compiled with
debug. Build a release, unless you want to test `Rasterbar Libtorrent`_.

Kindly report `Rasterbar Libtorrent`_ bugs to the `Rasterbar
Libtorrent`_ mailing list or bug tracker. The asserts, unless
triggered by BTG will not be handled as BTG bugs.

Can I expect SVN trunk to work?
-------------------------------

No. Trunk is being used for active development. Use a release or
release candidate.

How about adding feature X?
---------------------------

BTG accepts `patches`_. Make sure that its against the trunk of the SVN
repository.

.. _patches: http://developer.berlios.de/patch/?group_id=3293

How can I help?
---------------

- The WWW frontend seems to require a lot of attention. If you know PHP/javascript and have time to spare, consider implementing one of the requested features.

- Update/add documentation.

- Review/refactor existing code.

Join the #libtorrent IRC channel on irc.freenode.net and let us know
that you plan to help.

.. footer:: `BTG home page`_
.. _BTG home page: http://btg.berlios.de/
.. _Rasterbar Libtorrent: http://www.rasterbar.com/products/libtorrent.html
.. _Rakshasa Libtorrent: http://libtorrent.rakshasa.no
