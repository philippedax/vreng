#! /bin/sh
#
# merge branch
#
# Philippe Dax - dec 2023
#
p=$(basename $0)

case $# in
0)
  echo "usage: $p branch"
  exit 0
  ;;
1)
  branch=$1
  ;;
esac

git pull https://github.com/${branch}/vreng.git master
git checkout master
# no fast-forward
git merge --no-ff ${branch}-master
git push origin master

mkdir log 2>/dev/null
log=log/commits
if [ -f $log ] ; then
  last=$(tail -1 $log)
  num=$(echo $last | cut -f1 -d ' ')
  num=$[ $num + 1 ]
else
  num=1
  touch $log
fi
echo "$num $(date)	merge $branch" >> $log
echo "$num $(date)	merge $branch"
