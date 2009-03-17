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

Is the project accepting donations?
-----------------------------------

Yes, use the following button to donate a small amount to BTG development using Paypal.

.. raw:: html
  
  <form action="https://www.paypal.com/cgi-bin/webscr" method="post">
  <input type="hidden" name="cmd" value="_s-xclick">
  <input type="image" src="https://www.paypal.com/en_US/i/btn/btn_donate_SM.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">
  <img alt="" border="0" src="https://www.paypal.com/en_US/i/scr/pixel.gif" width="1" height="1">
  <input type="hidden" name="encrypted" value="-----BEGIN PKCS7-----MIIHdwYJKoZIhvcNAQcEoIIHaDCCB2QCAQExggEwMIIBLAIBADCBlDCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb20CAQAwDQYJKoZIhvcNAQEBBQAEgYCAN7qEsGnL3WdicvLzVbhZWq9Uy0j3UH+q6S1e6FArcZLyQiMewfjpwU9WlSnJtc+F27KGtT1te041dzPfA6n+fpD0sghncCthrBtgznrzBuWmXGH/Bj1uLNJnZ4rwPl2PoYDCSzNvrc6Q9JpuAFEXrkg2FNrplrupXun2+dbTJjELMAkGBSsOAwIaBQAwgfQGCSqGSIb3DQEHATAUBggqhkiG9w0DBwQIIEzVKkJmAV6AgdAyUiJ1dkTs60YFKPmQD7u3GwmDDfQdiPadhpsJsDsWSUWeY27rv/Au9lSlWN+xMi3QFuV+mEqPNXRorNBq/e1n6ndmBWV6aG7FgtYJbTOjVIz3uTSgUIna3CmcI5AdxkVXRahmsThqDQqDE8vl8mvrdBeg9z9DW8fyF6BY/C3jOJTJ00qpg/bRk5uzRtAX5OSmqOVV+Nc3GiBV8SUh9AQdKEYClH+O6IsVcLeqUMQHLVeGx/uD1nmfvPRNVMX5pKKk9qeWYOWurb9OHQzAa5PsoIIDhzCCA4MwggLsoAMCAQICAQAwDQYJKoZIhvcNAQEFBQAwgY4xCzAJBgNVBAYTAlVTMQswCQYDVQQIEwJDQTEWMBQGA1UEBxMNTW91bnRhaW4gVmlldzEUMBIGA1UEChMLUGF5UGFsIEluYy4xEzARBgNVBAsUCmxpdmVfY2VydHMxETAPBgNVBAMUCGxpdmVfYXBpMRwwGgYJKoZIhvcNAQkBFg1yZUBwYXlwYWwuY29tMB4XDTA0MDIxMzEwMTMxNVoXDTM1MDIxMzEwMTMxNVowgY4xCzAJBgNVBAYTAlVTMQswCQYDVQQIEwJDQTEWMBQGA1UEBxMNTW91bnRhaW4gVmlldzEUMBIGA1UEChMLUGF5UGFsIEluYy4xEzARBgNVBAsUCmxpdmVfY2VydHMxETAPBgNVBAMUCGxpdmVfYXBpMRwwGgYJKoZIhvcNAQkBFg1yZUBwYXlwYWwuY29tMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDBR07d/ETMS1ycjtkpkvjXZe9k+6CieLuLsPumsJ7QC1odNz3sJiCbs2wC0nLE0uLGaEtXynIgRqIddYCHx88pb5HTXv4SZeuv0Rqq4+axW9PLAAATU8w04qqjaSXgbGLP3NmohqM6bV9kZZwZLR/klDaQGo1u9uDb9lr4Yn+rBQIDAQABo4HuMIHrMB0GA1UdDgQWBBSWn3y7xm8XvVk/UtcKG+wQ1mSUazCBuwYDVR0jBIGzMIGwgBSWn3y7xm8XvVk/UtcKG+wQ1mSUa6GBlKSBkTCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb22CAQAwDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQUFAAOBgQCBXzpWmoBa5e9fo6ujionW1hUhPkOBakTr3YCDjbYfvJEiv/2P+IobhOGJr85+XHhN0v4gUkEDI8r2/rNk1m0GA8HKddvTjyGw/XqXa+LSTlDYkqI8OwR8GEYj4efEtcRpRYBxV8KxAW93YDWzFGvruKnnLbDAF6VR5w/cCMn5hzGCAZowggGWAgEBMIGUMIGOMQswCQYDVQQGEwJVUzELMAkGA1UECBMCQ0ExFjAUBgNVBAcTDU1vdW50YWluIFZpZXcxFDASBgNVBAoTC1BheVBhbCBJbmMuMRMwEQYDVQQLFApsaXZlX2NlcnRzMREwDwYDVQQDFAhsaXZlX2FwaTEcMBoGCSqGSIb3DQEJARYNcmVAcGF5cGFsLmNvbQIBADAJBgUrDgMCGgUAoF0wGAYJKoZIhvcNAQkDMQsGCSqGSIb3DQEHATAcBgkqhkiG9w0BCQUxDxcNMDgwOTE1MTkwMzU2WjAjBgkqhkiG9w0BCQQxFgQUPqDFWQUm82LTOZ5xPzrPrAtsCTkwDQYJKoZIhvcNAQEBBQAEgYBHarPP+IR2m2s6ijbPUbpk/ukovxj79ppFPxdCl6aFRlx19Ap0yb0a0tau/1xkont/wGMBVVp1+Z2mkLZTcGU73tqmvbNiG8M3CPhw2Vhf8L2wg6mUtyKW7l4lRdejGvLToEzz3wVQhVV9FNlIMme3DjceIG0xOTf9SO5zgCdBQQ==-----END PKCS7-----">
  </form>
  

How about adding feature X?
---------------------------

BTG accepts `patches`_. Make sure that its against the trunk of the SVN
repository.

BTG accepts donations, using Paypal. 

Making a donation that covers some of the time used adding a feature
will improve the chances that your feature will be added to BTG.

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
