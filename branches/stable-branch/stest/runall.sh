#!/bin/sh

./gen_btg_config.sh
./test_btgpasswd.sh

./gen_btg_config.sh
./test_create_sessions.sh

./gen_btg_config.sh
./test_limits.sh

./gen_btg_config.sh
./test_users.sh

./gen_btg_config.sh
./test_session_stress.sh

