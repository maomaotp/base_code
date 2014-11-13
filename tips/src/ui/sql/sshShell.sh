#!/bin/bash
# Program:
#   This program is created by LiNingJie.

echo -e "client is connecting the server \n"
ssh-keyscan -p 22 -t rsa,dsa 60.247.88.46 > /root/.ssh/known_hosts
ssh -NfR 6300:localhost:22 root@60.247.88.46 -p 22
echo -e "ok!finish.\n"
exit 0
