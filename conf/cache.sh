#! /bin/sh
#
# cache.sh
#
# Sets in cache some important files if more recent
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

# comparaison of dates and copy more recent to cache
comp()
{
  if [ $f1 -nt $f2 ] ; then
    cp -f $f1 $f2
    echo "cp $f1 $f2"
  fi
}

# direct comparaison of 2 files
compfile()
{
  f1=$data/$1
  f2=$cache/$(basename $1)
  comp $f1 $f2
}

# recursive comparaison in a directory
compdir()
{
  d=$1
  if [ -d $data/$d ]; then
    cd $data/$d
    for f in *
    do
      f1=$data/$d/$f
      f2=$cache/$f
      comp $f1 $f2
    done
  fi
}

compdir vre
compfile gif/marble.gif
compfile gif/mythology.gif
compfile gif/tarots.gif
compfile gif/beach.gif
compfile gif/dax.gif
compfile jpg/meeting.jpg
compfile txf/courier-br-46.txf

# obsoleted
#cp $data/vre/* $cache
#cp $data/gif/marble.gif $cache
#cp $data/gif/mythology.gif $cache
#cp $data/gif/tarots.gif $cache
#cp $data/gif/beach.gif $cache
#cp $data/gif/dax.gif $cache
#cp $data/jpg/meeting.jpg $cache
#cp $data/txf/courier-br-46.txf $cache

echo "$p: done"
