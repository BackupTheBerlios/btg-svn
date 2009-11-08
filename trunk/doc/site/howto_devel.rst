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

Trunk is being used for development. Do not use the stable-branch.

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

Connecting to the daemon
========================

One can use the following script to connect to the daemon using
XML-RPC. The transport in daemon.ini needs to be set to "xml-rpc".

::

 #!/usr/bin/python
 
 import xmlrpclib
 
 uri = "http://localhost:16000"
 p = xmlrpclib.ServerProxy(uri)
 # Command id of the sent command.
 command_id = 1
 p.general.initconnection(command_id, "user1", "hash of user1's password")
 
The above script sends one command, `general.initconnection`_. This
initialises the connection from a client to the daemon.

In the following, some of the more basic commands used to communicate
with the deamon are explained in some detail.

The daemon will send either `general.ack`_ or `general.error`_ back. The
ack command contains the ID of the command it acknowledges. The error
command contains the ID of the command it was caused by and a string
describing the error.

If the daemon sent a `general.ack`_ command back, one can create a new
session or attach to an existing session.

To create a new session, transmit `general.setup`_ to the daemon. It
will return `general.setuprsp`_, which contains a sesion ID used to
refer to this session. Or it returns an error, if the session cannot
be created.

To attach to an existing session, one has to know the session ID
first. To obtain the session IDs from the daemon, one has to transmit
`session.list`_ to it. The daemon will transmit a `session.listrsp`_
command back, which will contain a list of sessions.

A client uses `session.attach`_ to attach to a session with a certain
ID. The daemon will send a `general.ack`_ back to inform the client
that it is attached. More than one client can be attached to the same
session at the same time.

If the client is not allowed to attach to a session, it will receive
`general.error`_ in this situation.

Once a client created or attached to a session, it can start adding
torrents to the session. This can be done using
`context.createwithdata`_, `context.createfromfile`_ or
`context.createfromurl`_.

`context.createwithdata`_ adds a context (which is a torrent
identified by an ID) only using the information contained in the
command.

`context.createfromfile`_ creates a context by receiving parts of a
torrent file until all parts are received.  `context.createfromurl`_
downloads an URL.

`context.createfromurl`_ is async. The daemon returns an identifier
using `context.createfromurlrsp`_, which can be used to query the
status of the operation using `general.opstatus`_ or abort it using
`general.opabort`_.

A list of context IDs / file names can be obtained using
`general.list`_. Once the client has this list, it can query the
status of a context using `context.status`_. The daemon will send a
`context.statusrsp`_ back. This status contains information about how
much has been downloaded, download rate etc..

Some common oprations on a torrent are 

- start, using `context.start`_.
- stop, using `context.stop`_.
- abort/erase, using `context.abort`_.

Detaching from a session is done by sending `session.detach`_ to the
deamon.

This was a quick introduction to the commands used to interact with
the daemon. Not all commands were covered. The easiest way to learn
more about how they interact with the deamon is to observe the verbose
log produced by the daemon. It shows which commands are received/sent
to a client.

.. _general.initconnection: commands.html#g-init-connection
.. _general.ack: commands.html#g-ack
.. _general.error: commands.html#g-error
.. _general.setup: #g-setup-command
.. _general.setuprsp: #g-setup-command-response
.. _session.list: commands.html#s-list
.. _session.listrsp: commands.html#s-list-response

.. _session.attach: commands.html#s-attach
.. _general.list: commands.html#g-list-request
.. _session.detach: commands.html#s-attach

.. _context.statusrsp: commands.html#c-status-response

.. _context.createwithdata: commands.html#g-create-context
.. _context.createfromfile: commands.html#c-create-context-from-file
.. _context.createfromurl: commands.html#c-create-context-from-url

.. _context.createfromurlrsp: commands.html#c-create-context-from-url-response
.. _general.opstatus: commands.html#g-status-of-operation
.. _general.opabort: commands.html#g-abort-operation
.. _context.status: commands.html#c-status

.. _context.start: commands.html#c-start-context
.. _context.stop: commands.html#c-stop-context
.. _context.abort: commands.html#c-abort-context

List of commands
================

The list of commands is available `here`_ (generated from sources in the
trunk of SVN).

.. _here: commands.html

Context Commands
================

Commands which inherit from btg::core::contextCommand can be
constructed with an all-context flag set. This means that the daemon,
when it receives such a command, will send a reply for all contexts in
the session the command belongs to.

The all-context flag is not supported for some of the commands. This
will for example not work with Command::CN_CPEERS and
Command::CN_CFILEINFO - the daemon will not execute such commands. The
reason is that the amount of data to transfer would be enormous for
sessions with a large amount of torrents.

Patches
=======

Patches can be easily made using "svn diff > file.diff" from trunk.

.. footer:: `BTG home page`_
.. _BTG home page: http://btg.berlios.de/

