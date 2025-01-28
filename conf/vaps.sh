#! /bin/sh
#
# rc.vaps (VREng Anination Parameters Server)
#
# Philippe Dax - Jul 2001
#
VRENG_HOME=/Users/dax/vreng

PATH=$VRENG_HOME:$PATH export PATH

VAPS_DIR=${VRENG_HOME}/data/bap
VAPS_LOG=${VRENG_HOME}/log/vaps.log

vaps -d ${VAPS_DIR} -l ${VAPS_LOG}
