#!/bin/sh

. config.sh
. util.sh

TEST="create_100_sessions"

# Create 100 sessions.

counter=0
LIMIT=10

NUMBERS="0 1 2 3 4 5 6 7 8 9"

while [ "$counter" -lt "$LIMIT" ]
do
  echo "Creating 10 sessions ($counter)."

  for number in `echo $NUMBERS`
  do
    $BTGCLI_CMD -n -c "detach" &
  done

  sleep 3

  counter=`expr $counter + 1`
done

# Quit all the sessions.

counter=0
LIMIT=100

while [ "$counter" -lt "$LIMIT" ]
do
  echo "Quitting session $counter."

  $BTGCLI_CMD -n -A -c "quit"
  STATUS=$?

  checkExitCode "$TEST" "quit session $counter" $STATUS

#  sleep 1

  counter=`expr $counter + 1`
done

writeSuccess $TEST
