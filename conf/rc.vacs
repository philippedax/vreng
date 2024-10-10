#! /bin/sh
#
# rc.vacs (VREng Address Cache Server)
#
VRENG_HOME=/Users/dax/vreng

PATH=$VRENG_HOME:$PATH
export PATH

VACS_HOME=${VRENG_HOME}/data/vacs

if [ -d ${VACS_HOME} ]; then
	cd ${VACS_HOME}
	vacs -l ${VRENG_HOME}/log/vacs.log
	echo vacs > /dev/console
fi
