Customize yourself
------------------

Copy the file prefs into ~/.vreng/ and edit it to customize yourself.


VREng by Sdr
------------

Copy the present sdr plugins in your ~/.sdr/plugins directory.


VREng by the Web
----------------

To launch VREng in your favorite browser, do:

1- Append the content of mailcap into /usr/local/lib/netscape/mailcap

2- Append the content of mime.types into /usr/local/lib/netscape/mime.types
   and into /your_httpd_top/httpd/conf/mime.types

3- Kill and restart your browser

4- Kill -HUP <httpd_pid>

5- Try to get "sdr_id" by http://your_site/yout_path/sdr_id.vreng.
   "sdr_id" is the file name found in your ~/.sdr/cache/ or ~/sdr/cache
   which contains the SDP announce of VREng.
