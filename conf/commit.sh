#! /bin/sh
#
# commit message
#
# Philippe Dax - avr 2017
#
p=$(basename $0)

case $# in
0)
  echo "usage: $p message"
  exit 0
  ;;
1)
  message=$1
  ;;
esac

hist=conf/commits
chmod 644 $hist
if [ -f $hist ] ; then
  last=$(tail -1 $hist)
  num=$(echo $last | cut -f1 -d ' ')
  num=$[ $num + 1 ]
else
  num=1
  touch $hist
fi
echo "$num $(date)	$message" >> $hist
echo "$num $(date)	$message"

# commit
git commit -a -m "$message"

# push to github
git push origin master