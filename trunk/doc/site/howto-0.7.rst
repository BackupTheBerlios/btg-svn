=========
BTG Howto
=========

.. contents:: Table of Contents 
   :depth: 2

Introduction
============
This howto describes building, configuring and using the BTG daemon
and the available clients.


Building
========

Required Software
-----------------

The following software should be installed before attempting to build
BTG. Its assumed that a working C and C++ compiler is installed.

Daemon
~~~~~~
- C and C++ compiler.
- `Rasterbar Libtorrent 0.9`_ or `Rasterbar Libtorrent 0.10`_.
- Boost 1.33 or better (Because of the use of Boost IOStreams), 
  multithreaded versions of the libs should be used.
- `GNU TLS`_ 1.0 or better.
- Automake.
- Autoconf 2.59.

.. _Rasterbar Libtorrent 0.9: http://www.rasterbar.com/products/libtorrent.html
.. _Rasterbar Libtorrent 0.10: http://www.rasterbar.com/products/libtorrent.html
.. _GNU TLS: http://www.gnu.org/software/gnutls/

UI
~~
- Ncurses.
- Gtkmm 2.4.

WWW Frontend
~~~~~~~~~~~~
- Apache, for example Apache 2.2.
- PHP5.
- PHP5-extensions (where XML and ZLIB is needed for BTG. ZLIB is not a requirement but speeds up the traffic between php and btgdaemon).

Building the Daemon
-------------------

Checking out from CVS
~~~~~~~~~~~~~~~~~~~~~
Create a directory used for compiling.

# mkdir ~/btg-devel

Fetch BTG from CVS.

# cvs -d:pserver:anonymous@cvs.btg.berlios.de:/cvsroot/btg login

# cvs -z3 -d:pserver:anonymous@cvs.btg.berlios.de:/cvsroot/btg co -r devel-branch btg

Creating a Build System
~~~~~~~~~~~~~~~~~~~~~~~
Generate the build system.

# cd ~/btg-devel/btg

# ./autogen.sh

Configuring the Build
~~~~~~~~~~~~~~~~~~~~~

Now run configure to create either a release or debug build.

# ./configure

or

# ./configure --enable-debug --enable-unittest

The above command will enable writing extensive debug information and
build unittests, which can be executed using:

# make check

after a completed build.

The following options can be given to configure at this time:

======================= =========================================================
Option                  Description 
----------------------- ---------------------------------------------------------
--enable-gui            Enable building GUI client.
--enable-cli            Enable building CLI client.
--enable-www            Enable installing WWW client.
--enable-debug          Enable compiling with debug logging.
--enable-unittest       Enable building unittests.
--enable-session-saving Enable saving sessions (daemon).
--enable-event-callback Enable executing scripts for certain events (daemon).
--enable-upnp           Enable using UPnP. Interfaces the CyberLink UPnP library.
======================= =========================================================

Using Boost Installed from Source
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The configure script may need some help when boost was installed from
source.

In the following I installed boost 1.33.1 from source in my home
directory (I used the prefix "/home/wojci/boost-1.33.1".).

The configure script reads a number of enviroment variables. In the
following, CPPFLAGS and LDFLAGS will be used to tell the script where
to find the boost headers and libraries.

# export CPPFLAGS="-I/home/wojci/boost-1.33.1/include/boost-1_33_1"

# export LDFLAGS="-L/home/wojci/boost-1.33.1/lib"

The above commands should be executed before running the configure script.

Notice the following options:

- --with-boost-iostreams
- --with-boost-filesystem
- --with-boost-date-time
- --with-boost-thread
- --with-boost-program-options

The above options are used to tell the scripts that detects boost
libraries which libs should be used for linking.

For example to use boost iostreams installed in
/home/wojci/boost-1.33.1/lib use:

# ./configure --with-boost-iostreams=boost_iostreams-gcc-mt-1_33_1

See also `Building Boost, Rasterbar Libtorrent and BTG from Source`_.

.. _Building Boost, Rasterbar Libtorrent and BTG from Source: howto_bfs.html

Building
~~~~~~~~
Execute make to build BTG.

# make

Installation
~~~~~~~~~~~~
# make install

The above command will install BTG to the default location used by configure.
The WWW UI will be installed to PREFIX/share/wwwbtg. 
The default value of PREFIX is /usr/local.

Distro Packages
---------------
Debian
~~~~~~

A template can be found in the /debian directory. This needs adjusting
to the debian version used and to which boost version are installed.

Gentoo
~~~~~~

Ebuild files contributed by Peter Koeleman are located in the /gentoo
directory. These files need to be adjusted to the `Rasterbar
Libtorrent`_ and BTG versions used.

.. _Rasterbar Libtorrent: http://www.rasterbar.com/products/libtorrent.html

Configuration
=============

The applications must be configured before use. They will not work without a correct config file.
The following assumes that all configuration files will be located in ~/.btg.

Daemon
------

`daemon.ini`_ - Place this file in ~/.btg/daemon.ini, which is the default daemon config file location. 

.. _daemon.ini: files/daemon.ini

The above config sets the daemon to do the following:

- Setup BTG auth, namely the file from which users are read from.
- Uses the XML-RPC transport.
- Enables logging to a file.
- Makes the daemon listen to port 16000 or all interfaces.
- Enables limits on upload speed, download speed, max connections and max uploads. All speeds are in bytes or bytes per second.

Create users:
~~~~~~~~~~~~~

When the configuration files are done you will need to create one or more users. This is done with the btgpasswd utility:

# btgpasswd --create -a -t "~/some/path/torrents" -w "~/some/path/work" -d "~/btg/some/path/finished" -s "~/btg/some/path/seeding" -p -u <username>

- The -a parameter specifies that we want a new user to be created.

- The -t -w -d and -s parameters are directories to use for temporary .torrent storage, running downloads, finished downloads ("cleaned" torrents, not seeding anymore), and seeding torrents.

- The -p parameter means to read password from stdin.

- The -u parameter specifies the name of the user to add. 

Use the --create parameter to create a new password file, if it doesnt
exist. Enter the password on stdin, then you should get an OK and you
are done.

Make sure you create all the directories before you start the daemon.

Clients
-------

Before attempting to use any of the clients, follow the instruction in
`Testing`_ to make sure that the daemon starts and can communicate with
clients.

Gtkmm and Ncurses clients
~~~~~~~~~~~~~~~~~~~~~~~~~

`client.ini`_ - Place this file in ~/.btg/client.ini, which is the default client config file location. 

.. _client.ini: files/client.ini

The above config makes the clients to do the following:

- Uses the XML-RPC transport.
- Tells the clients to connect to localhost, port 16000.
- Enables logging to a file.

wwwBTG
~~~~~~
Configuring PHP:

Make sure that the following is included in the apache config file if you want to run the WWW UI.

AddType application/x-httpd-php .php

AddType application/x-httpd-php-source .phps

Configuring wwwBTG:

The easiest way to access wwwBTG is to create a symlink from your
webservers document root to PREFIX/share/wwwbtg/htdocs (see
Installation). If no PREFIX was set before installing, wwwBTG should
have been installed in /usr/local/share/wwwbtg.

# ln -s /usr/local/share/wwwbtg/htdocs /home/www/btg

Make sure Apache has FollowSymLinks enabled on this directory so it
can follow the symlink for wwwBTG.

wwwBTG has a configuration file named config.php. For the moment you
dont have to do any changes here unless you run the daemon and the web
UI on two different machines.

Surf to http://www.example.com/btg or wherever you placed it, and
select your session in the list (or create a new) and press attach,
and wwwBTG is ready to use.

Other:

When there are updates in BTG you can run cvs update -dP in
~/btg-devel/ again to download the latest source code, and then you
run the autogen-stuff, configure as before and finaly make (you might
want to run make clean before this to make sure no conflicts arises).

To stop the daemon just issue a kill in the pid. If you activated
sessionsaving all sessions & torrents will be saved when a SIGINT
(ctrl-c if you run in foreground) or SIGTERM (default kill signal) is
received. Sessions will also be saved periodicly, every 'timeout'
seconds as specified in configuration file.
When receiving a SIGHUP the password file will be reread.

Testing
=======

The following instructions are to ensure that an installation of BTG
actually works.

Starting the daemon
-------------------

The following command will start the daemon.

# btgdaemon

You can pass the -n parameter so it wont fork into background mode,
which is good when testing. -v or -d can also be added for more
verbose logging, this helps alot when troubleshooting). To use the
daemon you have to use a client, either the CLI-client, the GUI-client
or wwwBTG. All three can be used to setup sessions.

Starting the CLI
----------------

# btgcli

This will start the command line client. Most operations can be done
from the web interface (see below), but some things are not yet doable
(for example, set individual limits on torrents). 

You will be requested to enter username and password. If you run
without any parameters, you will create a new session which later can
be reattached. Write "detach" to detach the session, but leave it
running in the daemon (what you normally do). Note that if you write
"quit" the session will be terminated! If you like to run the CLI later,
you can use the parameter -A to attach to the first available session.

IPv4 Filter
===========

At this point two IPv4 filter formats are supported: Emule and Level One.

Emule
-----

An `Emule filter`_ can contain lines like (white space was added here):
::

 IPv4            IPv4            LEVEL DESCRIPTION
 --------------- --------------- ---   -------------------------------------------
 003.000.000.000-003.255.255.255,110,  [L1]General Electric Company
 004.000.025.146-004.000.025.148,110,  [L1]s0-0.ciscoseattle.bbnplanet.net 
 004.000.026.014-004.000.029.024,110,  [L1]p1-0.cisco.bbnplanet.net
 004.002.144.032-004.002.144.047,120,  [L2]International Generating Co. (Intergen)

When a filter it read the following is extracted:

- IPv4 range (the two IPv4 addresses).
- Level.

If the level is greater than 0 and less or equal to 120 then the IPv4 range is blocked.

.. _Emule filter : http://www2.openmedia.info:8080/p23.html

Level One
---------

The `Level One`_ filter can consist of lines like (white space was added here):

::

 LABEL                           IPv4        IPv4
 ------------------------------- ----------- -------------
 General Electric Company       :3.0.0.0    -3.255.255.255
 s0-0.ciscoseattle.bbnplanet.net:4.0.25.146 -4.0.25.148
 p1-0.cisco.bbnplanet.net       :4.0.26.14  -4.0.29.24
 Cisco Systems, Inc             :4.2.144.64 -4.2.144.95
 Drug Enforcement Adm           :4.2.144.224-4.2.144.231
 US Dept of Treasury - TIGTA    :4.2.144.248-4.2.144.255

When a filter it read the following is extracted:

- IPv4 range (the two IPv4 addresses).

All IPv4 ranges found are blocked.

.. _Level one: http://www.bluetack.co.uk/config/level1.zip

Reporting Bugs
==============
If you find any bugs, please report them using the `bugtracker`_.

Kindly provide the following information:

- Operating System.
- Which compiler was used to build BTG, Boost and Rasterbar Libtorrent.
- Libtorrent version.
- Options given to the configure script.
- If the problem is a m4 script, provide the config.log file produced by configure.
- Confiuration files (daemon.ini and client.ini), if needed to fix your bug.
- Log files produced by the daemon, core dumps etc..
- Any other information you think may be needed to fix your bug.

.. _bugtracker: http://developer.berlios.de/bugs/?group_id=3293

Credits
=======

Original howto, translated from swedish written by: donnex with help
of unclear, and jstrom.

Links
=====

- `BTG home page`_
- `BTG project page`_

.. _BTG project page: http://developer.berlios.de/projects/btg/ 
.. _BTG home page: http://btg.berlios.de/
