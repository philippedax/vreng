#! /bin/sh
mkdir log 2>&1 >/dev/null
date 2>&1 >log/date.log
cd support
./autogen.sh "$@"
cd ..
make
date 2>&1 >>log/date.log
cat log/date.log
