#!/bin/sh

. config.sh
. util.sh

TEST="create_100_sessions_differnet_users"

# Create 100 sessions, 
# each with a different user.

CLIENT_CONFIG="user_config.ini"

CLIENT_CONFIG_UNCHANGING="
[transport]
default=xml-rpc

[network]
daemon-address=127.0.0.1:16000

[misc]
never-ask=true
"

counter=0
LIMIT=100

while [ "$counter" -lt "$LIMIT" ]
do
  echo "; Test client config." > $CLIENT_CONFIG

  echo "$CLIENT_CONFIG_UNCHANGING" >> $CLIENT_CONFIG

  echo "[auth]
  username=test$counter
  password-hash=a94a8fe5ccb19ba61c4c0873d391e987982fbbd3
" >> $CLIENT_CONFIG

  $BTGCLI_CMD_BARE --config $CLIENT_CONFIG -n -c "detach" 

  STATUS=$?
  checkExitCode "$TEST" "create session $counter" $STATUS

  counter=`expr $counter + 1`
done

counter=0

while [ "$counter" -lt "$LIMIT" ]
do

  echo "Creating client.ini ($counter)."

  echo "; Test client config." > $CLIENT_CONFIG

  echo "$CLIENT_CONFIG_UNCHANGING" >> $CLIENT_CONFIG

  echo "
  [auth]
  username=test$counter
  password-hash=a94a8fe5ccb19ba61c4c0873d391e987982fbbd3
" >> $CLIENT_CONFIG

  $BTGCLI_CMD_BARE --config $CLIENT_CONFIG -n -A -c "quit" 

  STATUS=$?
  checkExitCode "$TEST" "quit session $counter" $STATUS

  counter=`expr $counter + 1`
done

rm -f $CLIENT_CONFIG

writeSuccess $TEST
