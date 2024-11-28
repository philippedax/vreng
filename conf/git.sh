#! /bin/sh
#
# git.sh
#
# git operation
#  - git commit "message"
#  - git merge "branch"
#
# Philippe Dax - nov 2024
#
p=$(basename $0)

# print usage
usage()
{
  echo "usage: $p commit \"message\""
  echo "       $p merge \"branch\""
  exit 0
}

# append message to log
log()
{
  message=$1
  hist=conf/commits
  chmod 644 $hist
  num=1
  if [ -f $hist ] ; then
    num=$(echo $(tail -1 $hist) | cut -f1 -d ' ')
    num=$[ $num + 1 ]
  fi
  echo "$num $(date)	$message" >> $hist
  echo "$num $(date)	$message"
}

# commit
commit()
{
  message=$1
  git commit -a -m "$message"
}

# push to github
push()
{
  git push origin master
}

# merge branch
merge()
{
  branch=$1
  git pull https://github.com/${branch}/vreng.git master
  git checkout master
  git merge --no-ff ${branch}-master	# no fast-forward
}

# main
main()
{
  case $# in
  2)
    case $1 in
    c|commit)
      commit $2
      log $2
      push
      ;;
    ;;
    m|merge)
      merge $2
      push
      log "merge $2"
    ;;
    esac
  *)
    usage
    ;;
  esac

  exit 0
}
