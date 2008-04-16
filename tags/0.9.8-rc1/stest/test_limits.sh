#!/bin/sh

. config.sh
. util.sh

TEST="limits"

# Find the torrent files.

INPUT=`ls -1 sources/*.torrent|tr "\n" " "`
NUM_FILES=`ls -1 sources/*.torrent|wc -l`

checkAssert $TEST "Number of torrent files" 3 $NUM_FILES

for file in `echo $INPUT`
do
  FILES[$COUNTER]=$file
  COUNTER=`expr $COUNTER + 1`
done

# if [ $STATUS -ne 3 ]; then
# fi

# Create 3 sessions.

counter=0
LIMIT=10

NUMBERS="0 1 2"

for number in `echo $NUMBERS`
do
  $BTGCLI_CMD -n -o "${FILES[number]}" --command "detach"
  STATUS=$?

  checkExitCode "$TEST" "create session $number" $STATUS
done

# Set 1K limits.

# Try this rates, in KiB/sec.
# 
RATES="1 3 5 10 13 25 50 100 200 300 512 1024 5000"

for rate in `echo $RATES`
do

  for number in `echo $NUMBERS`
  do
    SESSION=`expr $number + 1`
    $BTGCLI_CMD -n -a -s $SESSION --command "limit all $rate $rate -1 -1; detach"
    STATUS=$?

    checkExitCode "$TEST" "Limit session $SESSION, rate $rate KiB/sec" $STATUS

  done

  writeMessage "$TEST" "Sleeping for 1 sec."
  sleep 1

done

# Destroy the sessions.
# 

for number in `echo $NUMBERS`
do
    SESSION=`expr $number + 1`
    $BTGCLI_CMD -n -a -s $SESSION --command "quit"
    STATUS=$?

    checkExitCode "$TEST" "Quit session $SESSION" $STATUS

done

writeSuccess $TEST
