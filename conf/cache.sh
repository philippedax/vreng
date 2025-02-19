#! /bin/sh
#
# cache.sh
#
# Sets in cache some important files
#
# Philippe Dax - feb 2025
#
data=../data
cache=$HOME/.vreng/cache

p=$(basename $0)

if [ ! -d $data ]; then
  echo "$p: $data not found"
  exit 1
fi
if [ ! -d $cache ]; then
  echo "$p: $cache not found"
  exit 1
fi

cp $data/vre/* $cache
cp $data/gif/marble.gif $cache
cp $data/gif/mythology.gif $cache
cp $data/gif/tarots.gif $cache
cp $data/gif/beach.gif $cache
cp $data/gif/dax.gif $cache
cp $data/jpg/meeting.jpg $cache
cp $data/txf/courier-br-46.txf $cache

echo "$p: done"
