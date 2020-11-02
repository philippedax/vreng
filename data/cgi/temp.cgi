#! /bin/bash
#
# wind.cgi
#
# Philippe Dax - May 2008
#
echo "Content-Type: text/html"
echo ""

#echo "<html><body>"
/usr/share/bin/meteo | cut -f1 -d, 
#echo "</body></html>"
