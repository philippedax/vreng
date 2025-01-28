#! /bin/sh
#
# start-daemons
#
# Philippe Dax - jun 2017
#

./X11.sh
./httpd.sh

nohup ./vacs.sh &
nohup ./vaps.sh &
nohup ./vjs.sh &
nohup ./vrum.sh &
