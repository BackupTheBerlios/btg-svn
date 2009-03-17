============
BTG Packages
============

Debian Packages
~~~~~~~~~~~~~~~

The debian packages are currently being maintained by Roman Rybalko. 
They can be found in our `debian repository`_. For easy installation, 
add the following line to your ``/etc/apt/sources.list``:

::

 deb ftp://ftp.berlios.de/pub/btg/debian main stable unstable
 deb ftp://ftp.berlios.de/pub/btg/debian other stable unstable

This repository is gpg-signed. This means apt can check the validity of the
packages against a gpg signature. You can find more information on gpg-signed 
repositories at the `the Debian wiki`_. To use this feature you must add the 
key to the apt keyring. To do that you must:

1. Download the `public key`_.
2. Verify that the md5sum of the file you downloaded is ``0773d61b03fda0204e9db4577ee9aa1b``
        ``md5sum public_key.asc``
3. Insert it into the apt keyring. As root, do the following:
        ``apt-key add public_key.asc``

.. _debian repository: ftp://ftp.berlios.de/pub/btg/debian/
.. _the Debian wiki: http://wiki.debian.org/SecureApt
.. _public key: ftp://ftp.berlios.de/pub/btg/debian/public_key.asc

Gentoo Packages
~~~~~~~~~~~~~~~

A Gentoo ebuild was added to `Project Sunrise`_ ("Gentoo User Overlay").

One has to do the following:
::

 emerge -av layman
 echo "source /usr/portage/local/layman/make.conf" >> /etc/make.conf
 layman -f -a sunrise
 emerge -av btg

.. _Project Sunrise: http://www.gentoo.org/proj/en/sunrise/

SUSE Packages
~~~~~~~~~~~~~

- `RPMs`_.
- `SUSE instructions`_.

.. _RPMs: http://software.opensuse.org/download/home:/darix/
.. _SUSE instructions: http://en.opensuse.org/Build_Service/User

.. footer:: `BTG home page`_
.. _BTG home page: http://btg.berlios.de/

Arch Linux Packages
~~~~~~~~~~~~~~~~~~~

- `AUR package and instructions`_.

.. _AUR package and instructions: http://aur.archlinux.org/packages.php?ID=8212

