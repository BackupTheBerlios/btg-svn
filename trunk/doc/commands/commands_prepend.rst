================
List of commands
================

.. contents:: Table of Contents 
   :depth: 1

This file specifies all the commands available between a BTG client
and the BTG server.

Each command has a name, a method name (used in XMLRPC transport), a
command ID, a direction, and a set of parameters.

Parameters can be of a specific type. This specification first lists
the internal type used, and then the XMLRPC type used.

In the case of Lists, there is first an int (Integer) specifying the
size of the list, then the elements are passed with their specific
type.

