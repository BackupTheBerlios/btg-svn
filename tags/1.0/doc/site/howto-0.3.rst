=========
BTG Howto
=========

.. contents:: Table of Contents 
   :depth: 2

Introduction
============
This howto describes building, configuring and using the BTG daemon and the available clients.


Building
========

Required Software
-----------------
The following software should be installed before attempting to build BTG.

Daemon
~~~~~~
- `Libtorrent 0.9`_.
- Boost 1.32 or 1.33.
- Automake.
- Autoconf 2.59.

.. _Libtorrent 0.9: http://www.rasterbar.com/products/libtorrent.html

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

# cvs -z3 -d:pserver:anonymous@cvs.btg.berlios.de:/cvsroot/btg co -r devel-branch btg-devel

Creating a Build System
~~~~~~~~~~~~~~~~~~~~~~~
Generate the build system.

# cd ~/btg-devel/btg

# ./autogen.sh

Configuring the Build
~~~~~~~~~~~~~~~~~~~~~

Now run configure to create either a release or debug build.

# ./configure --enable-all

or

# ./configure --enable-all --enable-debug --enable-unittest

The above command will enable writing extensive debug information and build unittests, which can be executed using:

# make check

after a completed build.

Building
~~~~~~~~
Execute make to build BTG.

# make

Installation
~~~~~~~~~~~~
# make install

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

# btgpasswd -a -t "~/btg/user/username/torrents" -w "~/btg/user/username/work" -d "~/btg/user/username/finished" -d "~/btg/user/username/seeding" -p -u <username> -n

- The -a parameter specifies that we want a new user to be created.

- The -t -w -d and -s parameters are directories to use for temporary .torrent storage, running downloads, finished downloads ("cleaned" torrents, not seeding anymore), and seeding torrents.

- The -p parameter means to read password from stdin.

- The -u parameter specifies the name of the user to add. 

Use the -n parameter to create a new password file, if it doesnt
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
webservers document root to ~/btg-devel/UI/www/php/htdocs.

# ln -s ~/btg-devel/UI/www/php/htdocs /home/www/btg

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
received. When receiving a SIGHUP the password file will be reread.

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

Reporting Bugs
==============
If you find any bugs, please report them using the `bugtracker`_.

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
