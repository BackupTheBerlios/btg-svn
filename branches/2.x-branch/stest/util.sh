writeSuccess () {
  NAME=$1

  echo "Test $NAME PASSED."
}

writeFailure () {
  NAME=$1

  echo "Test $NAME FAILED."
}

writeMessage () {
  NAME=$1
  MESSAGE=$2

  echo "Test $NAME: $MESSAGE."
}

checkExitCode () {
  NAME=$1
  WHAT=$2
  STATUS=$3

  if [ $STATUS -ne 0 ]; then
    echo "Encountered an error in $NAME: $WHAT."
    writeFailure $NAME
    exit 0
  else
    echo "$NAME: $WHAT - OK."
  fi
}

checkAssert () {
  NAME=$1
  WHAT=$2
  EXPECTED=$3
  ACTUAL=$4

  if [ $ACTUAL -ne $EXPECTED ]; then
    echo "Encountered an error in $WHAT: ($ACTUAL == $EXPECTED)."
    writeFailure $NAME
    exit 0
  else
    echo "$NAME: Assert ($WHAT) - OK."
  fi
}
