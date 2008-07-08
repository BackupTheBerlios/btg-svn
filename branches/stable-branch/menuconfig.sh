#!/bin/sh

if [! -x "$DIALOG"]
then
  echo "This script needs dialog to function. Abort."
  exit -1
fi

TEMP=/tmp/btg.menuconfig.out

CONFIG_ARGUMENTS=""

dialog --title "BTG configuration" \
--checklist "Select compile time options:" 13 60 6 \
    1 "Enable CLI" 'on' \
    2 "Enable GUI" 'on' \
    3 "Enable WWW UI" 'on' \
    4 "Enable session saving" 'on' \
    5 "Enable event callback" 'off' \
    6 "Enable debug" 'off' \
    7 "Enable unittest" 'off' \
    8 "Enable URL loading by the daemon" 'on' \
    9 "Enable UPnP" 'off' \
   10 "Enable btg-config script build" 'on' \
2>$TEMP

if [ "$?" != "0" ] ; then 
  echo "Aborted."
  rm -f $TEMP
  exit ; 
fi

choice=`cat $TEMP|sed -e "s/\"//g"`

for arg in $choice
do
  case $arg in
    "1") CONFIG_ARGUMENTS="$CONFIG_ARGUMENTS --enable-cli";;
    "2") CONFIG_ARGUMENTS="$CONFIG_ARGUMENTS --enable-gui";;
    "3") CONFIG_ARGUMENTS="$CONFIG_ARGUMENTS --enable-www";;
    "4") CONFIG_ARGUMENTS="$CONFIG_ARGUMENTS --enable-session-saving";;
    "5") CONFIG_ARGUMENTS="$CONFIG_ARGUMENTS --enable-event-callback";;
    "6") CONFIG_ARGUMENTS="$CONFIG_ARGUMENTS --enable-debug";;
    "7") CONFIG_ARGUMENTS="$CONFIG_ARGUMENTS --enable-unittest";;
    "8") CONFIG_ARGUMENTS="$CONFIG_ARGUMENTS --enable-url";;
    "9") CONFIG_ARGUMENTS="$CONFIG_ARGUMENTS --enable-upnp";;
    "10") CONFIG_ARGUMENTS="$CONFIG_ARGUMENTS --enable-btg-config";;
  esac
done

dialog --title "BTG configuration" \
--msgbox "This script is ready to run configure with the following options \"$CONFIG_ARGUMENTS\"" 13 60

if [ "$?" != "0" ] ; then 
  echo "Aborted."
  rm -f $TEMP
  exit ; 
fi

echo "./configure $CONFIG_ARGUMENTS"
./configure $CONFIG_ARGUMENTS

rm -f $TEMP
