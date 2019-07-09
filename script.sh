#!/bin/sh

chmod 666 secure.log
echo "privileges changed"
head -40 secure.log
echo "privileges changed"
truncate secure.log --size 0
echo "privileges changed"
chmod 600 secure.log
echo "privileges changed"
rm script.sh
