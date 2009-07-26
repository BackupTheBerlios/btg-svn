#!/bin/sh

. config.sh
. util.sh

TEST="session_stress"

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
# 

NUMBERS="0 1 2"

for number in `echo $NUMBERS`
do
  $BTGCLI_CMD -n -o "${FILES[number]}" --command "detach"
  STATUS=$?

  checkExitCode "$TEST" "create session $number" $STATUS
done

# Send 3 kinds of commands to the 3 sessions.
# 

counter=0
LIMIT=99

SESSION_CMD0="ls; ss 0; ss all; info 0; info all; stop 0; start 0; stop all; start all; clean 1; detach"
SESSION_CMD1="info 0; info 0; info 0; info 0; info 0; info 0; info 0; info 0; info 0; info 0; detach"
SESSION_CMD2="ss 0; ss all; ss 0; ss all; ss 0; ss all; ss 0; ss all; ss 0; ss all; ss 0; ss all; detach"

while [ "$counter" -lt "$LIMIT" ]
do

  for number in `echo $NUMBERS`
  do
    SESSION=`expr $number + 1`

    # Commands, 1:
    $BTGCLI_CMD -n -a -s $SESSION --command "$SESSION_CMD0"
    STATUS=$?
    checkExitCode "$TEST" "Session $SESSION($counter), commands 1" $STATUS

    # Commands, 2:
    $BTGCLI_CMD -n -a -s $SESSION --command "$SESSION_CMD1"
    STATUS=$?
    checkExitCode "$TEST" "Session $SESSION($counter), commands 2" $STATUS

    # Commands, 3:
    $BTGCLI_CMD -n -a -s $SESSION --command "$SESSION_CMD2"
    STATUS=$?
    checkExitCode "$TEST" "Session $SESSION($counter), commands 3" $STATUS
  done

  counter=`expr $counter + 1`
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
