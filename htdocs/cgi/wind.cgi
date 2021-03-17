#! /bin/bash
#
# wind.cgi
#
# Philippe Dax - May 2008
#
echo "Content-Type: text/html"
echo ""

#echo "<html><body>"
/usr/share/bin/wind | cut -f2-3 -d: | sed -e 's/:/ /g' -e 's%km/h%%' 
#echo "</body></html>"
