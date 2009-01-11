#!/bin/sh

. config.sh
. util.sh

TEST="btgpasswd"

# Add a new user.
# 

DESTDIR=user
USERNAME=test2

mkdir $DESTDIR/$USERNAME
TEMP=$DESTDIR/$USERNAME/temp
WORK=$DESTDIR/$USERNAME/work
SEED=$DESTDIR/$USERNAME/seed
FINI=$DESTDIR/$USERNAME/fini

$BTGPASSWD_CMD -a -u $USERNAME -t $TEMP -w $WORK -s $SEED -d $FINI --control 1

STATUS=$?

checkExitCode $TEST "adding users" $STATUS

# Modify the user which was added before.
# 

TEMP2=$DESTDIR/$USERNAME/temp2
WORK2=$DESTDIR/$USERNAME/work2
SEED2=$DESTDIR/$USERNAME/seed2
FINI2=$DESTDIR/$USERNAME/fini2

$BTGPASSWD_CMD -m -u $USERNAME -w $WORK2 -t $TEMP2 -s $SEED2 -d $FINI2 --control 0

STATUS=$?

checkExitCode $TEST "modyfing users" $STATUS

# An user was modified,
# check that the paths were actually set.
# 

TEMP2_COUNT=`$BTGPASSWD_CMD -l|grep $TEMP2 passwd |wc -l`

if [ $TEMP2_COUNT -ne 1 ]; then
    writeFailure $NAME
    exit 0
fi

WORK2_COUNT=`$BTGPASSWD_CMD -l|grep $WORK2 passwd |wc -l`

if [ $WORK2_COUNT -ne 1 ]; then
    writeFailure $NAME
    exit 0
fi

SEED2_COUNT=`$BTGPASSWD_CMD -l|grep $SEED2 passwd |wc -l`

if [ $SEED2_COUNT -ne 1 ]; then
    writeFailure $NAME
    exit 0
fi

FINI2_COUNT=`$BTGPASSWD_CMD -l|grep $FINI2 passwd |wc -l`

if [ $FINI2_COUNT -ne 1 ]; then
    writeFailure $NAME
    exit 0
fi

# If the above worked, both add and modify seem to work.
# 

writeSuccess $TEST
