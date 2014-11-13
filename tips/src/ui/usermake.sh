#!/bin/bash
(export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/i386-linux-gnu/:/lib/i386-linux-gnu/:/usr/local/tipschecker/bin/ui
make -j4)
