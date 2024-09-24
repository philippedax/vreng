#! /bin/sh
mkdir log 2>&1 >/dev/null
date 2>&1 >log/$0.log
cd conf
./autogen.sh "$@"
cd ..
make
date 2>&1 >>log/$0.log
cat log/$0.log
