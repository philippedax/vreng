#! /bin/sh
#
# httpd.sh
#
# Configures and starts local httpd server
#
# Philippe Dax - jun 2017
#
conf=/etc/apache2/httpd.conf

p=$(basename $0)

if [ ! -f $conf ]; then
  echo "$p: $conf not found"
  exit 1
fi

if grep "vreng" $conf >/dev/null 2>&1; then
  echo "$p: httpd.conf already configured"
  exit 0
fi

echo "$p: httpd.conf not configured"
tmp=/tmp/httpd
cp $conf $tmp

case $(uname -s) in
  Darwin)	# MacOS
    path=$HOME/Sites/
    ;;
  *)		# Linux
    path=$HOME/public_html/
    ;;
esac

cat <<EOF >>$tmp
# vreng configuration added at the end of httpd.conf
LoadModule include_module libexec/apache2/mod_include.so
LoadModule userdir_module libexec/apache2/mod_userdir.so
LoadModule rewrite_module libexec/apache2/mod_rewrite.so

<DirectoryMatch "$path">
  Require all granted
</DirectoryMatch">
<Directory "$path">
  Options Indexes MultiViews FollowSymLinks
  AllowOverride All
  Require all granted
</Directory>

Include /private/etc/apache2/extra/httpd-userdir.conf
EOF

sudo cp $tmp $conf
sudo apachectl restart
