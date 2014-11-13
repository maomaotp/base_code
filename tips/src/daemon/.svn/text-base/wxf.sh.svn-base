#!/bin/bash

(cd /usr/local/tipschecker/bin/cache
./tipscache &)
sleep 1
(cd /usr/local/tipschecker/bin/Audit
./tipsauditor &)
sleep 1
(cd /usr/local/tipschecker/bin/Alarm
./tipsalarm &)
sleep 1
(export LD_LIBRARY_PATH:/usr/local/lib
cd /usr/local/tipschecker/bin/netblock 
./tipsnetblock &)
sleep 1
(export LD_LIBRARY_PATH:/usr/local/lib
cd /usr/local/tipschecker/bin/mngagent
./tipsmanager &)
sleep 1
(cd /usr/local/tipschecker/bin/testNetwork
./tipsstat &)
sleep 1
(cd /usr/local/tipschecker/bin/ui
./start.sh)
