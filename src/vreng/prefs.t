const char def_prefs[] = "\
#\n\
# my VREng config\n\
#\n\
\n\
# my body model\n\
# 5 available models : guy, humanoid, human, box, bbox, default=guy\n\
# model=guy\n\
# model=human\n\
# model=humanoid\n\
# model=box\n\
# model=bbox\n\
model=guy\n\
\n\
# my head model\n\
# 4 available models : lwo, off, obj, 3ds\n\
# syntax: head=url\n\
# head=/lwo/head.lwo\n\
# head=/off/head.off\n\
# head=/obj/head.obj\n\
# head=/3ds/head.3ds\n\
#head=/lwo/head.lwo\n\
\n\
# my sex\n\
# syntax: sex=male|female, default=male\n\
sex=male\n\
\n\
# my skin color\n\
# syntax: skin=color\n\
skin=wheat\n\
\n\
# my bust color\n\
# syntax: bust=color\n\
bust=grey\n\
\n\
# my body dimensions in meters\n\
# height=value\n\
# width=value\n\
# depth=value\n\
height=1.75\n\
width=0.25\n\
depth=0.10\n\
\n\
# my front side\n\
# syntax: mapface=urlimage\n\
# example: mapface=http://www.some.where/~login/front.gif\n\
\n\
# my back side\n\
# syntax: mapback=urlimage\n\
# example: mapback=http://www.some.where/~login/back.jpg\n\
\n\
# my world description\n\
# syntax: world=url\n\
# example: world=http://www.some.where/~login/myworld.vre\n\
\n\
# my web site\n\
# syntax: web=url\n\
# example: web=http://www.some.where/~login/\n\
\n\
# HTTP Proxy\n\
# syntax: http_proxy=http://<proxy_hostname>:<port>/\n\
# syntax: no_proxy=<domaine>\n\
# example: http_proxy=http://proxy.some.where:3128/\n\
# example: no_proxy=some.where\n\
\n\
\n\
";
