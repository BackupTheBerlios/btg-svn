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
BTG.

Its assumed that a working C and C++ compiler is installed. 

Its also assumed that, if one is generating the configure script, the
following applications are installed:

- GNU autoconf 2.61 or later.
- GNU automake 1.9.6 or later.
- GNU libtool 1.4.3 or later.
- pkg-config 0.21 or later.

Daemon
~~~~~~
- C and C++ compiler.
- `Rasterbar Libtorrent`_ 0.12.x, 0.13rc1-3 or SVN. To decide which libtorrent version to use see `Rasterbar Libtorrent Versions`_.
- Boost 1.33 or better (Because of the use of Boost IOStreams), 
  multithreaded versions of the libs should be used.
- `GNU TLS`_ 1.0 or better.
- `Expat`_ 1.95.8 or better.
- `CyberLink`_ 1.7.1 or better for UPnP (optional).
- `CURL`_ 7.15.5 or better for URL downloading (optional).

.. _Rasterbar Libtorrent: http://www.rasterbar.com/products/libtorrent.html
.. _GNU TLS: http://www.gnu.org/software/gnutls/
.. _CyberLink: https://sourceforge.net/projects/clinkcc/
.. _Expat: http://expat.sourceforge.net/
.. _CURL: http://curl.haxx.se/

UI
~~
- `Ncurses`_ 5.
- `Gtkmm`_ 2.4.
- `AGAR`_ 1.2 or 1.3.1.

.. _Gtkmm: http://www.gtkmm.org/
.. _AGAR: http://libagar.org/
.. _Ncurses: http://www.gnu.org/software/ncurses/ncurses.html

WWW Frontend
~~~~~~~~~~~~
- Apache, for example Apache 2.2.
- PHP5.
- PHP5-extensions (where XML and ZLIB is needed for BTG. ZLIB is not a requirement but speeds up the traffic between PHP and btgdaemon).

Support Scripts
~~~~~~~~~~~~~~~
- `Dialog`_.

.. _Dialog: http://hightek.org/dialog/

Building the Daemon
-------------------

Checking out from SVN
~~~~~~~~~~~~~~~~~~~~~
Create a directory used for compiling.

::

 mkdir ~/btg-devel

Fetch BTG from SVN.

::

 cd ~/btg-devel && svn checkout http://svn.berlios.de/svnroot/repos/btg/trunk

Creating a Build System
~~~~~~~~~~~~~~~~~~~~~~~
Generate the build system.

::

 cd ~/btg-devel/trunk
 ./autogen.sh

Configuring the Build
~~~~~~~~~~~~~~~~~~~~~

Now run configure to create either a release or debug build.

::

 ./configure

or

::

 ./configure --enable-debug --enable-unittest

The above command will enable writing extensive debug information and
build unittests, which can be executed using:

::

 make check

after a completed build.

The following options can be given to configure at this time:

======================= ==================================================================
Option                  Description 
----------------------- ------------------------------------------------------------------
--enable-btg-config     Enable generating the btg-config script (uses Dialog).
--enable-gui            Enable building GUI client.
--enable-viewer         Enable building SDL viewer (uses AGAR).
--enable-cli            Enable building CLI clients.
--enable-www            Enable installing WWW client.
--enable-debug          Enable debug logging.
--enable-unittest       Enable building unittests.
--enable-session-saving Enable saving sessions (daemon).
--enable-event-callback Enable executing scripts for certain events (daemon).
--enable-upnp           Enable using UPnP (daemon). Interfaces the CyberLink UPnP library.
--enable-command-list   Enable generating a command list (Internal option).
======================= ==================================================================

Link Check
~~~~~~~~~~

One of the final steps on the configure script is to perform a link
check.

This check attempts to link a short example which uses all the
libraries which are required by BTG. 

If it fails, config.log will contain the details why.

The configure script is verbose and writes the CXXFLAGS and LDFLAGS
that will be used for the check before performing it.

This could look like this (debian unstable, everything but Rasterbar
Libtorrent (from SVN) installed using apt-get):

:: 

  checking whether the a command list should be generated... yes
  configure: Link test, compiler flags: -Wall -W -g -I/pack/libtorrent/svn/include -I/pack/libtorrent/svn/include/libtorrent -I/usr/include -I/usr/include
  configure: Link test, linker flags  :  
            -lboost_iostreams -lboost_filesystem -lboost_date_time
            -lboost_thread -lboost_program_options
            -L/pack/libtorrent/svn/lib -ltorrent -lgnutls
  checking if BTG can link with the provided/found libraries... yes

If the link check fails, it does so before compiling any part of BTG.

A common fault is that a library, that is required by BTG, depends on
another library, which was not detected by the configure script. For
the linking to work, one has to adjust CXXFLAGS and LDFLAGS before
running configure.

A common problem can be that the boost.thread library uses pthreads,
but its not detected by the configure script. This is one of the
problems with the `Gentoo`_ Linux distribution.

.. _Gentoo: http://gentoo.org/

Using Boost Installed from Source
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The configure script may need some help when boost was installed from
source.

In the following I installed boost 1.33.1 from source in my home
directory (I used the prefix "~/boost-1.33.1".).

The configure script reads a number of enviroment variables. In the
following, CPPFLAGS and LDFLAGS will be used to tell the script where
to find the boost headers and libraries.

::

 export CPPFLAGS="-I~/boost-1.33.1/include/boost-1_33_1"
 export LDFLAGS="-L~/boost-1.33.1/lib"

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
~/boost-1.33.1/lib use:

::

 ./configure --with-boost-iostreams=boost_iostreams-gcc-mt-1_33_1

See also `Building Boost, Rasterbar Libtorrent and BTG from Source`_.

.. _Building Boost, Rasterbar Libtorrent and BTG from Source: howto_bfs.html

Building
~~~~~~~~
Execute make to build BTG.

::

 make

Rare Problem with GNU Libtool
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The following sometimes happends. One gets a message from libtool like this:

libtool: link: 'filename.lo' is not a valid libtool object.

The fix is to delete filename.lo and attempt to build again.

Installation
~~~~~~~~~~~~
::

 make install

The above command will install BTG to the default location used by configure.
The WWW UI will be installed to PREFIX/share/wwwbtg. 
The default value of PREFIX is /usr/local.

One can also use:

::

 make install-strip

to install stripped binaries and libraries.

Distro Packages
---------------

See `the list`_ of BTG packages.

.. _the list: packages.html

.. _Rasterbar Libtorrent: http://www.rasterbar.com/products/libtorrent.html

Configuration
=============

The applications must be configured before use. They will not work without a correct config file.
The following assumes that all configuration files will be located in ~/.btg.

Using btg-config
----------------

One can use the provided btg-config script. It asks the user a number of questions and produces 
the daemon and client .ini files based on the answers.

The final step of the script is to write the configuration files. 
They will be written in the directory in which btg-config was executed.

btg-config will not create a passwd file, only add an entry to the daemon.ini configuration 
file about where to find it. Therefore one should use btgpasswd to add the contents of the passwd file.

Daemon
------

`daemon.ini`_ - Place this file in ~/.btg/daemon.ini, which is the default daemon config file location. 

.. _daemon.ini: files/daemon.ini

The above config sets the daemon to do the following:

- Setup BTG auth, namely the file from which users are read from.
- Uses the XML-RPC transport.
- Enables logging to a file.
- Makes the daemon listen to port 16000 or all interfaces.
- Use ports 10024,10025 for libtorrent. If DHT is used, two ports are used per session, without DHT one port is used per session.
- Enables limits on upload speed, download speed, max connections and max uploads. All speeds are in bytes or bytes per second.

Create users:
~~~~~~~~~~~~~

When the configuration files are done you will need to create one or more users. This is done with the btgpasswd utility:

::

 btgpasswd --create -a -t "~/some/path/torrents" -w "~/some/path/work" -d "~/btg/some/path/finished" -s "~/btg/some/path/seeding" -p -u <username>

(See `Supported Directory Structure`_ before you create any directories.)

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

::

 AddType application/x-httpd-php .php
 AddType application/x-httpd-php-source .phps

Configuring wwwBTG:

The easiest way to access wwwBTG is to create a symlink from your
webservers document root to PREFIX/share/wwwbtg/htdocs (see
Installation). If no PREFIX was set before installing, wwwBTG should
have been installed in /usr/local/share/wwwbtg.

::

 ln -s /usr/local/share/wwwbtg/htdocs /home/www/btg

Make sure Apache has FollowSymLinks enabled on this directory so it
can follow the symlink for wwwBTG.

wwwBTG has a configuration file named config.php. For the moment you
dont have to do any changes here unless you run the daemon and the web
UI on two different machines.

Go to http://www.example.com/btg or wherever you placed it, and
select your session in the list (or create a new) and press attach,
and wwwBTG is ready to use.

Other:

When there are updates in BTG you can run svn update in
~/btg-devel/trunk again to download the latest source code, and then
you run the autogen-stuff, configure as before and finaly make (you
might want to run make clean before this to make sure no conflicts
arises).

To stop the daemon just issue

::

 killall btgdaemon

If you activated sessionsaving all sessions & torrents will be saved
when a SIGINT (ctrl-c if you run in foreground) or SIGTERM (default
kill signal) is received. Sessions will also be saved periodicly,
every 'timeout' seconds as specified in configuration file.  When
receiving a SIGHUP the password file will be reread.

Testing
=======

The following instructions are to ensure that an installation of BTG
actually works.

Starting the daemon
-------------------

The following command will start the daemon.

::

 btgdaemon

One can pass the -n parameter so it wont fork into background,
which is good when testing. 

The verbose(-v) or debug (-D, only when debug is compiled in) can also
be added for more logging - this helps alot when troubleshooting. To
interact with the daemon you have to use a client, either the
CLI-client, the GUI-client or wwwBTG. All three can be used to setup
sessions.

Starting the CLI
----------------

::

 btgcli

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

Auth System
===========

BTG uses a simple ASCII text file to keep users and other information
in. This file can be manipulated using the btgpasswd utility or using
a text editor.

Format
------

The passwd file used by BTG consists of lines formatted as follows (notice the use of ":"):

::

  User name:Password Hash:Temporary directory:Work directory:Seed directory:Destination directory:Control:Callback

User Name
~~~~~~~~~

The username, a string.

Password Hash
~~~~~~~~~~~~~

MD5 hash of password.

Directories.
~~~~~~~~~~~~

Temporary directory - where torrent files are kept in.

Work directory - where data is kept while downloading.

Seed directory - where data is moved to when a torrents starts
seeding.

Destination directory - where data is moved once a torrent is cleaned
or a seedtimer/upload timer expires.

Sharing of the above directories between users is not supported and
will have unexpected results.

Control
~~~~~~~

1 - enabled. 

0 - disabled.

If enabled, user is capable of shutting down the daemon and setting
global limits. This flag also makes it possible for the user to
manipulate sessions belonging to other users.

Callback
~~~~~~~~

If callbacks are enabled, the path to a script that is executed
when certain events are detected.

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

FreeBSD notes
=============

`Tcsh`_ seems to be the default shell on FreeBSD systems. Make sure
that `Bash`_ is installed and used to execute the configure
script. `Bash`_ should also be used for generating the configure
script by the way of autogen.sh.

`GNU Make`_ should be used instead of the native make. `GNU Make`_ can be
installed from ports and is as far as I know is called "gmake".

Most of the build process appears to work using the native FreeBSD
make with the exception of running unittests. One of the steps leading
up to executing the tests is not executed (creating files and
directories) and makes two of the unittests to fail.

If make fails on FreeBSD (observed on 6.2) with

::

 "libbtgcore.so: undefined reference to `backtrace_symbols'"

and the libexecinfo (provides the execinfo.h header) package is
installed, use the following linker flags before running the configure
script:

::

  LDFLAGS="-L/usr/local/lib -lexecinfo"

Notice that the native FreeBSD dialog used by btg-config does not work
- it does not implement --fselect. So use cdialog from ports instead.

.. _Tcsh: http://www.tcsh.org
.. _Bash: http://www.gnu.org/software/bash/
.. _GNU Make: http://www.gnu.org/software/make/

Rasterbar Libtorrent Versions
=============================

One can use one of the following `Rasterbar Libtorrent`_ versions with
the trunk the BTG SVN repository:

- 0.12.x.
- 0.13rc1-3.
- SVN.

Supported Directory Structure
=============================

When adding an user with the btgpasswd utility make sure that names of
the four directories are unique. 

For example, the following four directories could be used for an
imaginary user "sarah".

::

  /home/user/btg/users/sarah/temp
  /home/user/btg/users/sarah/work
  /home/user/btg/users/sarah/seed
  /home/user/btg/users/sarah/finished

When the user creates a session and uses one of the clients to open a
torrent file, the client will upload the file to the daemon. The
daemon will write the received torrent file in
"/home/user/btg/users/sarah/temp" and start downloading.

While the daemon is downloading, the contents of the torrent will be
written to "/home/user/btg/users/sarah/work".

When the torrent gets downloaded 100% and starts seeding, its data
will be moved from "/home/user/btg/users/sarah/work" to
"/home/user/btg/users/sarah/seed" where it will be untill the user
chooses to clean it. 

When the user chooses to clean the torrent, its data will be moved
from "/home/user/btg/users/sarah/seed" to
"/home/user/btg/users/sarah/finished". 

Only seeding torrents can be cleaned, To clean a torrent is equal to
abort it and move the files outside of the directories controlled by
BTG.

Once again, use unique directory names. 

Using the same directory for temp/work/seed/finished directory is not
supported and will result in undefined behaviour.

The directories belonging to an user should be on the same physical
disc or partition. Rasterbar libtorrent will not move files if it is
not the case.

Using a 3rd Party Client
========================

Instead of using the provided clients, one can use the daemon as
backend only.

A 3rd party client can be used, if it implements the commands used by
the daemon and uses one of the supported transports (TCP/IP or HTTP)
and externalization (XML-RPC).

The following explains how to generate the list of commands supported
by the daemon.

First make sure that BTG was given "--enable-command-list" as an
option to the configure script. This enables building an executable
which is used to generate a list of commands the daemon supports.

To generate the list of supported commands (after building BTG):

::

  cd doc/commands && make commanddoc

The above command will generate commands.rst and commands.html.

Using Commands
--------------

In the following, a few command sequences are listed. Notice that
error handling is left as an excersise for the reader.

The following sequence could be used to create a session:
::

 Command               Description
 --------------------- --------------------------
 (g) Init connection   Initialize the connection.
 (g) Ack               Response, OK.
 (g) Setup command     Setup the session.
 (g) Ack               Response, OK.

Having created a session, one can create a context(load a torrent):
::

 Command               Description
 --------------------- -----------------
 (g) Create context    Create a context.
 (g) Ack               Response, OK.

After a torrent has been loaded, it can be started. Also, one can get
its status, which contains information such as the upload/download
rate, number of percent finished and its state.

::

 Command               Description
 --------------------- -------------------------------
 (c) Start context     Start a torrent.
 (g) Ack               Response, OK.
 (c) Status            Get the status of one context.
 (c) Status response   The status of the context (OK).

Misc
====

Loading All Torrent Files from a Directory
------------------------------------------

Use the following command from a shell (like Bash):

::

 ls -1 *.torrent|xargs -n1 -ITORRENT btgcli -A -n -c "detach" -o "TORRENT"

The above commands works when using Debian Linux.

FreeBSD users should use the following command instead (thanks Andros):

::

 ls -1 *.torrent|xargs -n1 -o -ITORRENT btgcli -A -n -c "detach" -o "TORRENT"

(Tested on FreeBSD 6.2-RELEASE-p5.)

The above command executes the following actions for each torrent file
present in the current directory:

- start btgcli and attach to the first available session.
- upload a torrent file to the daemon.
- detach the session.

Notice that there can be slight differencies between which command
line arguments are supported by the different xargs versions.

Using an Incoming Directory
---------------------------

Using a script and cron, one can emulate an incoming directory, like
the one used by a bash script used to run `btlaunchmany.py`_.

.. _btlaunchmany.py: http://wiki.theory.org/BASH_script_to_run_bittorrent_as_a_daemon

::

 #!/bin/sh
 
 CLIENT=btgcli
 # The directory containing the torrent files.
 INCOMING_DIR=~/btg/incomming
 # The directory to which .torrent files are moved 
 # to after loading them into BTG. 
 DONE_DIR=~/btg/incomming/done

 GOT_SESSION=0
 $CLIENT -A -n -c "detach" &> /dev/null && GOT_SESSION=1

 if [ $GOT_SESSION -eq 0 ]
 then
   $CLIENT -n -c "detach" &> /dev/null && GOT_SESSION=1
 fi
 
 if [ $GOT_SESSION -eq 0 ]
 then
   echo "Unable to attach or create a BTG session."
   exit -1
 fi
 
 TORRENT_ADDED=0
 
 cd $INCOMING_DIR && \
 for f in `ls -1 *.torrent 2> /dev/null` ; do
   echo "Loading file: $f" && \
   $CLIENT -A -n -c "detach" -o $f &> /dev/null && \
   TORRENT_ADDED=`expr $TORRENT_ADDED + 1` && \
   mv $f $DONE_DIR
 done

 if [ $TORRENT_ADDED -gt 0 ]
 then
   echo "Added $TORRENT_ADDED torrents to BTG."
 fi

The above script attempts to:

- Create a new BTG session or attach to an existing BTG session.
- Load all present torrent files into BTG.
- Move any loaded torrent files to another directory.
- Write the filename of the torrent file which is being loaded into BTG.
- Write the total number of files loaded.

Remember to add a section named "auth" to your client.ini file. This
section should contain two keys: "username" and "password-hash". This
is done to avoid having the client (btgcli) prompt for username and
password, since its being used from a script called from cron.

Notice that once a torrent file is loaded by BTG, there is no reason
for keeping the torrent file. See `Supported Directory Structure`_.

Call the script from Vixie Cron, using an entry like this:

::

 */1 * * * *   /path/to/script 

The above entry calls the script each minute. Notice that cron will
e-mail the output of the script to you, so add "&> /dev/null" to avoid
any e-mail.

Setting global limits based on the time of the day
--------------------------------------------------

The following script could be used to set upload limits based on the time of the day.

::

 #!/bin/sh
 
 # The location of the BTG client application.
 CLIENT=btgcli
 
 H=`date +%H`
 O="none"
 
 if [ "$H" -gt "0" ] || [ "$H" -lt "6" ] 
 then
     O="night"
 fi
 
 if [ "$H" -gt "6" ] || [ "$H" -lt "12" ]
 then
     O="morning"
 fi
 
 if [ "$H" -gt "12" ] || [ "$H" -lt "16" ]
 then
     O="midday"
 fi
 
 if [ "$H" -gt "16" ] || [ "$H" -lt "23" ]
 then
     O="evening"
 fi
 
 # Max upload limit.
 UL_MAX=75
 
 # Global limits in KiB/sec.
 UL=-1
 DL=-1
 SET_LIMIT=0
 
 case "$O" in
     night)
 	UL=$UL_MAX
 	SET_LIMIT=1
 	echo "Limit:$O:$UL:$DL"
 	;;
     morning)
 	UL=`expr $UL_MAX - 20`
 	SET_LIMIT=1
 	echo "Limit:$O:$UL:$DL"
 	;;
     midday)
 	UL=`expr $UL_MAX - 40`
 	SET_LIMIT=1
 	echo "Limit:$O:$UL:$DL"
 	;;
     evening)
 	UL=`expr $UL_MAX - 70`
 	SET_LIMIT=1
 	echo "Limit:$O:$UL:$DL"
 	;;
     *) 
 	echo "Not setting limit."
 	;;
 esac
 
 if [ "$SET_LIMIT" -eq "0" ]
 then
     exit 0
 fi
 
 GOT_SESSION=0
 $CLIENT -A -n -c "detach" &> /dev/null && GOT_SESSION=1
 
 if [ $GOT_SESSION -eq 0 ]
 then
   $CLIENT -n -c "detach" &> /dev/null && GOT_SESSION=1
 fi
 
 if [ $GOT_SESSION -eq 0 ]
 then
   echo "Unable to attach or create a BTG session."
   exit -1
 fi
 
 $CLIENT -A -n -c "glimit $UL $DL -1 -1;detach" &> /dev/null && \
 echo "Limit set."

Add it to cron like the script used to load torrents from an incoming directory.

OpenWrt
=======

This section describes how to build BTG for use with `OpenWrt`_
kamikaze. Only the daemon is built and please do note that this
procedure is highly experimental.

Check out the required software:

 - BTG SVN in ~/remote-svn/btg.
 - OpenWrt SVN into ~/remote-svn/openwrt (buildroot).
 - OpenWrt package SVN in ~/remote-svn/openwrt-packages.

The method of building a working OpenWrt is described `elsewhere`_.

Make sure that Bjam is installed and working. It is used for building
boost.

Create symbolic links between the contents of
~/remote-svn/openwrt-packages and
~/remote-svn/openwrt/packages. Finally, link the directories in
~/remote-svn/btg/openwrt to ~/remote-svn/openwrt/packages.

The results in ~/remote-svn/openwrt/packages should look like:

::

  bmon -> ../../openwrt-packages/net/bmon
  boost -> ~/remote-svn/btg/openwrt/boost
  ..
  btg -> ~/remote-svn/btg/openwrt/btg
  rarpd -> ../../openwrt-packages/net/rarpd
  ..
  rblibtorrent -> ~/remote-svn/btg/openwrt/rblibtorrent
  ..
  zlib

Note that PKG_SOURCE and PKG_SOURCE_URL in
~/remote-svn/openwrt/packages/btg/Makefile and
~/remote-svn/openwrt/packages/rblibtorrent/Makefile should be
corrected, as the provided ones point to Rasterbar Libtorrent and BTG
versions used strictly for testing.

Now, configure OpenWrt by executing:

:: 

  make menuconfig

Select Network/Bittorrent/BTG. This selects the packages BTG depends
on, like Boost and Rasterbar libtorrent.

Save the config and build packages using:

::

  make V=99

The compiled packages will be in ~/remote-svn/openwrt/bin/packages, if
the build is successful. Copy the Boost, Rasterbar Libtorrent and BTG
packages to the device running OpenWrt and install them using ipkg.

.. _OpenWrt: http://openwrt.org/
.. _elsewhere: http://downloads.openwrt.org/kamikaze/docs/openwrt.html#x1-310002

Revisions
---------

The following OpenWrt revisions are known to build:

======================= ========================== ===========
**OpenWrt trunk**       **OpenWrt packages trunk** **BTG SVN**
----------------------- -------------------------- -----------
10359                   10359                      210 (0.9.7)
11011                   11011                      328       
======================= ========================== ===========

Revision 328 Notes
------------------

Uses boost 1.34.1, libtorrent 0.13 and BTG from the trunk of SVN.

GCC 4.2.3 is used because other versions had problems compiling
libtorrent 0.13. To be able to build revision 328 for the
XScale-IXP42x, the following had to be used in the .config file.

::

 CONFIG_TARGET_OPTIMIZATION="-Os -pipe -march=armv5te -mtune=xscale -funit-at-a-time"
 CONFIG_GCC_VERSION="4.2.3"
 CONFIG_UCLIBC_VERSION="0.9.29"

Credits
=======

Original howto, translated from swedish written by: donnex with help
of unclear, and jstrom.

.. footer:: `BTG home page`_
.. _BTG home page: http://btg.berlios.de/

