#!/bin/sh

. config.sh

$DAEMON_PATH -v -n -c `pwd`/daemon.ini
