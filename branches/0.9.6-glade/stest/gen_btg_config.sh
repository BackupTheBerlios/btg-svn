#!/bin/sh

. config.sh

DAEMON_CONFIG="daemon.ini"
DAEMON_PASSWD="passwd"
CLIENT_CONFIG="client.ini"
USERNAME="test"

# Clean up old config file.

rm -fr user
rm -f  $DAEMON_CONFIG $DAEMON_PASSWD $CLIENT_CONFIG

# Create a dir used for keeping torrent files.
# 

mkdir $SOURCES &> /dev/null

# Write a daemon config file.
#

echo "; Test daemon config" > $DAEMON_CONFIG

echo "[transport]
default=xml-rpc

[network]
listen-to=0.0.0.0:16000
; 200 ports, in case DHT is enabled.
torrent-range=10024:10224
use-upnp=false

[logging]
type=stdout

[savesessions]
enable=false

[auth]
passwd-file=`pwd`/$DAEMON_PASSWD
" >> $DAEMON_CONFIG

# Create the directories used by the user test, created next.
# 
TEMPDIR="`pwd`/user/$USERNAME/temp"
WORKDIR="`pwd`/user/$USERNAME/work"
SEEDDIR="`pwd`/user/$USERNAME/seed"
FINIDIR="`pwd`/user/$USERNAME/fini"

mkdir -p $TEMPDIR $WORKDIR $SEEDDIR $FINIDIR

# Create a password file.
# user test, password hash for "test".
# 
echo "$USERNAME:a94a8fe5ccb19ba61c4c0873d391e987982fbbd3:$TEMPDIR:$WORKDIR:$SEEDDIR:$FINIDIR:0:!" > `pwd`/$DAEMON_PASSWD

counter=0;
LIMIT=100

while [ "$counter" -lt "$LIMIT" ]
do
  USER="$USERNAME$counter"

#  echo "Creating user $username"

  TEMPDIR="`pwd`/user/$USER/temp"
  WORKDIR="`pwd`/user/$USER/work"
  SEEDDIR="`pwd`/user/$USER/seed"
  FINIDIR="`pwd`/user/$USER/fini"

  echo "$USER:a94a8fe5ccb19ba61c4c0873d391e987982fbbd3:$TEMPDIR:$WORKDIR:$SEEDDIR:$FINIDIR:0:!" >> `pwd`/$DAEMON_PASSWD

  mkdir -p $TEMPDIR $WORKDIR $SEEDDIR $FINIDIR

  counter=`expr $counter + 1`
done

# Write a client config file.
# 

echo "; Test client config." >> $CLIENT_CONFIG

echo "[transport]
default=xml-rpc

[network]
daemon-address=127.0.0.1:16000

[auth]
username=$USERNAME
password-hash=a94a8fe5ccb19ba61c4c0873d391e987982fbbd3

[misc]
never-ask=true
" >> $CLIENT_CONFIG
