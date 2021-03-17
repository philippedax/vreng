#! /bin/bash
#
# wind.cgi
#
# Philippe Dax - May 2008
#
echo "Content-Type: text/html"
echo ""

#echo "<html><body>"
/usr/share/bin/rain | cut -f2 -d',' | sed -e 's/mm//' -e 's/ //' -e 's/--//'
#echo "</body></html>"
