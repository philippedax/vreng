#! /bin/sh
#
# git.sh
#
# git operations:
#  - commit+push: git.sh commit "message"
#  - add:         git.sh add files
#  - push:        git.sh push
#  - merge:       git.sh merge "branch"
#
# Philippe Dax - nov 2024
#

p=$(basename $0)

#
# print usage
#
usage()
{
  echo "usage:"
  echo "    git.sh commit \"message\""
  echo "    git.sh add files"
  echo "    git.sh push"
  echo "    git.sh merge \"branch\""
  exit 0
}

#
# append message to commits log
#
log()
{
  mess=$*
  log=conf/commits.log
  chmod 644 $log
  num=1
  if [ -f $log ] ; then
    num=$(echo $(tail -1 $log) | cut -f1 -d ' ')
    num=$[ $num + 1 ]
  fi
  echo "$num $(date)	$mess" >> $log
  echo "$num $(date)	$mess"
}

#
# commit
#
commit()
{
  mess=$*
  git commit -a -m "$mess"
}

#
# add file
#
add()
{
  git add $*
}

#
# push to github
#
push()
{
  host github.com
  if [ $? = 0 ]; then
    git push origin master
  else
    echo "$p: do later \"$p push\""
  fi
}

#
# merge branch
#
merge()
{
  branch=$1
  git pull https://github.com/${branch}/vreng.git master
  git checkout master
  git merge --no-ff ${branch}-master	# no fast-forward
}

#
# main
#
main()
{
  case $# in
  0)
    usage
    ;;
  1)
    case $1 in
    push)		# push
      push
      ;;
    *)
      usage
      ;;
    esac
    ;;
  *)
    oper=$1
    case $oper in
    commit|c|co)	# commit
      shift
      mess=$*
      commit $mess
      log $mess
      push
      ;;
    add)		# add
      shift
      if [ test -f $1 ]; then
        files=$*
      else
        usage
      fi
      add $files
      ;;
    merge)		# merge
      branch=$2
      merge $branch
      push
      log "merge $branch"
      ;;
    *)			# bad
      echo "bad oper $oper"
      usage
      ;;
    esac
    ;;
  esac

  exit 0
}

main $@
