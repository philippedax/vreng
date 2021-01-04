const char ChangeLog[] = "\
Last changes in VREng\n\
---------------------\n\
\n\
------------------------------------------\n\
The rules for this file:\n\
* entries are sorted newest-first.\n\
*\n\
* ++ : major\n\
* == : medium\n\
* -- : minor\n\
* Version: major.medium.minor\n\
* date and author format: (YYYY/MM/DD, Author).\n\
------------------------------------------\n\
\n\
++ 9\n\
== 9.1\n\
-- 9.1.5\n\
	- added setrlimit to increase no of open files\n\
	- integrated src/ex/vrl to src/util/vrl\n\
	- fixed addobj in widgets.cpp (compound solids)\n\
	- fixed parseSolids to parse multi-solids\n\
	- reviewed util/vred\n\
	- reviewed compilation warnings bundle/glui\n\
	(2021/01/02, Philippe Dax)\n\
\n\
-- 9.1.4\n\
	- reviewed timer.*pp\n\
	- fixed needles motion in clock.cpp\n\
	- reviewed vrel and vred\n\
	- fixed numline in parse.cpp\n\
	(2020/12/14, Philippe Dax)\n\
\n\
-- 9.1.3\n\
	- renamed vre.hpp to sandbox.hpp\n\
	- using stl lists for netobj.*pp\n\
	- rewriting some algos for rendering\n\
	(2020/12/02, Philippe Dax)\n\
\n\
-- 9.1.2\n\
	- fixed -w option in pref.cpp\n\
	- added objectList in wobject.*pp\n\
	- added FLUID mode and fluidLists\n\
	- renamed initialize*Object to init*Oject methods\n\
	- renamed class PutInfo to class Message2\n\
	- renamed class Pitch to class Joystick2\n\
	- removed gui/pitch*pp\n\
	(2020/11/21, Philippe Dax)\n\
\n\
-- 9.1.1\n\
	- fixed urlpfx in universe.cpp\n\
	- reviewed some rendering order\n\
	- removed code creation from vrelines for rain and snow\n\
	- fixed guy distance to ground\n\
	- added parseBool() methods in parse.*pp\n\
	(2020/11/12, Philippe Dax)\n\
\n\
-- 9.1.0\n\
	- invalidated app ring in vjs/\n\
	- restored src/servers/vjs/gen.pl*\n\
	- added door open/close mecanisms : sliding and vertical\n\
	- added fractal element to dtd\n\
	- added command line options -k (keepcache) and -S (nostats)\n\
	(2020/11/03, Philippe Dax)\n\
\n\
== 9.0\n\
-- 9.0.3\n\
	- move gui/mvt.*pp to gui/motion.*pp\n\
	- removed vgl/hud.*pp\n\
	- restored missing file in src/ai for github\n\
	(2020/10/19, Philippe Dax)\n\
\n\
-- 9.0.2\n\
	- fixed angle when toland action\n\
	- restablished gui/hud*pp\n\
	(2020/10/17, Philippe Dax)\n\
\n\
-- 9.0.1\n\
	- added user actions: flyaway and toland\n\
        - fixed some memory leaks\n\
	- renamed WO_ACTION to O_ACTION\n\
	- move universe.*pp into src/vreng\n\
	- move world.*pp into src/vreng\n\
	- move xml.*pp into src/vreng\n\
	- move openal.*pp into src/vreng\n\
	- move useraction.hpp into user.hpp\n\
	- move vncdialog.cpp into widgets.cpp\n\
	- move vncdialog.hpp into widgets.hpp\n\
	- move hud.cpp into widgets.cpp\n\
	- move hud.hpp into widgets.hpp\n\
	(2020/10/15, Philippe Dax)\n\
\n\
-- 9.0.0\n\
        - fix connection to localhost in pref.cpp http.cpp universe.cpp\n\
        - avoid to resolve localhost in http.cpp\n\
        - --with-mysql=no by default\n\
	- move camera.cpp into render.cpp\n\
        - move gui/scene.*pp to scr/vreng\n\
	- move vr2gl.cpp to wo/\n\
	- move addobj.cpp into widgets.cpp\n\
	- move keys.cpp into widgets.cpp\n\
	- move filemenu.cpp into widgets.cpp\n\
	- move dialog.cpp into widgets.cpp\n\
	- move display.cpp into solid.cpp\n\
        - changed ray color from red to white stippled\n\
	(2020/10/03, Philippe Dax)\n\
\n\
++ 8\n\
== 8.7\n\
-- 8.7.3\n\
	- configure change openal by freealut\n\
	- changed fire parameters in fire.hpp\n\
	(2018/10/22, Philippe Dax)\n\
\n\
-- 8.7.2\n\
	- added fonts directory to git\n\
	- added texture/glpng{ch}pp to git\n\
	- moved githup to .github\n\
	(2018/10/21, Philippe Dax)\n\
\n\
-- 8.7.1\n\
	- alternative for missing fonts, use local fonts in uconf.cpp\n\
	- alternative if Xmu not found in udispX11.cpp\n\
	(2018/10/18, Philippe Dax)\n\
\n\
-- 8.7.0\n\
	- find Xmu and Xpm headers needed by src/ubit\n\
	- added missing ttf fonts in directory fonts/\n\
	(2018/10/18, Philippe Dax)\n\
\n\
== 8.6\n\
-- 8.6.0\n\
	- find dynamycaly presence of ttf freetype fonts when ./configure is running\n\
	- move bundle/glpng to texture/\n\
	- move bundle/png to bundle/mypng and phg.h to mypng.h\n\
	(2018/10/08, Philippe Dax)\n\
\n\
== 8.5\n\
-- 8.5.7\n\
	- fixed some compilation warnings\n\
	- fixed X11 fonts path in ubit/uconf.cpp to find freetype fonts\n\
	(2018/09/17, Philippe Dax)\n\
\n\
-- 8.5.6\n\
	- fixed some warnings with g++-8.1.1\n\
	(2018/02/10, Philippe Dax)\n\
\n\
-- 8.5.5\n\
	- fixed CXFLAG without -fpermissive\n\
	- fixed conficts with glpng\n\
	(2018/02/10, Philippe Dax)\n\
	- fixed buffer overflows\n\
	- fixed dynamic exception specification (C++11) by adding -Wno-deprecated to CXXFLAGS\n\
	(2017/12/10, Philippe Dax)\n\
\n\
-- 8.5.4\n\
	- added script start-daemons\n\
	- fixed fratal.*pp\n\
	(2017/06/23, Philippe Dax)\n\
\n\
-- 8.5.3\n\
	- added object fractal #83\n\
	(2017/05/04, Philippe Dax)\n\
\n\
-- 8.5.2\n\
	- fixed vac.cpp to work without a vacs server\n\
	- added timeout to connect to different servers\n\
	(2017/05/03, Philippe Dax)\n\
\n\
-- 8.5.1\n\
	- fixed &png_ptr in bundle/png.c\n\
	- fixed #include \"png.h\" in texture/png.cpp\n\
	- changed --with-png-builtin yes by default\n\
	(2017/04/28, Philippe Dax)\n\
\n\
-- 8.5.0\n\
	- added etc directory at top\n\
	(2017/04/11, Philippe Dax)\n\
\n\
== 8.4\n\
-- 8.4.0\n\
	- removed -Wno-unknown-warning\n\
	(2016/09/28, Philippe Dax)\n\
\n\
== 8.3\n\
-- 8.3.3\n\
	- added install vjs\n\
	- added install vrum\n\
	(2016/09/25, Philippe Dax)\n\
\n\
-- 8.3.2\n\
	- added -Wno-unknown-warnnig option\n\
	(2016/09/22, Philippe Dax)\n\
\n\
-- 8.3.1\n\
	- vreng project hosted at github.com\n\
	- changed CXXFLAGS to avoid warnings under gcc-6.1.1\n\
	- added view.xpm into gui/xpm/Makefile.am\n\
	- moved acinclude.m4 into support/\n\
	(2016/08/07, Philippe Dax)\n\
\n\
-- 8.3.0\n\
	- added checklib libsndfile into configure.ac\n\
	- added openal support --with-openal into configure.ac\n\
	(2016/03/26, Philippe Dax)\n\
\n\
== 8.2\n\
-- 8.2.2\n\
	- renamed missile to projectile\n\
	- added view third person button in panels.cpp\n\
	- added go button in xidgets.cpp dialog.cpp\n\
	- cleans up bone files\n\
	- fixed endianess in hairs.cpp\n\
	- fixed regression on flashy objects\n\
	- rewriting Render::solids\n\
	- fixed sun rendering\n\
	- added bindSocket in socket.cpp\n\
	- added trace2 without cr in log.cpp\n\
	- added stringcmp in str.cpp\n\
	(2016/01/20, Philippe Dax)\n\
\n\
-- 8.2.1\n\
	- fixed regression of ubit/uconf.cpp\n\
	(2015/12/29, Philippe Dax)\n\
\n\
-- 8.2.0\n\
	- updated ubit to be in conformity with the clang apple compiler\n\
	- updated check libmysqlclient for Linux\n\
	(2015/10/26, Philippe Dax)\n\
\n\
== 8.1\n\
-- 8.1.0\n\
	- added tar.xz distribution\n\
	- added vmm branch\n\
	(2013/10/15, Philippe Dax)\n\
\n\
== 8.0\n\
-- 8.0.0\n\
	- added bsphere bounding sphere\n\
	- fixed draw bbox\n\
	- fixed view source using cache\n\
	- added object firework\n\
	- added src/vreng/vector3.hpp\n\
	- deleted src/wo/grid.cpp\n\
	- deleted src/wo/netw.cpp\n\
	- moved src/wo/list.{cpp,hpp} to olist.{cpp,hpp}\n\
	- updated vreng data version to v8\n\
	(2012/09/03, Philippe Dax)\n\
\n\
=======\n\
++ 7\n\
== 7.4\n\
-- 7.4.0\n\
	- added src/sys\n\
	- rewriting src/app/* with classes\n\
	- updated display.cpp displayMirrored without pushing names in the buffer\n\
	- fixed xpm.cpp\n\
	- updated src/ubit/uconf.cpp for Type1 fonts under Linux\n\
	- moved src/ubit/nat/*pp to src/ubit\n\
	- moved src/vrel src/vred src/tool to src/util\n\
	- moved src/bone/*pp to src/model\n\
	- updated conf/vreng.spec to build rpm package\n\
	- fixed bin/Makefile.am for vred and vrel\n\
	(2012/07/08, Philippe Dax)\n\
\n\
== 7.3\n\
-- 7.3.3\n\
	- moved src/net/md5* src/net/random.cpp to src/vreng/\n\
	- resolved opens/closes leak\n\
	- updated textureList to STL\n\
	- moved src/net/nsl* src/net/socket* to src/vreng/\n\
	- updated file.{ch}pp : added openFile() and closeFile()\n\
	- updated fileio.{ch}pp to file.{ch}pp\n\
	- updated socket.cpp : closeDatagram(), closeStream()\n\
	- updated prof.cpp : added statIO()\n\
	(2012/06/06, Philippe Dax)\n\
\n\
-- 7.3.2\n\
	- updated mobileList, stillList, txfList to STL\n\
	- updated txf to be cached\n\
	- added net/noid.{ch}pp comeback\n\
	- updated netobjectList, channelList to STL\n\
	- updated cartList, wearList, lightList, deleteList, invisibleList to STL\n\
	- updated oclass from wclass\n\
	- updated lwo to class\n\
	- updated smoke\n\
	(2012/06/03, Philippe Dax)\n\
\n\
-- 7.3.1\n\
	- all remote data are now cached (off, lwo, dxf, cset, book, bodyparts, walls)\n\
	- added vreng/cache.{ch}pp\n\
	- added vreng/reader.{ch}pp\n\
	- added option -r,--refresh\n\
	- added tools obj2parts, jpobj.c, jpoff.c\n\
	- added model/bap.t\n\
	- updated android, body, bap\n\
	- added src/tool\n\
	- added sound.{ch}pp\n\
	(2012/04/25, Philippe Dax)\n\
\n\
-- 7.3.0\n\
	- updated vreng-data to version 7.3\n\
	- updated paralellipedic sizes\n\
	(2012/02/24, Philippe Dax)\n\
\n\
== 7.2\n\
-- 7.2.8\n\
	- added object smoke\n\
	- moved anim.{ch}pp to animator.{ch}pp\n\
	- added vred branch\n\
	- added vrel branch\n\
	(2012/02/21, Philippe Dax)\n\
\n\
-- 7.2.7\n\
	- moved Attractor to Slider\n\
	- moved Repulsor to Bumper\n\
	- moved Follower to Accessory\n\
	(2012/01/29, Philippe Dax)\n\
\n\
-- 7.2.6\n\
	- updated object ball from file\n\
	- added object follower\n\
	- added draw bbox\n\
	- added object repulsor\n\
	- added object attractor\n\
	- added dtd menu\n\
	- updated rendering order\n\
	- added model/bvh.{ch}pp\n\
	(2012/01/23, Philippe Dax)\n\
\n\
-- 7.2.5\n\
	- added mode roll and tilt to mirage\n\
	- added bubble solid translucid obloid\n\
	- updated plane multi bounding boxes\n\
	- added legend attribute to web\n\
	- added pivot action to web\n\
	- added AVI video reader/writer\n\
	- added PSD image loader\n\
	(2011/12/24, Philippe Dax)\n\
\n\
-- 7.2.4\n\
	- moved mirror algos to display.cpp\n\
	- added model helicopter to wings\n\
	- updated user and head, head follows avatar over the worlds\n\
	- renamed bone/ vnc/\n\
	- moved wo/{body,face} to model/\n\
	- updated url : saveToCacheAndOpen\n\
	- updated hairs\n\
	(2011/11/08, Philippe Dax)\n\
\n\
-- 7.2.3\n\
	- added prefs sex, head, skin, bust\n\
	(2011/09/20, Philippe Dax)\n\
\n\
-- 7.2.2\n\
	- added option -t, --trace\n\
	- added global localuser\n\
	(2011/09/14, Philippe Dax)\n\
\n\
-- 7.2.1\n\
	- added object fire\n\
	- added models butterfly, libellule, angel to wings\n\
	- added models spires, bands, roses to dress\n\
	- added models tophat, bowler, chinese, cardinal, country, clown to hat\n\
	- added female to guy\n\
	- added mysql to cloth\n\
	- added colors to guy\n\
	(2011/08/14, Philippe Dax)\n\
\n\
-- 7.2.0\n\
	- new layout\n\
	- added bin and man directories\n\
	- updated 3rdpart to extras\n\
	- moved com to vreng\n\
	(2011/07/30, Philippe Dax)\n\
\n\
== 7.1\n\
-- 7.1.0\n\
	- reduced dependencies (ubit, png, zlib)\n\
	- added builtin ubit\n\
	- added builtin png\n\
	- added builtin zlib\n\
	(2011/07/28, Philippe Dax)\n\
\n\
== 7.0\n\
-- 7.0.2\n\
	- added src/ubit\n\
	- added src/3rdpart/ftgl\n\
	(2011/07/19, Philippe Dax)\n\
\n\
-- 7.0.1\n\
	- fixed some g++ warnings\n\
	- moved config/ to support/\n\
	(2011/07/18, Philippe Dax, comeback after 2 years !)\n\
\n\
-- 7.0.0\n\
	- added flying bevavior for user\n\
	- added command line option -a,--avatar to select an avatar model (man, guy)\n\
	- added new user when enter into a new world\n\
	- renamed spark to thunder\n\
	- added wings object\n\
	- updated user : can manage avatar model preferences\n\
	- updated guy, now can be the avatar model by default\n\
	- added flare-shine rendering\n\
	- added teapot shape\n\
	- added dress object\n\
	- added hat object\n\
	- added halo object\n\
	- added cloth object\n\
	- added sun object\n\
	- added river object\n\
	- added terrain object\n\
	- added bubble object\n\
	- added snow object\n\
	- added rain object\n\
	- added fountain object\n\
	- added waterfall object\n\
	- added spiral object\n\
	- added travelator object\n\
	- added escalator object\n\
	- added stair object\n\
	- added cloud object\n\
	- fixed stars area\n\
	- added widget object\n\
	- added spark object\n\
	- added pthread for wheel progression in universe.cpp\n\
	- added wheel and needle rendering in solid.cpp\n\
	- added wheel and needle drawings in draw.cpp\n\
	- added savePNG in png.cpp\n\
	- added saveTGA in tga.cpp\n\
	- added curl support in url.cpp\n\
	- fixed read of vacs cache\n\
	- updated configure.ac to check linux /usr/lib64 and solaris /usr/sfw\n\
	- added --enable-debug into configure.ac\n\
	- added wind object\n\
	- updated configure.ac for macports\n\
	- renamed vnp.* to off.* (Object File Format)\n\
	- added sub-directories src/texture src/model\n\
	- renamed *.hh to *.hpp\n\
	- fixed circular motion in mirage.cpp\n\
	- added Xbm image loader\n\
	- added Xpm image loader\n\
	- added dds image loader\n\
	- added tiff image loader\n\
	- added class for format\n\
	- added timer.{cpp,hh} prof.{cpp,hh} socket.hh event.hh sap.hh\n\
	- moved stat.{cpp,hh} http.{cpp,hh} url.{cpp,hh} md5.{cpp,hh}\n\
	- renamed file.{cpp.hh} -> fileio.{cpp.hh} net.hh -> netobj.cpp\n\
	- updated video capture to produce directly a flash file (.swf)\n\
	- fixed lightings in hud.cpp\n\
	- added methods (set|reset)Flashy into animx3d.cpp\n\
	- fixed grid.cpp expecting the end of GUI initialization\n\
	- updated hud and xwin to use GLUT fonts (glutBitmapCharacter())\n\
	- added --with-glut into configure.ac\n\
	- added --with-sdl into configure.ac\n\
	(2009/05/10, Philippe Dax)\n\
	- updated gui/* reorganization and rewritting without X11 dependencies\n\
	(2008/07/19 - Eric Lecolinet)\n\
\n\
++ 6\n\
== 6.9\n\
-- 6.9.3\n\
	- updated GUI to be compliant with ubit6\n\
	- fixed movie.cpp include mpeg.h\n\
	- fixed link edit against OpenGL under MacOSX-10.5 (Leopard) \n\
	(2007/12/29, Philippe Dax)\n\
\n\
-- 6.9.2\n\
	- fixed mini-map rotation\n\
	(2007/10/16, Eric Varadaradjou)\n\
\n\
	- added mpeg_lib in 3rdparty if not available on the target system\n\
	- fixed pitch for other views\n\
	- fixed view of bubble above the head\n\
	- fixed click in the background\n\
	(2007/10/02, Philippe Dax)\n\
\n\
-- 6.9.1\n\
	- fixed bad references to 3party in som Makefile.am\n\
	- fixed compilation warning under g++-4.2.0\n\
	- fixed bad usage of GL_RGB in movie.cpp\n\
	- fixed small bug in Makefile.am\n\
	(2007/09/18, Philippe Dax)\n\
\n\
-- 6.9.0\n\
	- renamed x3dobject to animx3d\n\
	- renamed 3party to 3rdpart\n\
	- fixed DXF parsing\n\
	(2007/07/11, Philippe Dax)\n\
\n\
	- added x3d loader-parser, x3d aanimation rendering\n\
	- added x3dobject to play animations described by x3d format\n\
	(2007/06/28, Pascal Chambon)\n\
\n\
	- added support for dxf format (autocad)\n\
	- fixed static memory of name=\"basket\" to dynamic memory under Linux\n\
	- fixed specific parsing to avoid infinite loop\n\
	- fixed cleanCacheByExt to avoid deleting *.vre3d\n\
	- fixed glpng/Makefile.am using conditional test MYLIBPNG\n\
	- added textures for body for obj format\n\
	- revisted body and android (joint-points file xmlized)\n\
	- revisted body and bap\n\
	- revisted models Md2, Obj, Man\n\
	(2007/06/18, Philippe Dax)\n\
\n\
== 6.8\n\
-- 6.8.12\n\
	- fixed getProperty, don't call isValid()\n\
	- fixed glPushName((GLuint) (long)addr & 0xffffffff) (64 bits arch)\n\
	- fixed mrelease, don't call openedMenu->close(0): segfault 64 bits arch\n\
	- fixed createSendSocket test == false >= right to call setSendSocket\n\
	(2007/03/23, Philippe Dax)\n\
\n\
-- 6.8.11\n\
	- fixed next sdes item in rtp.cpp\n\
	- fixed Source constructor next=NULL in source.cpp\n\
	- fixed no loopback in socket.cpp\n\
	- fided compilation warning in navig.cpp\n\
	(2007/03/22, Philippe Dax)\n\
\n\
-- 6.8.10\n\
	- fixed CXXFLAGS in configure.ac\n\
	- fixed segfault in dart.cpp and bullet.cpp\n\
	- added file.{cpp,hh}\n\
	- added md2 into model.cpp\n\
	- revisted md2.{cpp,hh}\n\
	(2007/03/16, Philippe Dax)\n\
\n\
-- 6.8.9\n\
	- fixed com/Makefile.am env.cc -> pref.cpp\n\
	- fixed some extra qualification detected by g++-4.1.2 abort compilation\n\
	(2007/02/23, Philippe Dax)\n\
\n\
-- 6.8.8\n\
	- fixed AC_CONFIG_SRCDIR vreng.cc -> vreng.cpp\n\
	(2007/02/18, Philippe Dax)\n\
\n\
-- 6.8.7\n\
	- fixed obj.cpp avoid negative vertice index\n\
	- added gui.hpp from guiImpl.hpp and gui.hh which are removed\n\
	- renamed guiclass.* to xclass.*\n\
	- removed guimisc.cc now in vreng.cc\n\
	- added putinfo.{cpp,hpp} from gui.cpp\n\
	- added mvt.{cpp,hpp} from navig.cpp\n\
	- added xerr.{cpp,hpp} from xwin.cpp\n\
	- added double-click handling (default action id=0)\n\
	- fixed parsing bugs introduced in 6.8.4\n\
	- renamed all file .cc to .cpp\n\
	- renamed counter.* to hud.*\n\
	(2007/02/18, Philippe Dax)\n\
\n\
-- 6.8.6\n\
	- splited landmark.{cc,hh} into 6 files: (axis,grid,counter}.{cc,hh}\n\
	- renamed options.{cc,hh} to pref.{cc,hh}\n\
	- rewrited env.{cc,hh}, class Env added\n\
	- updated http.cc with getShort, getLong and getGLfloat\n\
	- updated configure.ac to be compliant with pkgsrc\n\
	- updated joint points data (man.jp) incompatible with previous releases\n\
	- updated VNP structure to VNP class\n\
	(2007/02/08, Philippe Dax)\n\
\n\
-- 6.8.5\n\
	- added bap.{cc,hh} and fap.hh\n\
	- updated man.cc (vertices, normals, texcoords)\n\
	- updated android to load VNP models with scale=1\n\
	- fixed include #include <map> in vicinity.cc with ubit-5.5.0\n\
	- added class Body in body.{cc,hh}\n\
	- fixed wrong polygons drawing for VNP models in vnp.cc\n\
	- added body.{cc,hh} to handle independantly human body joint points\n\
	(2007/01/22, Philippe Dax)\n\
\n\
-- 6.8.4\n\
	- revised DTD, NMTOKEN(S) added\n\
	- fixed a bug in color.cc when the colorname does not exist\n\
	- added mode=\"travelator\" in step.cc\n\
	- added mode=\"escalator\" up and down in step.cc\n\
	- added mode=\"stairs\" to build directly stairscase in step.cc\n\
	- added for for the whole scene in light.cc\n\
	- added fog for each object\n\
	- added Parse class to parse objects and solids\n\
	- added handling of http redirects like '301 - Move permanently'\n\
	- added soft rotation when the direction of the guide changes\n\
	- added mode=\"one-way\" attribute for the guide's path (no loop)\n\
	- added color attribute for the guide's path\n\
	- added temporisation delay at each vertice of the guide's path\n\
	- added actions to the guide : show/hide, pause/continue, reset\n\
	- added drawing with stipple lines the path of the guide\n\
	- fixed user takes the same orientation as the guide along the path\n\
	- fixed orientation of the guide along the path\n\
	(2007/01/08, Philippe Dax)\n\
\n\
-- 6.8.3\n\
	- added DTD into conf/vre-1.0.dtd\n\
	- updated configure.ac and Makefile.am for distribs tar.bz2 and zip\n\
	- fixed sgi loader (RBGA to RGB)\n\
	- fixed infinite loop in col.cc when intersection occurs with guide\n\
	- fixed drawing in vrelet.cc\n\
	- fixed lightings in light.cc, text.cc and render.cc\n\
	- fixed position and orientation of flag\n\
	- updated vreng icon for Window-manager\n\
	(2006/11/27, Philippe Dax)\n\
\n\
-- 6.8.2\n\
	- updated theme.{cc,hh} to be compliant with new ubit-5.4.1 API\n\
	- updated 3ds and ase models drawing in display lists\n\
	- updated gl2ps from 1.2.7 to 1.3.2\n\
	(2006/11/14, Philippe Dax)\n\
\n\
-- 6.8.1\n\
	- fixed segfaults in generalIntersect()\n\
	- added collision detection of oblique objects\n\
	- added alternative skins (background colors) to GUI handled by theme.cc\n\
	- added bubble text (welcome and chat) above the avatar's head\n\
	- added Open and Save buttons into File menu\n\
	- added reverse video on buttons\n\
	- added OBJ loader for Wavefront models\n\
	- added target svntags into Makefile.am for maintainer\n\
	(2006/11/09, Philippe Dax)\n\
\n\
-- 6.8.0\n\
	- updated Cart to update geometry when the object is dropped\n\
	- added compound solids (chair, table) into palette.cc\n\
	- added sandbox button and callback\n\
	- added builtin sandbox world\n\
	- moved Manip to Move, Goto to Approach\n\
	- added more MySql methods to handle columns\n\
	- updated structures of MySql tables\n\
	- added solid strings now handled by MySql, getGeom, updateGeom\n\
	- fixed manipulations of things in cart.cc\n\
	- added dymamic thing objects are now persistent\n\
	(2006/09/04, Philippe Dax)\n\
\n\
== 6.7\n\
-- 6.7.4\n\
	- added audio loaders : wav.cc, au.cc\n\
	- added audio player : vlc, play\n\
	- added sound action and update music.cc consequently\n\
	- removed obsolete applications : slidecast and vrbrowser\n\
	- updated screen of vnc\n\
	- added push object when user collide it\n\
	- fixed free memory in movie.cc\n\
	(2006/07/21, Philippe Dax)\n\
\n\
-- 6.7.3\n\
	- fixed model's rotations\n\
	- added persistency for models\n\
	- removed friend related to carrier class\n\
	- added owner for dynamic objects, method removeFromScene()\n\
	- added basic textures and alpha values into the palette\n\
	(2006/07/05, Philippe Dax)\n\
	- added palette to dynamicaly insert objects into the scene (palette.cc)\n\
	(2006/06/28, Basile Anglaret)\n\
\n\
-- 6.7.2\n\
	- fixed counter and avatar rendering under Ubit-5.0.2\n\
	(2006/06/21, Eric Lecolinet)\n\
	- added initializMobileObject(lasting)\n\
	(2006/06/21, Philippe Dax)\n\
	- fixed warnings given by autoscan and aclocal\n\
	- added showSolidList()\n\
	(2006/06/21, Eric Varadaradjou)\n\
\n\
-- 6.7.1\n\
	- updated Ubit interface\n\
	- fixed resizing GLzone\n\
	(2006/06/20, Eric Lecolinet)\n\
	- updated model with pos=\"...\"\n\
	- added makeSolid()\n\
	(2006/06/19, Philippe Dax)\n\
\n\
-- 6.7.0\n\
	- fixed an huge bug about setlocale LC_NUMERIC, now take LC_ALL=\"C\"\n\
	(2006/06/14, Philippe Dax)\n\
\n\
== 6.6\n\
-- 6.6.3\n\
	- check ubit version and compare it with required ubit version\n\
	- fixed minor bugs in gui/\n\
	(2006/06/13, Philippe Dax)\n\
	- added history of requests and navigation in history\n\
	(2006/06/12, Eric Varadaradjou)\n\
\n\
-- 6.6.2\n\
	- fixed enableBehavior isVisible(false)\n\
	- fixed rezising: update glViewport in render.cc\n\
	- removed all NAVIG actions\n\
	(2006/06/08, Philippe Dax)\n\
\n\
-- 6.6.1\n\
	- updated 16 to 24 color bits\n\
	- updated button to select actions by name and no more by index\n\
	- added macro AC_CHECK_FREETYPE in acinclude.m4\n\
	- added check of freetype library in configure.ac\n\
	(2006/06/06, Philippe Dax)\n\
\n\
-- 6.6.0\n\
	- reorganization of initobj.{cc,hh}nd general tables of functions\n\
	- fixed the disparition of object bar\n\
	- fixed bug in deleteObject when an object is destroyed in the scene\n\
	(2006/06/05, Philippe Dax)\n\
\n\
== 6.5\n\
-- 6.5.13\n\
	- added code in prevision of UBIT5\n\
	- added gui/baz.{cc,hh}\n\
	- fixed addlist in the GUI\n\
	- fixed button action name\n\
	(2006/06/01, Philippe Dax)\n\
	- added gui/vicinity.{cc,hh}\n\
	- added map view with its button\n\
	(2006/05/24, Eric Varadaradjou)\n\
\n\
-- 6.5.12\n\
	- fixed adding into avatar and world Gui lists\n\
	(2006/05/09, Eric Varadaradjou)\n\
	- added screenshot output in SVG format\n\
	- updated gl2ps with SVG capability\n\
	- removed --with-ubit from configure.ac\n\
	- added xpm.hh in src/gui/xpm\n\
	(2006/05/09, Philippe Dax)\n\
\n\
-- 6.5.11\n\
	- fixed ax angle in book.cc when approaching\n\
	- fixed scrolling in worldpane (Eric Lecolinet)\n\
	- added callback to enter into worlds from an url string\n\
	- added toggle buttons into the panel for axis/grid/overlap/counter\n\
	- fixed bugs: configure.ac, gui/Makefile.am, compilation without ocaml\n\
	(2006/05/04, Philippe Dax)\n\
\n\
-- 6.5.10\n\
	- split ubwidgets.cc into many smaller files\n\
	- add car.{cc,hh} taken from draw.cc\n\
	- human.hh becomes man.{cc.hh}\n\
	- add ground level view, update scissor view\n\
	(2006/04/13, Philippe Dax)\n\
	- fix vertical scrolling\n\
	- update Gui to be compliant with the current Ubit\n\
	(2006/03/27, Eric Lecolinet)\n\
	- fix bad WIDTH3D\n\
	(2006/03/19, Philippe Dax)\n\
	- code for what objects are viewed\n\
	(2006/02/27, Eric Varadaradjou)\n\
\n\
-- 6.5.9\n\
	- laser selects objects rather than solids\n\
	- solid class rewritten\n\
	(2006/01/13, Damien Bobillot)\n\
\n\
-- 6.5.8\n\
	- Ubit interface updated\n\
	(2006/01/05, Eric Lecolinet)\n\
	- added mode=\"circular\" in mirage.cc\n\
	(2006/01/03, Eric Varadaradjou)\n\
\n\
-- 6.5.7\n\
	- bug fixed worldname string not null terminated (introduced in 6.5.5)\n\
	- added perpetual movement for guide object: mode=\"perpetual\"\n\
	(2005/12/29, Philippe Dax)\n\
	- don't update MysSql if object has not moved\n\
	- world.cc ubit independant: move sourceDialog to ubwidgets.cc\n\
	(2005/12/18, Philippe Dax)\n\
	- Render class rewritten\n\
	(2005/12/08, Damien Bobillot)\n\
\n\
-- 6.5.6\n\
	- changes in gui/ to be compliant with new ubit API\n\
	(2005/11/30, Philippe Dax)\n\
	- allow buffer 3D selection according to a depth level\n\
	(2005/11/29, Eric Varadaradjou)\n\
\n\
-- 6.5.5\n\
	- bug fixed in bone/boneAnimator.cc\n\
	- bug fixed about analyseur.ml in ocaml/Makefile.am\n\
	- rtpsess.cc splited into rtp.cc, session.cc, source.cc\n\
	- added ellipse shape\n\
	(2005/11/21, Philippe Dax)\n\
	- rewriting World class\n\
	- merge debug.hh > log.hh\n\
	(2005/11/16, Damien Bobillot)\n\
	- changed request input field\n\
	(2005/11/07, Eric Varadaradjou)\n\
	- added plane object\n\
	- added ground object\n\
	- added missile object\n\
	- added color.cc, handle color names\n\
	(2005/11/01, Philippe Dax)\n\
\n\
-- 6.5.4\n\
	- laser published throught the network\n\
	(2005/10/28, Philippe Dax)\n\
	- ocaml designation mecanism\n\
	(2005/10/27, Eric Varadaradjou)\n\
\n\
-- 6.5.3\n\
	- fix a compilation bug with ocaml (if HAVE_OCAML)\n\
	- fix user names exchanged between participants\n\
	- fix getRtcp* returning string\n\
	- update cart.cc, icon.cc\n\
	(2005/10/24, Philippe Dax)\n\
	- ocaml updates\n\
	(2005/10/19, Eric Varadaradjou)\n\
\n\
-- 6.5.2\n\
	- updates in ocaml/\n\
	(2005/10/13, Eric Varadaradjou)\n\
	- fix stupid compilation bug in col.cc\n\
	(2005/10/12, Philippe Dax)\n\
\n\
-- 6.5.1\n\
	- compliance with gcc-4.0\n\
	(2005/10/11, Damien Bobillot)\n\
	- fix compilation error read_request not defined, use HAVE_OCAML\n\
	(2005/10/08, Philippe Dax)\n\
\n\
-- 6.5.0\n\
	- collisions for multi-solids\n\
	- Navigator prefered to XNavigator \n\
	- goto action : 2 compound movements\n\
	- add renderList for specific rendering\n\
	- code of mirror from render.cc now in mirror.cc\n\
	- cleanup 6.4.10\n\
	(2005/10/02, Philippe Dax)\n\
\n\
== 6.4\n\
-- 6.4.10\n\
	- goto deplacement is multithreaded and moves without colliding\n\
	- request input on the same panel as chat input\n\
	- gui background color is black, foreground is lightblue\n\
	(2005/07/31, Philippe Dax)\n\
	- add ocaml parsers: lexical, syntaxic, semantic,...\n\
	(2005/06/30, Eric Varadaradjou)\n\
	- re-add ocaml, adapt vreng/Makefile.am\n\
	(2005/06/30, Philippe Dax)\n\
\n\
-- 6.4.9\n\
	- remove ocaml from distribution\n\
	(2005/06/16, Philippe Dax)\n\
\n\
-- 6.4.8\n\
	- fix mirror\n\
	(2005/06/15, Eric Varadaradjou)\n\
	- co-existence new-look and old-look : option -l,--look old|new\n\
	(2005/06/15, Philippe Dax)\n\
	- gui new-look\n\
	- navigation new-look\n\
	- update theme\n\
	- new xpm files\n\
	(2005/06/07, Mael Leboite)\n\
\n\
-- 6.4.7\n\
	- add conditional compilation for 3party/gl2ps\n\
	- new object: area #49 (invisible)\n\
	- call QuickTime under MacOSX when playing Mpeg\n\
	(2005/06/02, Philippe Dax)\n\
	- link against libasmrun.a is ok\n\
	- new subdir src/ocaml\n\
	- first steps with ocaml\n\
	(2005/05/18, Philippe Dax)\n\
\n\
-- 6.4.6\n\
	- human avatar representing real user (human.hh, solid.cc, draw.cc)\n\
	- sources managed by subversion-1.2.0-rc1\n\
	- <!DOCTYPE ...> dtd line parsed, new files: xml.{cc,hh}\n\
	(2005/04/25, Eric Varadaradjou)\n\
	- bug fixed if hostname length is bigger than RTPNAME_LEN (rtpsess.cc)\n\
	- AC_SUBST on mysql and xml2 CFLAGS\n\
	- fix RTPNAME_LEN too short if hostname length too large\n\
	(2005/04/24, Philippe Dax)\n\
\n\
-- 6.4.5\n\
	- add check libxml2 in configure.ac\n\
	- improve guide with angular position changing along the path\n\
	(2005/04/05, Philippe Dax)\n\
\n\
-- 6.4.4\n\
	- fix a bug in src/vreng/Makefile.am\n\
	(2005/03/24, Philippe Dax)\n\
\n\
-- 6.4.3\n\
	- add object guide #48 to make automatic visits\n\
	(2005/03/18, Matthieu Hennion, Stephane Curinckx)\n\
\n\
-- 6.4.2\n\
	- split vgl.hh in render.hh draw.hh ogl.hh\n\
	- remame class VGContext by Render\n\
	- anti-aliased lines\n\
	- add wired view\n\
	- add object flag #47\n\
	- add displaylist in walls.cc\n\
	- fix a bug on displaylist in mech.cc\n\
	- add new sound rups.au\n\
	- add flash effect when capturing image\n\
	- add &user=<username> in httpd log\n\
	- move src/vgl/bones/ to src/bone/\n\
	- move src/app/vnc/ to src/vnc/\n\
	- pruning code, minor fixes in render.cc\n\
	- minor fixes in world.* (static -> virtual) methods\n\
	- publish laser to network, property PROPLASER\n\
	- laser replication\n\
	(2005/03/08, Philippe Dax)\n\
\n\
-- 6.4.1\n\
	- move vgl/glpng/inf* to 3party/glpng/zlib/\n\
	- move vgl/glpng/png* to 3party/glpng/png/\n\
	- move vgl/glpng to 3party/glpng\n\
	- png.cc without glpng code\n\
	- rename 3part to 3party\n\
	(2005/02/15, Philippe Dax)\n\
\n\
-- 6.4.0 <-- gnu.org announce\n\
	- fix Copyright terms\n\
	- fix compilation errors under gcc-4.0, thanks to Andreas Jochens\n\
	- capture a sequence of jpeg images to allow to make a clip\n\
	- function VglCopyPixels used by capture added in solid.cc\n\
	- add a button and action to enter into first person view\n\
	- remove dbs and elc objects from distribution\n\
	(2005/02/11, Philippe Dax)\n\
\n\
== 6.3\n\
-- 6.3.10\n\
	- add new sounds in snd.cc (clack, camera)\n\
	(2005/02/02, Philippe Dax)\n\
\n\
-- 6.3.9\n\
	- Laser ray oriented on the click direction\n\
	(2005/01/28, Eric Varadaradjou)\n\
\n\
-- 6.3.8\n\
	- move getSurfVecs from vrelet.cc to 3dw.cc\n\
	- move calcVar from vrelet.cc to matvec.cc\n\
	- fix server vacs to be independent of vreng data versions\n\
	(2005/01/27, Philippe Dax)\n\
\n\
-- 6.3.7\n\
	- fix a bug in mech.cc introduced since V-6.3.3\n\
	- fix a bug in anim.cc introduced since V-6.2.10\n\
	- add a laser ray between user'eye and the center of the selected object\n\
	- ttl fixed at 3 sec when moving user towards an object\n\
	- add moveBehind and change moveNear by moveFront\n\
	(2005/01/21, Philippe Dax)\n\
\n\
-- 6.3.6\n\
	- decrease messagePane width to adapt the 3D width with the main window\n\
	- add moveNear() callback function in move.cc\n\
	- add moveUserToObject() method to move user in front of an object\n\
	(2005/01/17, Philippe Dax)\n\
\n\
-- 6.3.5\n\
	- add capture by glReadPixels() to a JPEG file via saveJPG()\n\
	- add HAVE_JPEG detection in configure.ac\n\
	- add third party-software gl2ps in src/3part/gl2ps/\n\
	(2005/01/14, Philippe Dax)\n\
\n\
-- 6.3.4\n\
	- fix a bug in col.cc (step collision didn't work since V-6.2.3)\n\
	- add 2 far viewpoints\n\
	- add flip button to extend control panel in the GUI\n\
	- add saveJPG()\n\
	(2005/01/12, Philippe Dax)\n\
\n\
-- 6.3.3 <- OpenGL annouce\n\
	- fix a bug when replicating the full description of a remote avatar\n\
	- add rate argument to -f,--frames to indiquate the maximum of fps\n\
	- dissociate notice and warning messages by different colors\n\
	- capture vreng window using xwd and convert (ImageMagick)\n\
	- add capture button in the control pannel\n\
	- add camera.xpm and movie.xpm\n\
	- add WObject::updateMysSqlPosition() & WObject::flushMySqlPosition()\n\
	(2005/01/07, Philippe Dax)\n\
\n\
-- 6.3.2\n\
	- current world fetches the balls in the MySql DB and recreates them\n\
	- add VRSql::getSubstring to get the full string if a pattern matches\n\
	- when a ball is destroyed, delete it from the MySql database\n\
	- make ball object persistent\n\
	- add getCountBall in mysql.cc\n\
	- modify getCountCart in mysql.cc\n\
	- new xpm files for x,y,z positive and negative translations\n\
	- dynamic switching of navigator/manipulator in the control panel\n\
	- move wo/keys.hh in gui/keys.hh\n\
	- add -s,--silent option to disable sound effects\n\
	(2005/01/02, Philippe Dax)\n\
\n\
-- 6.3.1\n\
	- Manipulator calls setLspeed and setAspeed in ubnavig.cc\n\
	- add actions setLspeed, setAspeed into carrier.cc\n\
	(2004/12/29, Philippe Dax)\n\
\n\
-- 6.3.0\n\
	- add row index parameter to MySql get methods\n\
	- add xpm files to represent translations and rotations on the objects\n\
	- add methods to move objects from the mouse in carrier.cc\n\
	- add manipulator control menu in ubnavig.cc\n\
	- add panel to manipulate position and orientation of objects\n\
	(2004/12/14, Philippe Dax)\n\
\n\
== 6.2\n\
-- 6.2.11\n\
        - fix a compilation bug in mirror.cc\n\
	(2004/12/14, Philippe Dax)\n\
\n\
-- 6.2.10\n\
	- add tips on buttons into the GUI\n\
	- add MySql methods: deleteRows, insertCol, updateString, getCountRow\n\
	- add engine methods: updatePosToNetwork, updateLasting, testMoving\n\
	(2004/12/10, Philippe Dax)\n\
\n\
-- 6.2.9\n\
	- fix small bug in mysql.cc and door.cc\n\
	- add persistency to ball, book\n\
	- Objects gate, board, mirage, step, clip, movie, ftp are now persistent\n\
	(2004/12/06, Philippe Dax)\n\
\n\
-- 6.2.8\n\
	- Objects web, doc, lift, music are now persistent\n\
	(2004/12/03, Philippe Dax)\n\
\n\
-- 6.2.7\n\
	- Objects thing, host are now persistent\n\
	- add methods to handle position and orientation persistency in mysql.cc\n\
	(2004/11/30, Philippe Dax)\n\
\n\
-- 6.2.6\n\
	- fix an HUGE bug: string \"Content-Type:\" too long from apache2 server\n\
	- fix a bug when VRSql::quit is invoqued, don't delete vrsql\n\
	- remove DBG_THRD bit from debug.hh\n\
	- change line options: -d,--debug mask; -u,--universe; -p,--pseudo, ...\n\
	- add -n,--number number : max of simultaneous threads to download\n\
	(2004/11/27, Philippe Dax)\n\
\n\
-- 6.2.5\n\
	- add -U,--universe comand line option\n\
	- add universe.{cc,hh}\n\
	- add COPYRIGHT notice to all files\n\
	(2004/11/21, Philippe Dax)\n\
\n\
-- 6.2.4\n\
	- use urlAbs() in world.cc to find world by url\n\
	- fix urlAbs() to take in account the DEF_URL_PFX\n\
	- use macros FREE, DELETE and DELETE2 (delete[]) to free memory\n\
	- add --enable-efence to link against libefence (for memory debugging)\n\
	- fix door.cc by calling the VRMsql destructor to do a mysql_close()\n\
	- add -g,--no-gravity command line option\n\
	- add descr=\"... description string ...\" attribute\n\
	(2004/11/18, Philippe Dax)\n\
\n\
-- 6.2.3\n\
	- object wall now mobile and takable like object thing\n\
	- limit cpu usage in the case where the rate fps is too high\n\
	- rename all headers .h to .hh\n\
	- add subdir gui/xpm where are *.xpm images\n\
	- executable name is \"vreng\" instead of \"vreng-ogl\"\n\
	- executable script wrapper name is \"vre\" instead of \"vreng\"\n\
	- remove CHECK-LIB on -lSM -lICE -lXext from configure.ac\n\
	- remove support for obsoleted TinyGL\n\
	- remove support for obsoleted motif and X11-athena\n\
	- add option --with-httpd-prefix=[DIR] in configure.ac\n\
	- fix a loop in col.cc\n\
	(2004/11/12, Philippe Dax)\n\
\n\
-- 6.2.2\n\
	- add -d, --display host for X11 DISPLAY\n\
	- add long options -- style\n\
	- new files options.cc & options.h\n\
	- change macro GEN_URL and DEF_URL_PFX in defaults.h\n\
	- add missing file question.xpm into the distribution\n\
	(2004/11/05, Philippe Dax)\n\
\n\
-- 6.2.1\n\
	_ fix a bug in door.cc when MySql is not available\n\
	- split env.cc into env.cc and inits.cc\n\
	- fix some bugs in vac.cc\n\
	- use #include to include xpm image files into theme.cc \n\
	- new buttons in neon style for the menubar\n\
	- move suffixes of text files built by txt2str from .h to .t\n\
	(2004/10/23, Philippe Dax)\n\
\n\
-- 6.2.0\n\
	- requires ubit-4.2.0 -> ftp://ftp.enst.fr/pub/ubit/ubit-4.2.0.tar.gz\n\
	- upgrade gui to be compliant with ubit-4.2.0\n\
	(2004/09/24, Eric Lecolinet)\n\
	- fixed bug in carrier.cc when we leave an object already leaved\n\
	- add roll and tilt camera movement into the control panel\n\
	- increase window size from 480x270 to 600x360 (ratio 16/9)\n\
	- fix boneVect3D.h: compile error under gcc-3.4\n\
	- update mech.cc to use drawing routines from draw.cc\n\
	- modify line shape (length, thick)\n\
	- add pyramid shape\n\
	(2004/09/28, Philippe Dax)\n\
\n\
== 6.1\n\
-- 6.1.4\n\
	- update door.cc and vrmysql.cc, remove setSqlState from wobject.cc\n\
	- fix bug in parse.cc where strtok makes a segfault under Linux in libc\n\
	(2004/07/15, Philippe Dax)\n\
\n\
-- 6.1.3\n\
	- change mysql server to an other one, because firewall filtering\n\
	- configure.ac revision (CXXFLAGS & lib paths under MocOSX)\n\
	- add setlocale for international environment, but not used now\n\
	(2004/07/12, Philippe Dax)\n\
\n\
-- 6.1.2\n\
	- unify each object's state in the state.state variable\n\
	- change a lot of #define to static const\n\
	(2004/06/29, Philippe Dax)\n\
\n\
-- 6.1.1\n\
	- new files vac.cc vac.h to handle vac operations\n\
	- fix gate.cc to resolve unknown world urls via the vacs server\n\
	- update chanstr.cc to return strings by reference\n\
	- fix gate.cc to join a group (this bug appeared since version 5.2.0)\n\
	- update ubnavig, vnc, carrier, move for mouseEvent & keyEvent methods\n\
	- move method VglClickToDir from render.cc to clickDirection in solid.cc\n\
	(2004/06/22, Philippe Dax)\n\
\n\
-- 6.1.0\n\
	- change <matrix> </matrix> by <transform> </transform>\n\
	- add <scene> </scene>\n\
	- add <local> </local> tags : local coordinate system (push and pop)\n\
	(2004/06/15, Philippe Dax)\n\
\n\
== 6.0\n\
-- 6.0.0\n\
	- fix worldname of user forgotten when swapping worlds\n\
	- add ptsize=\"val\" attribute\n\
	- resolve keys Command & Option on an Apple keyboard\n\
	- remove attribute body=\"url\", url becomes a child of android\n\
	- remove attribute java=\"class\", class becomes a child of vrelet\n\
	- anim object gets nbframes value from its solid not from an attribute\n\
	- update statue to be tagged\n\
	- tag </solid> at the end of multi-frame solids\n\
	- add tag <frame for multi-frames solids\n\
	(2004/06/06, Philippe Dax)\n\
	- change attribute refname=\"...\" by use=\"...\" for object button\n\
	- add value shape=\"point\"\n\
	- rotations and translations for multi-solids (3dw.cc, solid.cc)\n\
	(2004/06/08, Philippe Dax)\n\
	- default values for book\n\
	- vrelet: attribute class becomes java\n\
	- tag <vre ...> ... </vre>\n\
	- tag <object ...> ... </object>\n\
	- tag <solid ... />\n\
	(2004/06/02, Philippe Dax)\n\
	- font by default for the text object\n\
	- attributes can be written without an imposed order\n\
	- attribute name=\"...\" is managed as other attributes\n\
	- add tag <meta ... /> inside <head> ... </head>\n\
	- parseGeometry becomes parseSolid\n\
	- parseObject becomes parseTag\n\
	- maintain description file .vre in the cache during the session\n\
	- fix XML comments <!-- ... -->\n\
	(2004/05/03, Philippe Dax)\n\
	- fix syntax of vnc and elc objects\n\
	- update vacs for v6\n\
	- new version of syntax (v6)\n\
	(2004/04/30, Philippe Dax)\n\
\n\
++ 5\n\
== 5.2\n\
-- 5.2.0\n\
	- fix http.cc to allow Apache VirtualHost\n\
	- remove /net/vreng prefix\n\
	(2004/04/23, Philippe Dax)\n\
	- view vertical above the user\n\
	- move gcontext.h into vgl.h\n\
	- newlines accepted in the XML object description\n\
	(2004/04/15, Philippe Dax)\n\
	- repeat texture support\n\
	- add ase (Ascii Scene Export) model loader\n\
	- add 3ds (3D Studio Max) model loader\n\
	- view 'turn around' the local user\n\
	- view local user as third person (render.cc, gcontext.h, user.*)\n\
	(2004/03/22, Romain Piegay)\n\
	- use ubit-4.0.2 -> ftp://ftp.enst.fr/pub/ubit/ubit-4.0.2.tar.gz\n\
	- add url.{cc.h} to manipulate urls\n\
	- add new memory counters in stat.*\n\
	- restructuration of http.* and thread.*\n\
	- add --enable-profiling to use gprof tool\n\
	(2004/03/15, Philippe Dax)\n\
	- add --enable-dmalloc with -DMALLOC_FUNC_CHECK in configure.ac\n\
	- new VREP header (version 3), uncompatible with previous version\n\
	- list of transformations in matrix.cc\n\
	- new syntax in matrix.cc\n\
	- fix render.cc vs landmark.cc\n\
	- class lwObject in lwo.cc\n\
	(2004/03/09, Philippe Dax)\n\
\n\
== 5.1\n\
-- 5.1.5\n\
	- fix inet_ntop declaration id MacOSX\n\
	- add bug-report email in configure.ac\n\
	- For each object, initFuncList(), renamed init(), is now in its class\n\
	- enums now inside classes\n\
	- add config.h viewer in the help menu\n\
	- smaller font size in the menubar\n\
	- update servers/vjs/Makefile.am\n\
	- fix a bug in col.cc\n\
	(2004/02/23, Philippe Dax)\n\
\n\
-- 5.1.4\n\
	- add face parameter in solid.cc to choose CULL_FACE\n\
	- more comments to doxygen\n\
	- redefine some classes in bones/\n\
	- fix blend bug in text.cc introduced with 5.1.3\n\
	- fix stillToTurn bug in book.cc introduced with 5.1.3\n\
	- move models loading from model.{h.cc} to format.{h.cc}\n\
	(2004/02/16, Philippe Dax)\n\
\n\
-- 5.1.3\n\
	- fix buffer passwdFile too short in vnc.h\n\
	- add params color, verso, scale to text object\n\
	- update script vreng.in to allow 15 planes on MacOSX\n\
	- update man vreng.1\n\
	- add option -i to show OpenGL infos (GL/GLU versions and extensions)\n\
	- add ogl.cc to check weither OpenGL extensions are supported\n\
	- fix counter color in log.cc by a glDisable(GL_LIGHTING)\n\
	(2004/02/08, Philippe Dax)\n\
\n\
-- 5.1.2\n\
	- add time percentages in stat.cc\n\
	- change default texture size from 128x128 to 256x256\n\
	- update texture.cc to don't resize 128x128, 64x64, 32x32 textures\n\
	- update Image constructor, remove type parameter (still/anim)\n\
	- fix Vjc constructor/destructor\n\
	(2004/02/02, Philippe Dax)\n\
	- fix input parameters in the Vnc dialog box\n\
	- add xpm metaltile and woodtile in theme.cc\n\
	(2004/01/30, Eric Lecolinet)\n\
\n\
-- 5.1.1\n\
	- update app/vnc\n\
	- update vrmysql such as the object's class name is the MySql table name\n\
	- add TGA loader\n\
	(2004/01/26, Philippe Dax)\n\
\n\
-- 5.1.0\n\
	- thanks to chris.coleman@arcucomp.com for fixing minor bugs\n\
	- use automake-1.8.2, autoconf-2.59\n\
	- define HAVE_MYSQL, HAVE_GLU\n\
	- -DDARWIN becomes -DMACOSX\n\
	- replace some #define ... by enum {...};\n\
	- add elc object for further use\n\
	(2004/01/16, Philippe Dax)\n\
\n\
== 5.0\n\
-- 5.0.3 <-- www.opengl.org news\n\
	- submit news to www.opengl.org\n\
	- fix bad <solid=...> to solid=\"...\"\n\
	- speed regulation for ball object according to the vreng rate\n\
	- bouncePosition function to bounce mobile objects on an obstacle\n\
	- project user movement of user on an icon\n\
	- use extension if subtype mime is plain\n\
	(2004/01/12, Philippe Dax)\n\
\n\
-- 5.0.2\n\
	- use open command under MacOSX to do execs in {browser,pdf,office}.cc\n\
	- fix bad value of ssrc by getMySsrcId() in pkt.cc\n\
	(2003/12/14, Philippe Dax)\n\
\n\
-- 5.0.1 <-- Debian 2004\n\
	- fix configure.ac for NetBsd os\n\
	(2003/11/21, Philippe Dax)\n\
\n\
-- 5.0.0\n\
	- add snapshot and fingerprint targets in Makefile.am\n\
	- exec open command to launch MacOSX applications\n\
	- add icon flashing\n\
	- interaction with 'ums' (Ubit Mouse Server)\n\
	- xmlize description of objects (parse.cc)\n\
	- add App::checkUrl using wget --spider\n\
	- rename vijs to vjs (Vreng Java Server)\n\
	- rename ifcserver to vjc (Vreng Java Client)\n\
	- rename ifc to vrelet (VREng servLET)\n\
	- rename wobject.h to wo.h, remove wmgt.h\n\
	- add link behavior in gate object\n\
	- remove link object\n\
	- remove obsoleted cell object\n\
	- add earth and moon behaviors in mirage object\n\
	- remove earth and moon objects\n\
	- merge push pop rot trans objects in only one object called matrix\n\
	- merge wmgt.cc and wobject.cc in wobject.cc\n\
	- rename plain object to mirage object (no interactions)\n\
        - remove panel object, wall is equivalent\n\
	- m4 macros for java moved from aclocal.m4 to acinclude.m4\n\
	(2003/10/01, Philippe Dax)\n\
\n\
== 4.6\n\
-- 4.6.6 <-- Last release before 5.0.0\n\
        - fix my_inet_ntop, my_inet_pton\n\
	(2003/09/01, Philippe Dax)\n\
\n\
-- 4.6.5\n\
	- add VRE_VERSION for description files\n\
	- fix world back and world forward\n\
	(2003/08/27, Philippe Dax)\n\
\n\
-- 4.6.4\n\
	- rename baps to vaps (Vreng Animation Parameters Server)\n\
	- rename ifcs to vijs (Vreng Interaction Java Server)\n\
	- rename reflector to vrum (Vreng Reflector Unicast Multicast)\n\
	- change -d by -D option to allow X11 Toolkit command arguments\n\
	- add --enable-standalone to use vreng without network connectivity\n\
	- add --enable-vacs to use locally the vacs server\n\
	- update channel.cc, gui.cc, vreng.cc to use the vacs server\n\
	- add servers/vacs (Vreng Address Cache Server) to distribute channels\n\
	(2003/07/17, Philippe Dax)\n\
\n\
-- 4.6.3\n\
	- requires ubit-3.4.1 -> ftp://ftp.enst.fr/pub/ubit/ubit-3.4.1.tar.gz\n\
	(2003/07/03, Eric Lecolinet)\n\
	- settings menu redesigned\n\
	- move net/dates to com/dates\n\
	- remove all obsolete #ifndef VRENGD\n\
	- clock object with argument needle=s|m|h\n\
	(2003/07/10, Philippe Dax)\n\
\n\
-- 4.6.2\n\
	- add UBIT_VERSION in configure.ac\n\
	- add format.{cc,h} to handle file extensions and mime subtypes\n\
	- fix clock, remove network operations\n\
	- fix iconRemove to remove local persistent file\n\
	- add roll action to icon\n\
	- add icon persistency for the owner (~/.vreng/icon/*)\n\
	- add iconSave\n\
	- add put and publish file, iconOpen on a published file\n\
	(2003/07/01, Philippe Dax)\n\
\n\
-- 4.6.1\n\
	- require ubit-3.4.0 -> ftp://ftp.enst.fr/pub/ubit/ubit-3.4.0.tar.gz\n\
	- fix control menu position in the 3D (observed with ubit-3.3.0)\n\
	(2003/06/19, Eric Lecolinet)\n\
	- add office loader app/office.cc (staroffice, openoffice)\n\
	- add pdf loader app/pdf.cc (acrobat-reader)\n\
	- add object 'icon' to put, manipulate and share documents into the 3D\n\
	- add m4 macros for java into aclocal.m4 and configure.ac\n\
	- resolve reflector.enst.fr\n\
	- minor bug fixes in vrmysql.cc\n\
	- add createVolatileNetObject()\n\
	- bug fixes in channel.cc\n\
	(2003/06/19, Philippe Dax)\n\
\n\
-- 4.6.0 <-- www.cygwin.com news\n\
	- require ubit-3.3.0 -> ftp://ftp.enst.fr/pub/ubit/ubit-3.3.0.tar.gz\n\
	- update ubxwin.cc to be compatible with ubit-3.3.0\n\
	(2003/05/14, Eric Lecolinet)\n\
	- control rate by checkRate (stat.cc, guy.cc, water.cc)\n\
	- fix lessFrequency in water.cc\n\
	- add inet4_ntop, inet4_pton functions in vrnetdb.cc\n\
	- toggle counter display in landmark.cc \n\
	- reorder AC_CHECK_LIB in configure.ac\n\
	(2003/05/28, Philippe Dax)\n\
\n\
== 4.5\n\
-- 4.5.7\n\
	- add take and drop actions for ball\n\
	- fix aoiQuit\n\
	- add grid colors\n\
	(2003/05/12, Philippe Dax)\n\
\n\
-- 4.5.6\n\
	- fix a huge BUG in move.cc, introduced since Version 4.0.0,\n\
          to enable network publishing for objects in movement\n\
	- classes documentation ala 'doxygen'\n\
	- change returns of GetLine/NextLine\n\
	- add on/off actions in mirror.cc\n\
	- sdrListen now in a new and independent module sdr.cc\n\
	- new functions socketDatragram() and socketStream() in socket.cc\n\
	- rename vrsocket.cc in socket.cc\n\
	- remove obsoleted code about proxy-multicast from channel.cc\n\
	- fix ball up\n\
	(2003/05/07, Philippe Dax)\n\
\n\
-- 4.5.5\n\
	- update aoi using Aoi class\n\
	- update vrmysql using Sql class\n\
	- fix --with-ubit-prefix in configure.ac\n\
	- remove wait.h because conflit with /usr/include/wait.h under some OS\n\
	- book now reads in first the number of pages (lines)\n\
	- change integer types to be in accordance with stdint.h\n\
	- add vgl/glu.{cc,h} in the case where GLU is not found\n\
	(2003/04/17, Philippe Dax)\n\
\n\
-- 4.5.4\n\
	- baps -v gives current version\n\
	- fix a bug in my_inet_ntop\n\
	- check more functions in configure.ac for vrnetdb.cc\n\
	- fix my_wait for a child process\n\
	- add buttons and dialogs to display README, COPYRIGHT, COPYING,\n\
          Changelog, TODO\n\
	- add gui/txt2str.cc to convert files into strings\n\
	- add check of utime.h in configure.ac\n\
	- set current time for files downloaded in the cache\n\
	- accept empty lines in .vre files\n\
	- remove the last fatal calls\n\
	(2003/04/11, Philippe Dax)\n\
\n\
-- 4.5.3\n\
	- build default preferences in ~/.vreng/prefs\n\
	- add callback view world source\n\
	- add callback view worlds list\n\
	- add callback view prefs\n\
	- replace view dialog by view menu containing 4 buttons:\n\
          source, worlds, prefs, axisgrid\n\
	- remove worlds button in the menubar\n\
	- change DEF_HTTP_SERVER with vreng.enst.fr\n\
	- add BMP (Microsoft image format) reader\n\
	(2003/04/01, Philippe Dax)\n\
\n\
-- 4.5.2\n\
	- automatic detection of MBone by listening SAP/SDP announcements\n\
	- new format of book's urls, only one line per page\n\
	- fix downloadInCache for jpg, png, sgi\n\
	- update man vreng.1\n\
	(2003/03/24, Philippe Dax)\n\
\n\
-- 4.5.1\n\
	- add src/servers/reflector/ tree into the distribution\n\
	- add reflector IP Unicast/Multicast for those who are not on the MBone\n\
	(2003/03/21, Samuel Tardieu)\n\
\n\
-- 4.5.0\n\
	- require ubit-3.2.0 -> ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit-3.2.0.tar.gz\n\
	(2003/03/19, Eric Lecolinet)\n\
	- add objects 'book' (#53) and 'sheet' (#54) with appropriated actions\n\
	(2003/03/18, Julien Desreumaux - Sebastien Piraud)\n\
\n\
== 4.4\n\
-- 4.4.5\n\
	- add axis and grid landmarks into the 3D with landmark.{cc,h}\n\
	(2003/03/17, Mario Trentini)\n\
	- fix IP_ADD_MEMBERSHIP by struct ip_mreq in configure.ac\n\
	- fix java2 in configure.ac\n\
	- add gl font -mixed-fixed-medium-r-* to display values into the 3D area\n\
	(2003/03/11, Philippe Dax)\n\
\n\
-- 4.4.4\n\
	- fix compute of codebase url in ifc.cc\n\
	- add stdint.h if available\n\
	- fix bugs in render.cc\n\
	- fix ifcs/Makefile.am\n\
	(2003/03/07, Philippe Dax)\n\
\n\
-- 4.4.3\n\
	- use new autoconf-2.57, automake-1.7\n\
	- remove WANT_WGET\n\
	- add functions: my_inet_ntop, my_inet_pton in vrnetdb.cc\n\
	(2003/03/05, Philippe Dax)\n\
\n\
-- 4.4.2\n\
	- using getipnodeby* if available instead of gethostby*{,_r}\n\
	(2003/03/03, Philippe Dax)\n\
\n\
-- 4.4.1\n\
	- add manual baps.8\n\
	- add actions into mech\n\
	- fix call to gethostbyname_r()\n\
	(2003/02/28, Philippe Dax)\n\
\n\
-- 4.4.0\n\
	- fix an HUGE BUG about tex_xp, tex_xn, tex_yp\n\
	- change all .vre files (v4), but keep the olders (v3) for compatibility\n\
	- new initial world named 'RendezVous.vre' (v4)\n\
	- add function copyPosZAndBB to resolve rotation around X axis\n\
	- add rendering angles of replicated avatars\n\
	- change keysyms, eliminate Shift modifiers, prefer Alt & Meta & Control\n\
	- add alias name for solid shapes\n\
	- add solid attributes: blink, slices, stacks, cylinders, circles\n\
	- add solid shapes: 'cross', 'sphere+disk', 'cone+disk'\n\
	- remove solids: 'boxblend', 'sphereblend' because alpha is detected\n\
	- rename support/ directory to config/ and cleanup it\n\
	- rewrite mirror.cc, add stencil for reflexive solids in render.cc\n\
	- fix texture mapping on the local user\n\
	- use ~/.vreng/cache rather than /tmp for downloading files\n\
	- add manual vreng.1\n\
	- bug fixes applied to be compliant with Debian GNU/Linux distribution\n\
	(2003/02/25, Philippe Dax)\n\
\n\
== 4.3\n\
-- 4.3.1 <-- Debian 'sarge'\n\
	- update music object to play midi pieces of music\n\
	- add call to timidity++ player to play midi sounds by http in app/\n\
	- add Worlds button in menubar for futur feature\n\
	- fix GUI settings\n\
	- fix old help string\n\
	(2003/01/20, Philippe Dax)\n\
\n\
-- 4.3.0\n\
	- add --enable-baps and --enable-ifcs to compile optionnal servers\n\
	- add Mozilla browser\n\
	- Http class added\n\
	- ImgReader class updated\n\
	(2003/01/07, Philippe Dax)\n\
\n\
== 4.2\n\
-- 4.2.0\n\
	- require ubit-2.11.0 -> ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit-2.11.0.tar.gz\n\
	- gui/* compliant with ubit-2.11.0\n\
	(2002/11/20, Eric Lecolinet)\n\
\n\
== 4.1\n\
-- 4.1.11\n\
	- fix lift movement\n\
	- fix check of fpsetmask in configure.in\n\
	(2002/11/19, Philippe Dax)\n\
\n\
-- 4.1.10\n\
	- add basic support for win32: windoze.{cc,h}\n\
	- config.guess (timestamp 2002/07/09), config.sub (timestamp 2002/07/03)\n\
	- add README.VNC in the distrib\n\
	- minor changes in configure.in\n\
	(2002/11/08, Philippe Dax)\n\
\n\
-- 4.1.9\n\
	- add call action to call a lift\n\
	- don't new a World if already exists\n\
	- change world list chaining\n\
	- add entry attribute in gate.cc to comeback with an adequate position\n\
	- add -DREENTRANT in CXXFLAGS if libpthread is there\n\
	(2002/10/21, Philippe Dax)\n\
\n\
-- 4.1.8\n\
	- fix EXTRA_DIST in conf/Makefile.am\n\
	- fix configure.in to link with -lpthread\n\
	- class imgReader to read images\n\
	(2002/10/09, Philippe Dax)\n\
\n\
-- 4.1.7\n\
	- add code to download jpeg files\n\
	- do --enable-pthread by default\n\
	- uncomment --enable-ipv6 (work in progress)\n\
	(2002/10/07, Philippe Dax)\n\
\n\
-- 4.1.6\n\
	- fix bug in configure.in to find libubit in other places\n\
	- move string symbols to other names (string khown by c++)\n\
	- fix blending with glDepthMask\n\
	(2002/10/04, Philippe Dax)\n\
\n\
-- 4.1.5\n\
	- blended solids (shape boxblend)\n\
	- new sub-directory servers and move bap and java inside it\n\
	(2002/09/30, Philippe Dax)\n\
\n\
-- 4.1.4\n\
	- fix guy.cc and mech.cc if libGLU is not present\n\
	(2002/09/17, Philippe Dax)\n\
\n\
-- 4.1.3\n\
	- first port under MacOS X 10.1.x with Xtools successful\n\
	- check libpng and libz, if they exist don't compile vgl/glpng/{png*,z*}\n\
	- fix order of include paths for ubit\n\
	(2002/09/16, Philippe Dax)\n\
\n\
-- 4.1.2\n\
	- move content of sdr and mime directories into conf directory\n\
	- delete cfg directory of vre examples from the distribution\n\
	- delete srv directory from the distribution\n\
	- fix order of include paths and java paths in configure.in\n\
	(2002/09/15, Philippe Dax)\n\
\n\
-- 4.1.1\n\
	- check if socklen_t exists\n\
	- remove obsoleted dependencies in */.deps/\n\
	(2002/09/11, Philippe Dax)\n\
\n\
-- 4.1.0\n\
	- add particle object to produce special effects (rain, snow, fountain)\n\
	- add fish object to swim into the water\n\
	- fix mp3.cc and mpg.cc to handle relative urls\n\
	- fix entry, bgcolor, dbs objects to be unique per world\n\
	(2002/09/06, Philippe Dax)\n\
\n\
== 4.0\n\
-- 4.0.3\n\
	- fixes theme.cc (classical and flashy buttons)\n\
	- fixes to open relative url in browser.cc\n\
	- fixes in ifc parsing\n\
	- fixes in selection mode\n\
	(2002/09/01, Philippe Dax)\n\
	- extract code from android.cc to make a vnp (vertices,normals,polygons)\n\
          loader in vgl/vnp.{h,cc}\n\
	(2002/08/01, Philippe Dax)\n\
\n\
-- 4.0.2\n\
	- split lwo object into model object and lwo loader\n\
	(2002/07/27, Philippe Dax)\n\
\n\
-- 4.0.1\n\
	- move bgcolor structure from render.cc to bgcolor class\n\
	- internal clock handled by the current world\n\
	- selection in 3D on a non solid gives infos on the scene\n\
	- rename cd object to music object\n\
	- rename cinema object to movie object\n\
	(2002/07/27, Philippe Dax)\n\
\n\
-- 4.0.0\n\
	- require ubit-2.6.0 -> ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit-2.6.0.tar.gz\n\
	- revised syntax to describe objects (uncompatible with previous releases)\n\
	  ex. (name) position geometry attribute=value ...\n\
	- the entire code is rewritten in C++ ``spirit''\n\
	- water is now transparent\n\
	- add new cart object to bring in a basket small objects\n\
	- add new carrier object to move small objects\n\
	- add classes: Session NetObject Rtp Channel Payload Solid VGContext App\n\
	(2002/07/12, Philippe Dax)\n\
\n\
++ 3\n\
== 3.6\n\
-- 3.6.0\n\
	- require ubit-2.4.3 -> ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit-2.4.3.tar.gz\n\
	- require libungif.a\n\
	- gui with new ubit release\n\
	(2002/04/22, Eric Lecolinet)\n\
\n\
== 3.5\n\
-- 3.5.7\n\
	- replace fatal() by warning()\n\
	- suppress assert()\n\
	(2002/04/19, Philippe Dax)\n\
\n\
-- 3.5.6\n\
	- fixes to run under Cygwin\n\
	(2002/04/10, Philippe Dax)\n\
\n\
-- 3.5.5\n\
	- add dbs object for future extensions\n\
	(2002/04/08, Philippe Dax)\n\
\n\
-- 3.5.4\n\
	- fix pixmap generation r = g = b (gray) in pgm.cc\n\
	- rename sql.{cc,h} in vrmysql.{cc.h} for further sgbd extensions\n\
	(2002/03/26, Philippe Dax)\n\
\n\
-- 3.5.3\n\
	- add pgm loader in pgm.cc\n\
	- check if mysql_error function exists to avoid link-editing errors\n\
	- new bap and fap buttons\n\
	(2002/03/25, Philippe Dax)\n\
\n\
-- 3.5.2\n\
	- update bap server to run on Win32/Cygwin\n\
	 (2002/03/01, Jose-Marques Soares - int-evry.fr)\n\
\n\
-- 3.5.1\n\
	- option --with-mysql with default=yes in configure.in\n\
	- change int id_objet to string <given_name@world> in the mysql table\n\
	(2002/02/28, Philippe Dax)\n\
\n\
-- 3.5.0\n\
	- add mysql client interface in sql.cc to handle objects persistency\n\
	(2002/02/22, Pierre Gros)\n\
	- http errors !=200 now explicitly printed on stderr\n\
	- adapt lightwave object (LWOB) loader lw.cc with HTTP in lwo.cc\n\
	(2002/02/04, Philippe Dax)\n\
	- add new objects 'push', 'pop', 'trans', 'rot'\n\
	  to handle relative transforms in 3D\n\
	- add new object 'lwo' to render LightWave Objects Models\n\
	(2002/01/25, Pierre Gros)\n\
\n\
== 3.4\n\
-- 3.4.5\n\
	- suppress AC_C_CONST from configure.in, don't check working const\n\
	  resolve compilation errors with <string.h>\n\
	(2002/01/25, Philippe Dax)\n\
\n\
-- 3.4.4\n\
	- all fcntl functions now in vrsocket.cc\n\
	- new files vrsocket.{cc,h} channel independent, remove mcast.cc\n\
	(2002/01/21, Philippe Dax)\n\
\n\
-- 3.4.3\n\
	- change static const int XXX = value by #define XXX value\n\
	- suppress floatingpoint.h which conflicts with stdlib.h\n\
	(2002/01/11, Philippe Dax)\n\
\n\
-- 3.4.2\n\
	- fix synchronization problem in Unicast with baps server in android.cc\n\
	- fix bug computing send timeout in baps.cc\n\
	- complete fap name in face.h and face.cc\n\
	- includes ubit as <ubit/*.hh>\n\
	- relook sysdep.h, random.cc\n\
	- handle new fap2.1 header in android.cc\n\
	- fix bugs pointed out by Debian testers\n\
	(2002/01/07, Philippe Dax)\n\
\n\
-- 3.4.1\n\
	- requires ubit-b1.12.0: ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit.tar.gz\n\
	- fix AC_CHECK_X11 in configure.in to work with ubit, motif and athena\n\
	- fix amwidgets.cc and amxwin.cc to run with recent changes\n\
	(2001/12/03, Philippe Dax)\n\
\n\
-- 3.4.0 <-- Debian stable 'woody'\n\
	- requires ubit-b1.11.7: ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit.tar.gz\n\
	- move and rewrite vnc functions from ubnavig.cc to vnc.cc\n\
	- less dependencies with Xt in ubxwin.cc\n\
	- new control menu to navigate in ubwidgets.cc\n\
	(2001/07/30, Eric Lecolinet)\n\
\n\
== 3.3\n\
-- 3.3.4\n\
	- fix confused --enable --with in configure.in\n\
	- add with-ubit-prefix=DIR in configure.in\n\
	- suppress handle of colormap in ubxwin.cc\n\
	- fix zombie child in baps.cc\n\
	(2001/11/26, Philippe Dax)\n\
\n\
-- 3.3.3\n\
	- no more difference between OpenGL and Mesa3D\n\
	- change all execl to execlp\n\
	- deletion of AC_PATH_PROG in configure.in\n\
	- deletion of helpers/vrpaths.h\n\
	(2001/11/20, Philippe Dax)\n\
\n\
-- 3.3.2\n\
	- FAP integration (quick and dirty ;-)) in android.cc and face.cc\n\
	(2001/11/16, Philippe Dax)\n\
\n\
-- 3.3.1\n\
	- fix magic formula to read bap files version 3.2 in android.cc\n\
	(2001/11/14, Guillaume Brule - int-evry.fr)\n\
	- fix left knee rotation in android.cc\n\
	- adapt bap translation numbers 181,182,183 in android.cc\n\
	- adapt actions to new gestures in android.cc\n\
	- fix getopt parsing in baps.cc\n\
	- change effects of lighting in render.cc\n\
	- suppress gui/tcl* and gui/glut* from the distribution\n\
	- add color parameter for vnc object\n\
	(2001/11/15, Philippe Dax)\n\
\n\
-- 3.3.0\n\
	- replace xresources.cc by getopt in gui.cc\n\
	- do scaling on the fly when downloding models in android.cc\n\
	- moving composed objects in thing.cc\n\
	- add <group>...</group> to group objects in parse.cc\n\
	- add shape type of solid in struct Solid\n\
	- fix args of create_pthread in thread.cc and audio.cc\n\
	- fix immobilty of water after a laps time\n\
	- add comments for model files *.m\n\
	(2001/11/12, Philippe Dax)\n\
\n\
== 3.2\n\
-- 3.2.5\n\
	- fix ubxwin.cc: no more reference to colormap\n\
	(2001/10/29, Alexandre Fenyo)\n\
	- update vreng script-shell: no more reference to vreng-tgl (TinyGL)\n\
	- new DBG flags: DBG_MAN, DBG_VNC, DBG_IFC\n\
	- ifcserver: server-port and local-port become optional in vre file\n\
	(2001/10/29, Philippe Dax)\n\
\n\
-- 3.2.4\n\
	- fix vreng script-shell about LD_LIBRARY_PATH\n\
	- fix the vnc destructor causing a seg-fault\n\
	- fix baps.cc to broadcast with a ttl scope of 127\n\
	- fix some check-headers in configure.in\n\
	- fix wrong action list in android.cc\n\
	- fix too many open files in baps.cc\n\
	(2001/10/24, Philippe Dax)\n\
\n\
-- 3.2.3\n\
	- fix replication side effects by suppressing unusefull WObject copies\n\
	(2001/10/11, Philippe Dax)\n\
\n\
-- 3.2.2\n\
	- add skin color parameter to android object\n\
	- check inet_pton in configure.in\n\
	- unification of all http i/o functions in http.cc\n\
	(2001/10/09, Philippe Dax)\n\
\n\
-- 3.2.1\n\
	- fix gethostbyaddr() call in baps.cc\n\
	- fix select() CPU looping to become blocking in baps.cc\n\
	- rename 'lara' object to 'android'\n\
	(2001/09/24, Philippe Dax)\n\
\n\
-- 3.2.0\n\
	- bap streaming send by IP Multicast to all participants by default\n\
	- add buttons 'yes' and 'no' for face animation\n\
	- add buttons 'smile' and 'sulk' in lara actions\n\
	- face animation called by Lara::changePermanent()\n\
	- face models indirectly pointed by the content of the joint-point url\n\
	- each lara object can define its own face by an url (face=url_face)\n\
	- each lara object can define its own bap server (baps=bap_host)\n\
	(2001/09/21, Philippe Dax)\n\
\n\
== 3.1\n\
-- 3.1.0\n\
	- update client/server handshaking to play bap files\n\
	- fix dimension and space position of lara object\n\
	- fix skin color and collide behavior in lara object\n\
	(2001/08/27, Philippe Dax)\n\
	- add face object in conjunction with body's lara\n\
	- add face animation built with bones, zv/bones tree\n\
	(2001/08/03, Yann Renard)\n\
\n\
-- 3.0.0	<-- (full C++) - unstable, obsoleted by 4.0.0\n\
	- requires ubit-b1.7.0: ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit.tar.gz\n\
	(2001/07/27, Eric Lecolinet)\n\
	- add src/bap tree containing Mpeg4/bap server\n\
	- add checking java2 path in configure.in\n\
	- update config.guess & config.sub\n\
	(2001/07/27, Philippe Dax)\n\
	- add src/java tree for ifcserver\n\
	(2001/07/27, Mathieu Seigneurin)\n\
	- add downloader in 'lara' for models coming from 3D Studio Max tool\n\
	(2001/07/10, Philippe Dax)\n\
	- new directory src/java\n\
	- add new object ifcserver(#43) (Interface Communication Server)\n\
	- add new object ifc(#42) (Interface Communication)\n\
	(2001/06/29, Mathieu Seigneurin)\n\
	- add new object vnc(#41) (Virtual Network Computing)\n\
	- add new helper directory vnc (Virtual Network Computing)\n\
	- build a CVS tree\n\
	(2001/07/06, Martin Renard, Mathieu Picard, Laurent Maingault,\n\
		       Stephane Ploix, Clement Sommelet)\n\
	- network interface in 'lara' to read Mpeg4 flows in Bap format\n\
	- add in 'lara' an other model built with triangles called 'man'\n\
	- add new object lara(#40) a Lara Croft like\n\
	(2001/06/15, Ankit Jalote)\n\
	- WClass instance class for objects\n\
	- WObject is the top level class for all objects\n\
	- all objects have their own class\n\
	- all *.c source files are now rewritten in C++ (*.cc)\n\
	- add intuitive mouse navigation\n\
	(2001/05/29, Eric Lecolinet, Philippe Dax)\n\
\n\
++ 2\n\
== 2.6\n\
-- 2.6.5	<-- stable in C language\n\
	- bug fixes for Motif\n\
	(2001/06/21, Philippe Dax)\n\
\n\
-- 2.6.4\n\
	- add intuitive mouse navigation\n\
	- requires ubit-b1.5: ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit.tar.gz\n\
	(2001/05/22, Eric Lecolinet)\n\
	- delete some memset on WObject\n\
	(2001/05/28, Philippe Dax)\n\
\n\
-- 2.6.3\n\
	- rewrite 3dw.c interaction between objects and solids\n\
	(2001/05/09, Philippe Dax)\n\
\n\
-- 2.6.2\n\
	- minor bug fixes\n\
	(2001/04/28, Philippe Dax)\n\
\n\
-- 2.6.1\n\
	- add new object thing(#39) to be dragged and dropped\n\
	(2001/04/09, Philippe Dax)\n\
	- code cinema.c to map Mpeg1 videos on textures in 3D\n\
	(2001/03/30, Raphael Duee, Nicolas Durand, Wakim Hakim, Nicolas Wack)\n\
\n\
-- 2.6.0\n\
	- ./configure uses by default Ubit & Mesa3D vs X11-Athena & TinyGL\n\
	(2001/03/26, Philippe Dax)\n\
	- comment Glut & Tcl/Tk targets in configure.in because not supported\n\
	- add AC_CHECK_LIB for libmpeg in configure.in\n\
	- add warning notifications when the avatar collides a gate or a link\n\
	(2001/03/22, Philippe Dax)\n\
\n\
== 2.5\n\
-- 2.5.9\n\
	- fix SolidGetSelection for objects with specialRendering\n\
	(2001/03/19, Patrick Bellot)\n\
	- add bbox solid geometry without draw for objects with specialRendering\n\
	- enable drawing with LINES for box, rect, cylinder, disk, octagon\n\
	- add style parameter in geometry description (0=fill,1=lines,2=points)\n\
	- add bookmarks button (add bookmark & list bookmarks)\n\
	(2001/03/18, Philippe Dax)\n\
\n\
-- 2.5.8\n\
	- requires ubit-0.14.2: ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit-0.14.2.tar.gz\n\
	- new 3D mouse navigator with accelerators\n\
	- new GUI background (metal)\n\
	(2001/03/18, Eric Lecolinet)\n\
\n\
-- 2.5.7\n\
	- fix move.c when an object has no BB neither elementary movement\n\
	- add new object guy(#38) taken from glut/progs/demos/walker\n\
	- CSet (Curve Set) loader\n\
	- fix rotations & translations in mech.c\n\
	(2001/03/12, Philippe Dax)\n\
\n\
-- 2.5.6\n\
	- bug fix in stars.c stars_colors\n\
	- bug fix in text.c when mapping the string\n\
	(2001/02/22, Patrick Bellot)\n\
\n\
-- 2.5.5\n\
	- replace names MesaGL by Mesa3D, oops!\n\
	- when it'is possible change assert()\n\
	- suppress handling of colors in amxwin.cc\n\
	- change API/GUI for Gui*User and Gui*World\n\
	- fixed bug when receiving RTCP/SDES packets\n\
	(2001/02/19, Philippe Dax)\n\
\n\
-- 2.5.4\n\
	- add a memory cache for txf fonts\n\
	- add URLs to retrieve additional softwares in configure.in\n\
	- fix quitWorld() which doesn't do a closeChannel()\n\
	- add renderMaterials() in render.c\n\
	- add new object link(#37) to go in a world with the same channel\n\
	- move static variables of mech, stars, text, water in WObject->ext\n\
	(2001/02/12, Philippe Dax)\n\
\n\
-- 2.5.3\n\
	- add material GL_EMISSION, lights GL_SPOT_CUTOFF, GL_*_ATTENUATION\n\
	- add scripts configure-{athena,motif,ubit,glut}-{ogl,mgl,tgl}\n\
	- fix glut*.[hc] files\n\
	- clean up code\n\
	- less include dependencies between modules\n\
	(2001/02/05, Philippe Dax)\n\
\n\
-- 2.5.2\n\
	- adapt texfont.c and text.c for TinyGL\n\
	- update generalFunctionList[n].render with (WObject *pobj) in argument\n\
	- delete generalFunctionList[n].light\n\
	- struct Nature in struct WObject\n\
	(2001/01/29, Philippe Dax)\n\
\n\
-- 2.5.1\n\
	- add new object cinema(#36) (only its template)\n\
	- add http loader for TXF format fonts taken from Mark J. Kilgard \n\
	- add new object text(#35) to render strings using texture-mapped fonts\n\
	- add new object light(#34)\n\
	- shininess with only one argument\n\
	- suppress irrelevent solids: user,wall,door,clock,mirror,ring,wheel\n\
	(2001/01/26, Philippe Dax)\n\
\n\
-- 2.5.0\n\
	- requires ubit-0.12.4\n\
	- new popup navigator to accelerate linear and angular speeds\n\
	(2001/01/12, Eric Lecolinet)\n\
	- no more documentation in the distribution, use the Web\n\
	- new look 3D zone with 16/9 format (like cinema)\n\
	- add void *datas argument to specialAction and all methods\n\
	- move Move structure into WObject structure\n\
	- all objects with angle a3\n\
	- add new object plain(#33), plain is an inert object\n\
	- suppress objects vrml, cone, disk, wheel, ray, ring, sphere\n\
	- cone, disk, sphere, ray, wheel, ring are solid shapes, not objects\n\
	- objects web and vrml are now merged into web object\n\
	- explicit names and url are now in the WObject structure\n\
	- suppress net-strings from parsing for door, anim, button, clock, lift\n\
	- add parsing functions\n\
	- all objects can be explicitly named in config files\n\
	- popmenu displays instance_name\n\
	- earth turns now in the right sense\n\
	- automatic gothrough when collide a gate\n\
	- new object attr: collision behaviour, viscosity, density, persistency\n\
	- bug fixed in col.c when intersect is in progress\n\
	- bug fixed in RtpDump against loops\n\
	- mech object with anim and walk flags\n\
	- suppress a2 angle from panel\n\
	(2001/01/12, Philippe Dax)\n\
\n\
== 2.4\n\
-- 2.4.1\n\
	- requires ubit-0.12.0\n\
	- add actions pause and continue in clip.c and cd.c\n\
	- move vre.in to vreng.in to create script-shell vreng and vre\n\
	- use OGL, MGL, TGL in vreng/Makefile.am to create executables\n\
	- add OGL, MGL, TGL conditional variables in configure.in\n\
	- add 'back' and 'forward' buttons to navigate into visited worlds\n\
	- add callbacks backWorld & forwardWorld\n\
	- worlds are now allocated and chained in a list called worlds\n\
	- add feature in clip.c to get A/V streams played by rtpplay on a server\n\
	(2000/12/17, Philippe Dax)\n\
	- add translucid buttons into the 3D to navigate natively with the mouse\n\
	- add translucid popup menus\n\
	- split xwin.cc into 2 files: ubxwin.cc, amxwin.cc\n\
	(2000/12/15, Eric Lecolinet)\n\
\n\
-- 2.4.0\n\
	- add conf/vrengicon.xpm in order to iconize with a Window Manager\n\
	- bugs fixed in bullet.c and dart.c when network replication\n\
	- change {add,update,remove}World in ubwidgets.cc, amwidgets.cc\n\
	- change {Add,Update,Remove}World in gui.cc, gui.h\n\
	- new structures World and Universe in world.h\n\
	(2000/12/07, Philippe Dax)\n\
	- add Popup Menus when selecting an object in the 3D area\n\
	- add contextual colors depending on messages: chat, notice, warning\n\
	  requires ubit-0.11.1\n\
	(2000/11/29, Eric Lecolinet)\n\
	- bug fixed in md2.{ch} if 64 bits architecture\n\
	- add dumpSessions() for debugging\n\
	- add properties (web, world, ssrc, rctpname) for user replicas\n\
	- add helpers/modeller.[ch] to launch an 3D modeler\n\
	- add png.c to download PNG files and apply them on textures\n\
	- merge libglpng from Ben Wyatt into zv/ to downdload png files\n\
	(2000/11/29, Philippe Dax)\n\
\n\
== 2.3\n\
-- 2.3.7\n\
	- replace URL files extensions by MIME types to download images\n\
	- split rocket(#16) in 2 different objects: bullet(#16) & dart(#38)\n\
	- add object mech(#39) adapted from Simon Parkinson-Bates's source\n\
	(2000/11/13, Philippe Dax)\n\
\n\
-- 2.3.6\n\
	- replace md5 from RSA by md5 from FreeBSD\n\
	(2000/10/24, Philippe Dax)\n\
	- some memory leaks fixed\n\
	(2000/10/14, Philippe Dax)\n\
\n\
-- 2.3.5\n\
	- keys to increase/decrease/default linear & angular avatar's speeds\n\
	- methods to increase/decrease/default linear & angular avatar's speeds\n\
	- --enable-pthread is not by default in configure.in\n\
	- fix pre-selected buttons in ubwidgets.cc\n\
	- announce projectile sounds on the network\n\
	- add built sounds drip.au and shout.au\n\
	- add helpers/wget.[hc]\n\
	- add tool helpers/slidecast.[hc] to send/receive slides\n\
	- add preferences file conf/prefs in the distrib\n\
	(2000/09/27, Philippe Dax)\n\
\n\
-- 2.3.4\n\
	- --enable-pthread is now the default in configure.in\n\
	- add --enable-permissive in configure.in to compile with --permissive\n\
	- playsound() multithreaded -> better performence\n\
	(2000/09/21, Philippe Dax)\n\
\n\
-- 2.3.3\n\
	- clear trailer bits from previous sound (Patrick Bellot)\n\
	- bug fixed in vrnetdb.c if option --enable-pthread\n\
	- ambient sound 'bubbles' when entering into the water\n\
	- suppress snd/ directory from distrib\n\
	- builtin sounds using base64: common/base64.[hc], helpers/snd.[hc]\n\
	(2000/09/18, Philippe Dax)\n\
\n\
-- 2.3.2\n\
	- modify configure.in to find 64 bits libraries\n\
	- modify vre script to call vreng appl 64 bits\n\
	(2000/09/14, Philippe Dax)\n\
	- rename objects.h -> wobject.h (because objects.h exists in ssh dist)\n\
	(2000/09/11, Philippe Dax)\n\
\n\
-- 2.3.1\n\
	- add --enable-optimize in configure.in\n\
	- deletion of src/include and gui/GLw*\n\
	(2000/09/03, Philippe Dax)\n\
	- bugs fixed in moon.c\n\
	- change long to int for init32 and u_int32 (for compiling with 64 bits)\n\
	(2000/08/23, Patrick Bellot)\n\
	- add object water(#37)\n\
	- profiling time functions startTime, stopTime, clearTime\n\
	- add checking GLU lib in configure.in\n\
	- add solid boxblend\n\
	- bug fixed in drawRect()\n\
	(2000/08/23, Philippe Dax)\n\
\n\
-- 2.3.0\n\
	- new UI ubit (Ubiquitous Brick ToolKit)\n\
	- navigation with clicks into arrow buttons (not full mouse navigation)\n\
	- work to do more independent files in gui structure\n\
	- add ubit code (.cc,.hh) in gui/u*\n\
	(2000/07/27, Eric Lecolinet)\n\
	- add --with-ubit in configure\n\
	- new object ftp(#36), to download files (as tarballs)\n\
	(2000/07/23, Philippe Dax)\n\
	- bug fix in x11main.c to authorize 16 bits depth visuals\n\
	(2000/07/13, Mathieu Imrazene)\n\
	- decrease texture size from 256x256 to 128x128 to improve rendering\n\
	(2000/07/07, Mathieu Imrazene)\n\
\n\
== 2.2\n\
-- 2.2.3\n\
	- add --enable-64bit into configure.in if gcc-2.96 available\n\
	(2000/07/03, Philippe Dax)\n\
	- update support/config.{sub,guess} taken from gcc-2.95.2\n\
	- update src/*/Makefile.am to build separated libraries for vrengd\n\
	- directory src/lib deleted\n\
	(2000/06/28, Philippe Dax)\n\
	- add SGI-images (la, bw, rgb, rgba, sgi) decoder (sgi.{hc})\n\
	(2000/06/26, Philippe Dax)\n\
\n\
-- 2.2.2\n\
	- add common/wait.[hc] to do my_wait()\n\
	- add PHTML type to read webpages.phtml\n\
	- add code to use a Multicast Unicast Proxy (MUP) in mcast.c\n\
	(2000/06/15, Philippe Dax)\n\
	- add detection of Multicast Unicast Proxy 'mcast_proxy'\n\
	- improve httpParser\n\
	- add telnet scheme in http.c\n\
	- bug fixed in moon.c\n\
	(2000/06/11, Philippe Dax)\n\
\n\
-- 2.2.1\n\
	- CFLAGS -DVRENG if --enable-vrengd given\n\
	(2000/06/09, Philippe Dax)\n\
\n\
-- 2.2.0\n\
	- major reorganization of gui/x11*\n\
	(2000/06/06, Eric Lecolinet)\n\
\n\
== 2.1\n\
-- 2.1.0\n\
	- overload netstrings by %d/%d, objTYPE, objID++ to keep uniqueness\n\
	- dismiss loop in genaralIntersect (col.c)\n\
	- repare model.c\n\
	- now slide along walls (wall, board, gate and opened door)\n\
	- fix getSeletion with the good camera->near\n\
	- new object moon(#35), moving permanently along a circonference\n\
	(2000/06/03, Philippe Dax)\n\
	- detection of libz (compression) in configure.in\n\
	- payload length in words in the packet if flags field = 1\n\
	  warning: obsoletes previous versions !!!\n\
	- new functions (payload.c): seekPayload, tellStrInPayload, tellPayload\n\
	- bug fixed in http.c if environment variable no_proxy undefined\n\
	- add stat max_packet & min_packet in stat.c\n\
	- repair lift.c\n\
	- declare user geometry to the network\n\
	(2000/05/27, Philippe Dax)\n\
\n\
== 2.0\n\
-- 2.0.3\n\
	- change --enable-thread by default from 'yes' to 'no' (more robust)\n\
	  some core dumps occur inside gethostbyname_r()\n\
	- add parsing of preferences (world, geometry) in env.c\n\
	- change float strings to float in user.h\n\
	(2000/05/26, Philippe Dax)\n\
\n\
-- 2.0.2\n\
	- new object clip(#34) to play mpeg clips\n\
	- add mpg helpers using mtvp (with audio) or mpeg_play (without audio)\n\
	- change mp3.c to play in streaming mode\n\
	- translate avatar'eyes at the right location\n\
	(2000/05/25, Philippe Dax)\n\
-- 2.0.1\n\
	- update vre script to run vreng on Solaris2.6\n\
	- add bandwidth info in stat.c integrating IP, UDP, RTP headers\n\
	- clock in 3 clock objects (secondes, minutes, hour)\n\
	(2000/05/23, Philippe Dax)\n\
\n\
-- 2.0.0 <-- stable, obsoletes previous releases, obsoleted by 3.0.0\n\
\n\
	- all threads stuff in thread.c\n\
	(2000/05/22, Philippe Dax)\n\
	- refresh timeout grows as log(nb_sources) to limit bandwidth\n\
	(2000/05/19, Philippe Dax)\n\
	- bug fixed in http.c if no threads (Samuel Tardieu)\n\
	- add object names for aoi, gate, host, doc, cd, clock\n\
	- new object clock(#33)\n\
	(2000/05/17, Philippe Dax)\n\
	- add ray(#32) object\n\
	- add line solid\n\
	- add mirror(#31) object\n\
	- add sphere(#30) object\n\
	- objects.c (from initobj.c & parse.c)\n\
	(2000/05/16, Philippe Dax)\n\
	- now menubar is independent from paths found by configure\n\
	- add include/paths.h for helper applications\n\
	- unlink /tmp/files_wget_downloaded\n\
	- add cd(#29) object to play piece of music\n\
	- add mp3 launcher (mpg123, xaudio, freeamp)\n\
	- add bgcolor(#28) object to define its own background\n\
	- add wheel(#27) solid & object\n\
	- add ring(#26) solid & object\n\
	- add stars(#25) solid & object\n\
	- add panel(#24) object\n\
	(2000/05/10, Philippe Dax)\n\
	- add disk(#23) solid & object\n\
	- bug fixed in parseName\n\
	- add lib directory where lives libvreng.a\n\
	(2000/05/05, Philippe Dax)\n\
	- add --enable-vrengd into configure.in & vrengd variabe into global.c\n\
	- new src directories common/ and server/\n\
	- add --with-qt into configure.in\n\
	- bug fixed in channel.c, pkt.c, proto.c\n\
	(2000/05/02, Philippe Dax)\n\
	- src_id is now my-ssrcid (IPv4/v6 independent), older was my_hostid\n\
	- add functions getChannelbysa, getFdSendRTP, getFdSendRTCP, getSaRTCP\n\
	- bug fixed in RtpCreateSession\n\
	(2000/05/01, Philippe Dax)\n\
	- change payload type from 105 (dynamic) to 65 (not assigned)\n\
	- mc.c functions into channel.c\n\
	- add vreng/inits.[hc] to groups system initializations\n\
	- relooking of rtpsess.c\n\
	(2000/04/24, Philippe Dax)\n\
	- add IPv6 detection into configure.in\n\
	(2000/04/21, Philippe Dax)\n\
	- rename in helpers html.c -> browser.c, vrml.c -> vrbrowser.c\n\
	- add persistency functions into proto.c\n\
	(2000/04/19, Philippe Dax)\n\
	- new Protocol VREP-V2 - change payload header 4 bytes / old 7 bytes\n\
	- change VREP-V2 command from ascii to binary\n\
	- add xmesa.h into src/include/GL (Linux compiling)\n\
	(2000/04/14, Philippe Dax)\n\
	- add code to handle a manager group common to all applications\n\
	- change configuration files suffixes .cfg to .vre\n\
	(2000/04/12, Philippe Dax)\n\
	- move aux/ to support/ (Windows recognizes aux as a logical device :-))\n\
	- add CYGWIN32 in acconfig.h and configure.in\n\
	(2000/03/27, Philippe Dax)\n\
	- hardcoded sin, cos, tan tables\n\
	(2000/03/22, Philippe Dax)\n\
	- split zview.c into solid.c draw.c render.c\n\
	(2000/03/20, Philippe Dax)\n\
	- fix texture.c; url non null terminated\n\
	(2000/03/17, Patrick Bellot)\n\
	- fix drawCylinder (GLU_FILL style)\n\
	- add --enable-thread into configure.in\n\
	(2000/03/16, Philippe Dax)\n\
	- add src subdir where live all the sources\n\
	- move gui/*menu.[ch] in gui/*gui.[hc]\n\
	- move gui/*init.[ch] in gui/*w3d.[hc]\n\
	- move gui/chat.[ch] in gui/*cb.[hc]\n\
	- move gui/members.[ch] in gui/*cb.[hc]\n\
	- add gui/cb.h (call callbacks)\n\
	- split gui/supportfuncs.[ch] into x11cb.[ch], glutcb.[ch], tkcb.[ch]\n\
	(2000/03/13, Philippe Dax)\n\
	- add --with-motif, --with-glut, --with-tcltk in configure.in\n\
	(2000/03/08, Philippe Dax)\n\
	- merge Glut code written by Jean-Francois Karr (30/06/98) into gui/\n\
	(2000/02/28, Philippe Dax)\n\
	- add object cone(#22) in cone.c\n\
	- add solid cylinder/cone in zview.c\n\
	(2000/02/25, Philippe Dax)\n\
	- merge Tcl/Tk code written by Adrien Felon (13/09/99) into gui/\n\
	- add configure.in.tk of vic-1.0ucl from cs.ucl.ac.uk\n\
	(2000/02/21, Philippe Dax)\n\
	- add include/types.h\n\
	- del gui/callbaks.h & gui/wmgtincl.h\n\
	(2000/02/20, Philippe Dax)\n\
\n\
++ 1\n\
== 1.6\n\
-- 1.6.5 <-- stable, obsoletes previous releases, obsoleted by 2.0.0\n\
\n\
	- #define FPSET_MASK if FREEBSD defined\n\
	(2000/01/14, Philippe Dax)\n\
\n\
-- 1.6.4\n\
	- accept compound RTCP packets\n\
	- bug fix in receive RTCP_SDES\n\
	(1999/12/17, Philippe Dax)\n\
\n\
-- 1.6.3\n\
	- bug fix in SendDelta confused SEND_RTP -> SOCK_RTP\n\
	(1999/12/10, Philippe Dax)\n\
	- RTP and RTCP fixes\n\
	(1999/12/10, Samuel Tardieu)\n\
\n\
-- 1.6.2\n\
	- change initial port number 62666 to 52666 (Linux Mascarade feature)\n\
	- some minor [hn]to[nh]s fixes\n\
	(1999/12/05, Philippe Dax)\n\
\n\
-- 1.6.1\n\
\n\
	- add entry(#21) pseudo-object to define user origin in a new world\n\
	(1999/12/01, Philippe Dax)\n\
	- rtp.h upgraded with BIG/LITTLE ENDIAN\n\
	- new keys to zoom/unzomm, to tilt left/right\n\
	- fpsetmask detected by configure\n\
	- some hton[sl] fixes\n\
	(1999/11/04, Philippe Dax)\n\
\n\
-- 1.6.0 <-- WARNING: this release is uncompatible with 1.5.8 !!!\n\
		  First release RTP/RTCP, previous was UDP\n\
\n\
	- new functions to parse objects description (wmgt/parse.c)\n\
	- bug fixed: compile without any browsers\n\
	- add door status (opened/closed/[un]lock) in wmgt/door.[hc]\n\
	- add mime directory in the distribution to launch vreng by the Web\n\
	(1999/10/19, Philippe Dax)\n\
	- nice the vreng process to run at low priority when cpu conflicts\n\
	- new sdr_plugin: sdr2.plugin.dvr.rtp.vre.vreng\n\
	- new structure Channel to manage multiple Network IOs in net/channel.h\n\
	- reduce NetObjectID in order to transfering less datas on the network\n\
	(1999/10/12, Philippe Dax)\n\
	- new functions: getfdbysa(), getsabysa() in net/channel.c\n\
	(1999/10/07, Philippe Dax)\n\
	- RTP/RTCP session integrated from contribution of Vincent Haverlant\n\
	(1999/10/01, Vincent Haverlant and Philippe Dax)\n\
	- split NetInit() into ChannelInit() and ChannelOpen()\n\
	- split newWorld() into quitChannel() and newChannel()\n\
	- bug fixed in ps.c if GVPATH not defined\n\
	- add file FIXES in the distribution\n\
	(1999/09/24, Philippe Dax)\n\
	- add trace function for debugging\n\
	(1999/08/10, Philippe Dax)\n\
\n\
== 1.5\n\
-- 1.5.8 <-- stable, obsoletes previous releases, obsoleted by 1.6.0\n\
\n\
	- bug fixed in ps.c while link-editing\n\
	(1999/07/24, Philippe Dax)\n\
	- aoi deletion\n\
	(1999/07/19, Adrien Felon)\n\
\n\
-- 1.5.7\n\
	- avatars can meet themselves inside the same Aoi, but not outside\n\
	(1999/07/16, Adrien Felon)\n\
	- bug fixed in stat.c (session time)\n\
	(1999/07/16, Philippe Dax)\n\
\n\
-- 1.5.6\n\
	- walls deletion done when quitting walls (walls.[hc])\n\
	(1999/07/13, Adrien Felon)\n\
	- repackaging in gui/: log.[hc], chat.[hc], members.[hc]\n\
	(1999/07/13, Philippe Dax)\n\
\n\
-- 1.5.5\n\
	- add method print in doc.c and helper a2ps in ps.c\n\
	- bug fix in zview,c about 2D textures\n\
	(1999/07/12, Philippe Dax)\n\
\n\
-- 1.5.4\n\
	- some fixes about aoi (move.c, aoi.c)\n\
	(1999/07/09, Adrien Felon)\n\
	- bug fixed in initSignal for Linux\n\
	- add initEnv() to create ~/.vreng/{prefs,cache/}\n\
	- global.[hc] revisted\n\
	- helper ps.c using wget to retreive url and launch gv/ghostview\n\
	(1999/07/09, Philippe Dax)\n\
\n\
-- 1.5.3\n\
	- add dart sub-object as an alternative to bullet in rocket object\n\
	- add object doc(#19) to be read by an Postscript or Pdf interpreter\n\
	- mctools/ becomes helpers/, includes/ -> include/, w_mgt -> wmgt/\n\
	- keys.c separated from widget3D.c\n\
	- add argument -v or --version on command line\n\
	- add initsysinfo() in http.c for tracing URLs\n\
	- update html/vreng.html\n\
	(1999/07/07, Philippe Dax)\n\
\n\
-- 1.5.2\n\
	- add object aoi(#18) (Aera of Interest) without intersection when\n\
	  we are in and invisible (w_mgt/aoi.[hc])\n\
	(1999/07/05, Adrien Felon)\n\
	- cell is now a solid without visibilty\n\
	(1999/07/04, Philippe Dax)\n\
\n\
-- 1.5.1\n\
	- merging debug flags by shift\n\
	(1999/07/02, Adrien Felon)\n\
	- warning replaces fatal if urlcfg doesn't exist (w_mgt/file.c)\n\
	(1999/07/01, Philippe Dax)\n\
\n\
-- 1.5.0\n\
	- add cell(#17) object for thin grain multicast areas (w_mgt/cell.*)\n\
	- work on code readability\n\
	- repackaging w_mgt/ includes/\n\
	- repackaging net/, new files, first bricks to re-implement RTP :-)\n\
	- sender packet format RTP capable\n\
	- receiver compatible with current packet header and futur RTP\n\
	- add info ?version=<version> a the end of the URLs (Samuel Tardieu)\n\
	- buf fixed for 'rect' 2D shade in zview.c\n\
	- all theses changes leads to upgrade medium version from 1.4 to 1.5\n\
	(1999/06/29, Philippe Dax)\n\
\n\
== 1.4\n\
-- 1.4.11 <-- obsoleted\n\
	- bug fix floating point exception in freeBSD - add fpsetmask(0L)\n\
	- bug fix in mctools/whiteboard.h\n\
	(1999/06/18, Philippe Dax)\n\
\n\
-- 1.4.10\n\
	- change name of wrapper (vreng -> vre) for $prefix/bin\n\
	  and in sdr2.plugins\n\
	- Makefile.am for vreng subdir\n\
	- vre script built by configure & vre.in\n\
	(1999/06/15, Philippe Dax, Samuel Tardieu)\n\
\n\
-- 1.4.9\n\
	- bug fix text too long (net/net_text.c)\n\
	(1999/06/10, Philippe Dax)\n\
\n\
-- 1.4.8\n\
	- add sound directory snd/ in distribution\n\
	(1999/06/10, Philippe Dax)\n\
\n\
-- 1.4.7\n\
	- Change X11 resource class name from VReng to Vreng (gui/initUI.*)\n\
	(1999/06/07, Philippe Dax)\n\
\n\
-- 1.4.6\n\
	- add script wrapper to launch vreng.{tgl,ogl}\n\
	  because TinyGL dont't find the Visual True Color on Solaris2.7\n\
	  to compile vreng on Solaris2.7 do configure --with-lib-GL\n\
	(1999/06/04, Philippe Dax)\n\
\n\
-- 1.4.5\n\
	- bug fix in ttl import\n\
	- bug fix in audio.c (bad ttl args)\n\
	(1999/05/18, Philippe Dax)\n\
\n\
-- 1.4.4\n\
	- add ssh (default) alternative to telnet, xterm.[hc]\n\
	(1999/05/07, Philippe Dax)\n\
\n\
-- 1.4.3\n\
	- include glu.h was misssing\n\
	(1999/03/01, Philippe Dax)\n\
\n\
-- 1.4.2\n\
	- some repackaging\n\
	- some fixes\n\
	(1998/12/12, Philippe Dax)\n\
\n\
-- 1.4.0\n\
	- corrected camera coordinates (still some problems, but better)\n\
	- added walls(#20) independant from objects (for the moment, only\n\
	  'grid' walls allowed).\n\
	- added 'MiMaze' simulation (straight walls + rockets)\n\
	(1998/06/30, Lionel Ulmer)\n\
\n\
== 1.3\n\
-- 1.3.7 <-- obsoleted\n\
	- added object naming for doors (to allow an object to call a method\n\
	  from another).\n\
	- changed object geometry loading method.\n\
	- added multiple frames for one model (MD2) without reloading it\n\
	- added 'Animator' object\n\
	- added 'Lift' (in fact, a movable step)\n\
	(1998/06/27, Lionel Ulmer)\n\
\n\
-- 1.3.6\n\
	- added frames to geometry, changes in parser to have multiple \n\
	  geometry for one object.\n\
	- add new objet to test frames 'Button'\n\
	(1998/06/25, Lionel Ulmer)\n\
\n\
-- 1.3.5\n\
	- added 'scale' parameter to MD2 loading\n\
	- added .MD2 file loading (Quake 2 models format) and .PCX format for\n\
	  images.\n\
	- changed .PCX loading to a 'streaming' load (instead of saving the\n\
	  data in a file before decoding).\n\
	- did the same for .MD2 files.\n\
	(1998/06/24, Lionel Ulmer)\n\
\n\
-- 1.3.4\n\
	- bug fix in OpenGL Widget initialization\n\
	- added 'world initialized' and 'zlib initialized' flags to prevent\n\
	  calling a close function before having done the initialisation\n\
	- bug fix in world changing and quitting\n\
	- quality option now works\n\
	- bug fix in selection \n\
	(1998/05/27, Lionel Ulmer)\n\
\n\
-- 1.3.3\n\
	- Open GL/Mesa porting. Use of GLX in the GUI. autoconf fix\n\
	- renaming of the files zlib.h & zlib.c to zview.h & zview.c\n\
	- major update of TinyGL. Separate distribution\n\
	(1998/05/24, Fabrice Bellard, Lionel Ulmer)\n\
\n\
-- 1.3.2\n\
	- bugs fix\n\
	(1998/05/24, Fabrice Bellard)\n\
	(1998/03/25, Stephane Belmon)\n\
\n\
-- 1.3.1\n\
	- initial group from SDR or shell-command is kept to be used in the\n\
	  future as a supervisor group (multicast addresses allocation)\n\
	- smooth sliding when an avatar intersects with an other one\n\
	- supress automatic refreshing to don't agress the network\n\
	  and add an ACK from already existing avatars to mark their\n\
	  positions when a new avatar come into the arena\n\
	- don't transmit user and ball deplacements in Z delta < 2cm\n\
	- don't transmit ball angular deplacements to don't agress the network\n\
	- add rtp.h from RFC 1889 (RTP/RTCP)\n\
	- use random.c and md5c.c from RFC 1889 to compute SSRC\n\
	- adapt initNetwork() to initialize RTCP sockets\n\
	- adapt incoming() to manage RTCP packets\n\
	- ObjectName struct with ssrcid member (RTP)\n\
	- ObjectHeader struct with a pointer on source state info (RTCP)\n\
	(1998/03/18, Philippe Dax)\n\
\n\
-- 1.3.0\n\
	- button Home\n\
	- action shoot a ball, w_mgt_ball.c\n\
	- go automaticaly into a new world when user intersects a gate\n\
	- webspace vrml browser, vrml.[hc]\n\
	- vrweb vrml browser, vrml.[hc]\n\
	- object vrml, w_mgt_vrml.[hc]\n\
	- random initial position for users and balls\n\
	- ball created from config file -> [ don't work now ]\n\
	- delete sender IP-addr/port from payload, use recvfrom() vs recv()\n\
	- new sdr_plugin RTP: sdr2.plugin.S61.dis.rtp.vr.vreng\n\
	- libXaw3d in configure.in\n\
	(1998/03/12, Philippe Dax)\n\
\n\
== 1.2\n\
-- 1.2.9 <-- obsoleted\n\
	- when is active, keep audio tool active through worlds\n\
 	- no_proxy variable\n\
	(1998/02/12, Philippe Dax)\n\
\n\
-- 1.2.8\n\
 	- button members\n\
	- RTPnames\n\
	(1997/12/19, Philippe Dax)\n\
\n\
-- 1.2.7\n\
 	- bug fix en configure.in\n\
	- bug fix if proxy variable empty\n\
	- mainloop, rescan events\n\
	(1997/12/16, Samuel Tardieu)\n\
\n\
-- 1.2.6\n\
	- horizontal menubar\n\
	- popup menus to choose or activate tools\n\
	(1997/12/16, Philippe Dax)\n\
\n\
-- 1.2.5\n\
	- bug fix while checking X libraries, configure.in\n\
	- button preferences\n\
	(1997/12/12, Philippe Dax)\n\
\n\
-- 1.2.4\n\
	- target install_archive_bin, vreng/Makefile.am\n\
	- bug fix while checking X libraries, configure.in\n\
	- add proxy access: env. variable http://proxy:3128/, gui/http.c\n\
	- discard unknown objects & their descriptions in *.cfg, w_mgt_file.c\n\
	- option -d level\n\
	(1997/12/08, Philippe Dax)\n\
\n\
-- 1.2.3\n\
	- new object: web, w_mgt_web.[hc]\n\
	- netscape and mMosaic browsers, browser.[hc]\n\
	- add methods: netscape, mMosaic, mailto\n\
	(1997/12/02, Philippe Dax)\n\
\n\
-- 1.2.2\n\
	- bug fix on match Visual for color depth=24\n\
	(1997/11/19, Philippe Dax)\n\
\n\
-- 1.2.1\n\
	- match Visual for color depth=8\n\
	(1997/11/18, Philippe Dax)\n\
\n\
-- 1.2.0\n\
	- new packaging ala GNU\n\
	- automake, autoconf, configure\n\
	(1997/11/10, Philippe Dax)\n\
\n\
== 1.1\n\
-- 1.1.3 <-- obsoleted\n\
	- port on Linux Pentium\n\
	- port on Linux Sparc\n\
	- port on NetBSD-1.2 Sparc\n\
	(1997/11/08, Philippe Dax)\n\
\n\
-- 1.1.2\n\
	- audio button, gui/int_supportFuncs.c\n\
	- video button, gui/int_supportFuncs.c\n\
	- vic support, mctools/video.[hc]\n\
	- wb support, mctools/wb.[hc]\n\
	- new object: board, w_mgt_board.[hc]\n\
	(1997/11/07, Philippe Dax)\n\
\n\
-- 1.1.1\n\
	- enable # comments in world configuation files, w_mgt_files.c\n\
	- initial world configuration builtin, w_mgt/w_cfg.h\n\
	- file TODO\n\
	- file CHANGES\n\
	(1997/11/03, Philippe Dax)\n\
\n\
-- 1.1.0\n\
	- add external audio support with fphone, mctools/audio.[hc]\n\
	- new object: host, w_mgt/w_mgt_host.[hc]\n\
	(1997/10/30, Philippe Dax)\n\
\n\
== 1.0\n\
-- 1.0.1\n\
	- bug fix about GET HTTP/1.0, gui/int_supportFuncs.c\n\
	(1997/10/22, Philippe Dax)\n\
\n\
-- 1.0.0\n\
	- First public release\n\
	- repackaging\n\
	- inherit of initial TTL for the other worlds, mctools/mc.[hc]\n\
	- add external audio support with vat, mctools/audio.[hc]\n\
	(1997/10/16, Philippe Dax)\n\
\n\
++ 0\n\
== 0.4\n\
-- 0.4.6 <-- obsoleted\n\
	- original version\n\
	- (May 1997)\n\
\n\
== 0.1\n\
-- 0.0.1\n\
	- first version\n\
	- (Nov 1996)\n\
\n\
";
