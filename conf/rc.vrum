#! /bin/sh
#
# rc.vrum (vreng reflector unicast-multicast)
#
PATH=.:bin:/usr/local/bin:$PATH export PATH

VRUM_HOME=~/vreng/src/servers/vrum

if [ -d ${VRUM_HOME} ]; then
  cd ${VRUM_HOME}
  ./vrum.py >/dev/null 2>&1 &
  echo vrum > /dev/console
else
  cd /usr/local/bin
  if [ -x vrum.py ]; then
    ./vrum.py &
    echo vrum > /dev/console
  fi
fi
