#!/bin/bash

run(){ mysql --password=beep < $1; }

cat create.sql procedure.sql insert.sql > final.sql
run final.sql
