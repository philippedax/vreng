Last changes in VREng
---------------------

------------------------------------------
The rules for this file:
* entries are sorted newest-first.
*
* ++ : major
* == : medium
* -- : minor
* Version: major.medium.minor
* date and author format: (YYYY/MM/DD, Author).
------------------------------------------

++ 9
== 9.4
-- 9.4.5
	(2024/04/03, Philippe Dax)

-- 9.4.4
	- fix CXXFLAGS configure.ac and */Makefile.am
	- add -B option to show build infos
	- fix throw exceptions in ubit/*pp
	(2024/03/28, Philippe Dax)

-- 9.4.3
	- fix AM_CPPFLAGS for freetype2 src/*/Makefile.am
	- fix -lptread by reordering CHECK-LIBs configure.ac
	(2023/12/24, Philippe Dax)
	- rename model/face*pp to model/v3d*pp
	- rename world/humanhead*pp to world/face*pp
	(2023/12/16, Philippe Dax)
	- rename support to config
	- rename htdocs to data
	- rename src/bundle ro src/ext
	- rename src/vreng ro src/base
	- rename src/wo ro src/world
	(2023/09/17, Philippe Dax)

-- 9.4.2
	- add object spot (#88)
	(2023/07/22, Philippe Dax)
	- add spot to movie
	(2023/07/20, Philippe Dax)
	- add -Wno-macro-redefined configure.ac
	(2023/05/08, Philippe Dax)

-- 9.4.1
	- clean #if VRSQL ... #endif
	(2023/05/05, Philippe Dax)
	- fix avatar as humanoid in humanoid.cpp
	(2023/05/02, Philippe Dax)
	- fix vertical view in drone.cpp
	(2023/04/30, Philippe Dax)
	- fix bufferSelection for hinted objects
	(2023/04/27, Philippe Dax)
	- fix close write cache in world.cpp
	- fix jpeg capture
	(2023/04/07, Philippe Dax)
	- fix warning deprecated in Java
	(2023/02/14, Philippe Dax)
	- drone flying, user following drone
	(2023/03/13, Philippe Dax)
	- make rules to build types.hpp
	(2023/02/15, Philippe Dax)

-- 9.4.0
	- moved VERSION to VERSION.md avoid conflicts with #include <version>
	- files md moved into md directory
	- build script replace autogen.sh
	- renamed object head to humanhead
	- added option -K to fill the cache
	- fix cache when an http-err occurs
	(2023/01/05, Philippe Dax)

== 9.3
-- 9.3.3
	- removed prof*pp see stat*pp
	- added button view stats in the GUI
	(2022/12/12, Philippe Dax)
	- fix bug "too many opened files" in cache.cpp
	- replace select() by poll() in udispX11.cpp
	(2022/12/08, Philippe Dax)
	- compute urlpfx, no more hardcoded string "dax"
	(2022/11/30, Philippe Dax)
	- fixed sendKeyEvent in vnccli.cpp
	(2022/11/13, Philippe Dax)
	- added object #87 : fog
	(2022/10/26, Philippe Dax)
	- moved gui/capture.*pp to 2d/
	- show only input text in message panel
	(2022/10/07, Philippe Dax)
	- basic motions of body
	(2022/04/07, Philippe Dax)

-- 9.3.2
	- removed bundle/mypng bundle/zlib
	(2021/11/24, Philippe Dax)
	- fixed returned size read by httpRead
	(2021/11/21, Philippe Dax)
	- fixed writing binary files into the cache in cache.cpp
	(2021/11/17, Philippe Dax)
	- renamed android to humanoid object
	(2021/11/15, Philippe Dax)

-- 9.3.1
	- fixed bug http bad request in url.cpp
	(2021/11/13, Philippe Dax)
	- added new object #86 : drone
	(2021/08/18, Philippe Dax)
	- added new object #85 : bird
	(2021/08/08, Philippe Dax)

-- 9.3.0
	- moved servers/* into src/
	- moved util/* into src/
	- renamed vgl/ to 3d/
	- renamed texture/ to 2d/
	- renamed app/ to tool/
	- moved fonts into src/ubit
	(2021/06/04, Philippe Dax)

== 9.2
-- 9.2.0
	- added new object #84 : human
	(2021/05/24, Philippe Dax)
	- rewrited model/bone* to bone.cpp and bone.hpp"
	- fixed rendering guide both specific and normal
	- added compFrame to sort solids by increasing order of frames
	(2021/04/13, Philippe Dax)
	- fixed picking in thr selection buffer before display
	(2021/03/29, Philippe Dax)
	- fixed errno 22 on select in udispX11.cpp (bad maxfd)
	(2021/03/27, Philippe Dax)
	- added opaque list sorted decreasingly by surface sizes
	- added symlink to htdocs if universe is local
	(2021/03/18, Philippe Dax)
	- fixed delete bubble text
	- fixed text.cpp and guy.cpp : call maleSolid
	- added methods isRendered() & setRendered()
	(2021/03/05, Philippe Dax)
	- added translucid list to render decreasingly distance from eyes
	(2021/03/03, Philippe Dax)
	- removed old lists
	(2021/02/24, Philippe Dax)

== 9.1
-- 9.1.6
	- fixed toDelete call to deleteFromGrid()
	(2021/02/21, Philippe Dax)
	- added sqlite operations in vrsql*pp
	- added support for sqlite in configure.ac
	- added fictif solid to play all solids
	(2021/02/10, Philippe Dax)
	- updated defaults to run local autonomously
	- replace glui-2.01f by glui-2.37
	- fixed loop state in movie.cpp
	- added getDlist() in solid*pp for board.cpp
	- reviewed vred
	(2021/01/18, Philippe Dax)

-- 9.1.5
	- fixed type converson in aiinit.cpp
	- invalided xml*pp xml2 includes not found
	- added setrlimit to increase no of open files
	- integrated src/ex/vrl to src/util/vrl
	- fixed addobj in widgets.cpp (compound solids)
	- fixed parseSolids to parse multi-solids
	- reviewed util/vred
	- reviewed compilation warnings bundle/glui
	(2021/01/06, Philippe Dax)

-- 9.1.4
	- reviewed timer.*pp
	- fixed needles motion in clock.cpp
	- reviewed vrel and vred
	- fixed numline in parse.cpp
	(2020/12/14, Philippe Dax)

-- 9.1.3
	- renamed vre.hpp to sandbox.hpp
	- using stl lists for netobj.*pp
	- rewriting some algos for rendering
	(2020/12/02, Philippe Dax)

-- 9.1.2
	- fixed -w option in pref.cpp
	- added objectList in wobject.*pp
	- added FLUID mode and fluidLists
	- renamed initialize*Object to init*Object methods
	- renamed class PutInfo to class Message2
	- renamed class Pitch to class Joystick2
	- removed gui/pitch*pp
	(2020/11/21, Philippe Dax)

-- 9.1.1
	- fixed urlpfx in universe.cpp
	- reviewed some rendering order
	- removed code creation from vrelines for rain and snow
	- fixed guy distance to ground
	- added parseBool() methods in parse.*pp
	(2020/11/12, Philippe Dax)

-- 9.1.0
	- invalidated app ring in vjs/
	- restored src/servers/vjs/gen.pl*
	- added door open/close mecanisms : sliding and vertical
	- added fractal element to dtd
	- added command line options -k (keepcache) and -S (nostats)
	(2020/11/03, Philippe Dax)

== 9.0
-- 9.0.3
	- move gui/mvt.*pp to gui/motion.*pp
	- removed vgl/hud.*pp
	- restored missing file in src/ai for github
	(2020/10/19, Philippe Dax)

-- 9.0.2
	- fixed angle when toland action
	- restablished gui/hud*pp
	(2020/10/17, Philippe Dax)

-- 9.0.1
	- added user actions: flyaway and toland
        - fixed some memory leaks
	- renamed WO_ACTION to O_ACTION
	- move universe.*pp into src/vreng
	- move world.*pp into src/vreng
	- move xml.*pp into src/vreng
	- move openal.*pp into src/vreng
	- move useraction.hpp into user.hpp
	- move vncdialog.cpp into widgets.cpp
	- move vncdialog.hpp into widgets.hpp
	- move hud.cpp into widgets.cpp
	- move hud.hpp into widgets.hpp
	(2020/10/15, Philippe Dax)

-- 9.0.0
        - fix connection to localhost in pref.cpp http.cpp universe.cpp
        - avoid to resolve localhost in http.cpp
        - --with-mysql=no by default
	- move camera.cpp into render.cpp
        - move gui/scene.*pp to scr/vreng
	- move vr2gl.cpp to wo/
	- move addobj.cpp into widgets.cpp
	- move keys.cpp into widgets.cpp
	- move filemenu.cpp into widgets.cpp
	- move dialog.cpp into widgets.cpp
	- move display.cpp into solid.cpp
        - changed ray color from red to white stippled
	(2020/10/03, Philippe Dax)

++ 8
== 8.7
-- 8.7.3
	- configure change openal by freealut
	- changed fire parameters in fire.hpp
	(2018/10/22, Philippe Dax)

-- 8.7.2
	- added fonts directory to git
	- added texture/glpng{ch}pp to git
	- moved githup to .github
	(2018/10/21, Philippe Dax)

-- 8.7.1
	- alternative for missing fonts, use local fonts in uconf.cpp
	- alternative if Xmu not found in udispX11.cpp
	(2018/10/18, Philippe Dax)

-- 8.7.0
	- find Xmu and Xpm headers needed by src/ubit
	- added missing ttf fonts in directory fonts/
	(2018/10/18, Philippe Dax)

== 8.6
-- 8.6.0
	- find dynamycaly presence of ttf freetype fonts when ./configure is running
	- move bundle/glpng to texture/
	- move bundle/png to bundle/mypng and phg.h to mypng.h
	(2018/10/08, Philippe Dax)

== 8.5
-- 8.5.7
	- fixed some compilation warnings
	- fixed X11 fonts path in ubit/uconf.cpp to find freetype fonts
	(2018/09/17, Philippe Dax)

-- 8.5.6
	- fixed some warnings with g++-8.1.1
	(2018/02/10, Philippe Dax)

-- 8.5.5
	- fixed CXFLAG without -fpermissive
	- fixed conficts with glpng
	(2018/02/10, Philippe Dax)
	- fixed buffer overflows
	- fixed dynamic exception specification (C++11) by adding -Wno-deprecated to CXXFLAGS
	(2017/12/10, Philippe Dax)

-- 8.5.4
	- added script start-daemons
	- fixed fratal.*pp
	(2017/06/23, Philippe Dax)

-- 8.5.3
	- added object fractal #83
	(2017/05/04, Philippe Dax)

-- 8.5.2
	- fixed vac.cpp to work without a vacs server
	- added timeout to connect to different servers
	(2017/05/03, Philippe Dax)

-- 8.5.1
	- fixed &png_ptr in bundle/png.c
	- fixed #include "png.h" in texture/png.cpp
	- changed --with-png-builtin yes by default
	(2017/04/28, Philippe Dax)

-- 8.5.0
	- added etc directory at top
	(2017/04/11, Philippe Dax)

== 8.4
-- 8.4.0
	- removed -Wno-unknown-warning
	(2016/09/28, Philippe Dax)

== 8.3
-- 8.3.3
	- added install vjs
	- added install vrum
	(2016/09/25, Philippe Dax)

-- 8.3.2
	- added -Wno-unknown-warnnig option
	(2016/09/22, Philippe Dax)

-- 8.3.1
	- vreng project hosted at github.com
	- changed CXXFLAGS to avoid warnings under gcc-6.1.1
	- added view.xpm into gui/xpm/Makefile.am
	- moved acinclude.m4 into support/
	(2016/08/07, Philippe Dax)

-- 8.3.0
	- added checklib libsndfile into configure.ac
	- added openal support --with-openal into configure.ac
	(2016/03/26, Philippe Dax)

== 8.2
-- 8.2.2
	- renamed missile to projectile
	- added view third person button in panels.cpp
	- added go button in xidgets.cpp dialog.cpp
	- cleans up bone files
	- fixed endianess in hairs.cpp
	- fixed regression on flashy objects
	- rewriting Render::solids
	- fixed sun rendering
	- added bindSocket in socket.cpp
	- added trace2 without cr in log.cpp
	- added stringcmp in str.cpp
	(2016/01/20, Philippe Dax)

-- 8.2.1
	- fixed regression of ubit/uconf.cpp
	(2015/12/29, Philippe Dax)

-- 8.2.0
	- updated ubit to be in conformity with the clang apple compiler
	- updated check libmysqlclient for Linux
	(2015/10/26, Philippe Dax)

== 8.1
-- 8.1.0
	- added tar.xz distribution
	- added vmm branch
	(2013/10/15, Philippe Dax)

== 8.0
-- 8.0.0
	- added bsphere bounding sphere
	- fixed draw bbox
	- fixed view source using cache
	- added object firework
	- added src/vreng/vector3.hpp
	- deleted src/wo/grid.cpp
	- deleted src/wo/netw.cpp
	- moved src/wo/list.{cpp,hpp} to olist.{cpp,hpp}
	- updated vreng data version to v8
	(2012/09/03, Philippe Dax)

=======
++ 7
== 7.4
-- 7.4.0
	- added src/sys
	- rewriting src/app/* with classes
	- updated display.cpp displayMirrored without pushing names in the buffer
	- fixed xpm.cpp
	- updated src/ubit/uconf.cpp for Type1 fonts under Linux
	- moved src/ubit/nat/*pp to src/ubit
	- moved src/vrel src/vred src/tool to src/util
	- moved src/bone/*pp to src/model
	- updated conf/vreng.spec to build rpm package
	- fixed bin/Makefile.am for vred and vrel
	(2012/07/08, Philippe Dax)

== 7.3
-- 7.3.3
	- moved src/net/md5* src/net/random.cpp to src/vreng/
	- resolved opens/closes leak
	- updated textureList to STL
	- moved src/net/nsl* src/net/socket* to src/vreng/
	- updated file.{ch}pp : added openFile() and closeFile()
	- updated fileio.{ch}pp to file.{ch}pp
	- updated socket.cpp : closeDatagram(), closeStream()
	- updated prof.cpp : added statIO()
	(2012/06/06, Philippe Dax)

-- 7.3.2
	- updated mobileList, stillList, txfList to STL
	- updated txf to be cached
	- added net/noid.{ch}pp comeback
	- updated netobjectList, channelList to STL
	- updated cartList, wearList, lightList, deleteList, invisibleList to STL
	- updated oclass from wclass
	- updated lwo to class
	- updated smoke
	(2012/06/03, Philippe Dax)

-- 7.3.1
	- all remote data are now cached (off, lwo, dxf, cset, book, bodyparts, walls)
	- added vreng/cache.{ch}pp
	- added vreng/reader.{ch}pp
	- added option -r,--refresh
	- added tools obj2parts, jpobj.c, jpoff.c
	- added model/bap.t
	- updated android, body, bap
	- added src/tool
	- added sound.{ch}pp
	(2012/04/25, Philippe Dax)

-- 7.3.0
	- updated vreng-data to version 7.3
	- updated paralellipedic sizes
	(2012/02/24, Philippe Dax)

== 7.2
-- 7.2.8
	- added object smoke
	- moved anim.{ch}pp to animator.{ch}pp
	- added vred branch
	- added vrel branch
	(2012/02/21, Philippe Dax)

-- 7.2.7
	- moved Attractor to Slider
	- moved Repulsor to Bumper
	- moved Follower to Accessory
	(2012/01/29, Philippe Dax)

-- 7.2.6
	- updated object ball from file
	- added object follower
	- added draw bbox
	- added object repulsor
	- added object attractor
	- added dtd menu
	- updated rendering order
	- added model/bvh.{ch}pp
	(2012/01/23, Philippe Dax)

-- 7.2.5
	- added mode roll and tilt to mirage
	- added bubble solid translucid obloid
	- updated plane multi bounding boxes
	- added legend attribute to web
	- added pivot action to web
	- added AVI video reader/writer
	- added PSD image loader
	(2011/12/24, Philippe Dax)

-- 7.2.4
	- moved mirror algos to display.cpp
	- added model helicopter to wings
	- updated user and head, head follows avatar over the worlds
	- renamed bone/ vnc/
	- moved wo/{body,face} to model/
	- updated url : saveToCacheAndOpen
	- updated hairs
	(2011/11/08, Philippe Dax)

-- 7.2.3
	- added prefs sex, head, skin, bust
	(2011/09/20, Philippe Dax)

-- 7.2.2
	- added option -t, --trace
	- added global localuser
	(2011/09/14, Philippe Dax)

-- 7.2.1
	- added object fire
	- added models butterfly, libellule, angel to wings
	- added models spires, bands, roses to dress
	- added models tophat, bowler, chinese, cardinal, country, clown to hat
	- added female to guy
	- added mysql to cloth
	- added colors to guy
	(2011/08/14, Philippe Dax)

-- 7.2.0
	- new layout
	- added bin and man directories
	- updated 3rdpart to extras
	- moved com to vreng
	(2011/07/30, Philippe Dax)

== 7.1
-- 7.1.0
	- reduced dependencies (ubit, png, zlib)
	- added builtin ubit
	- added builtin png
	- added builtin zlib
	(2011/07/28, Philippe Dax)

== 7.0
-- 7.0.2
	- added src/ubit
	- added src/3rdpart/ftgl
	(2011/07/19, Philippe Dax)

-- 7.0.1
	- fixed some g++ warnings
	- moved config/ to support/
	(2011/07/18, Philippe Dax, comeback after 2 years !)

-- 7.0.0
	- added flying bevavior for user
	- added command line option -a,--avatar to select an avatar model (man, guy)
	- added new user when enter into a new world
	- renamed spark to thunder
	- added wings object
	- updated user : can manage avatar model preferences
	- updated guy, now can be the avatar model by default
	- added flare-shine rendering
	- added teapot shape
	- added dress object
	- added hat object
	- added halo object
	- added cloth object
	- added sun object
	- added river object
	- added terrain object
	- added bubble object
	- added snow object
	- added rain object
	- added fountain object
	- added waterfall object
	- added spiral object
	- added travelator object
	- added escalator object
	- added stair object
	- added cloud object
	- fixed stars area
	- added widget object
	- added spark object
	- added pthread for wheel progression in universe.cpp
	- added wheel and needle rendering in solid.cpp
	- added wheel and needle drawings in draw.cpp
	- added savePNG in png.cpp
	- added saveTGA in tga.cpp
	- added curl support in url.cpp
	- fixed read of vacs cache
	- updated configure.ac to check linux /usr/lib64 and solaris /usr/sfw
	- added --enable-debug into configure.ac
	- added wind object
	- updated configure.ac for macports
	- renamed vnp.* to off.* (Object File Format)
	- added sub-directories src/texture src/model
	- renamed *.hh to *.hpp
	- fixed circular motion in mirage.cpp
	- added Xbm image loader
	- added Xpm image loader
	- added dds image loader
	- added tiff image loader
	- added class for format
	- added timer.{cpp,hh} prof.{cpp,hh} socket.hh event.hh sap.hh
	- moved stat.{cpp,hh} http.{cpp,hh} url.{cpp,hh} md5.{cpp,hh}
	- renamed file.{cpp.hh} -> fileio.{cpp.hh} net.hh -> netobj.cpp
	- updated video capture to produce directly a flash file (.swf)
	- fixed lightings in hud.cpp
	- added methods (set|reset)Flashy into animx3d.cpp
	- fixed grid.cpp expecting the end of GUI initialization
	- updated hud and xwin to use GLUT fonts (glutBitmapCharacter())
	- added --with-glut into configure.ac
	- added --with-sdl into configure.ac
	(2009/05/10, Philippe Dax)
	- updated gui/* reorganization and rewritting without X11 dependencies
	(2008/07/19 - Eric Lecolinet)

++ 6
== 6.9
-- 6.9.3
	- updated GUI to be compliant with ubit6
	- fixed movie.cpp include mpeg.h
	- fixed link edit against OpenGL under MacOSX-10.5 (Leopard) 
	(2007/12/29, Philippe Dax)

-- 6.9.2
	- fixed mini-map rotation
	(2007/10/16, Eric Varadaradjou)

	- added mpeg_lib in 3rdparty if not available on the target system
	- fixed pitch for other views
	- fixed view of bubble above the head
	- fixed click in the background
	(2007/10/02, Philippe Dax)

-- 6.9.1
	- fixed bad references to 3party in som Makefile.am
	- fixed compilation warning under g++-4.2.0
	- fixed bad usage of GL_RGB in movie.cpp
	- fixed small bug in Makefile.am
	(2007/09/18, Philippe Dax)

-- 6.9.0
	- renamed x3dobject to animx3d
	- renamed 3party to 3rdpart
	- fixed DXF parsing
	(2007/07/11, Philippe Dax)

	- added x3d loader-parser, x3d aanimation rendering
	- added x3dobject to play animations described by x3d format
	(2007/06/28, Pascal Chambon)

	- added support for dxf format (autocad)
	- fixed static memory of name="basket" to dynamic memory under Linux
	- fixed specific parsing to avoid infinite loop
	- fixed cleanCacheByExt to avoid deleting *.vre3d
	- fixed glpng/Makefile.am using conditional test MYLIBPNG
	- added textures for body for obj format
	- revisted body and android (joint-points file xmlized)
	- revisted body and bap
	- revisted models Md2, Obj, Man
	(2007/06/18, Philippe Dax)

== 6.8
-- 6.8.12
	- fixed getProperty, don't call isValid()
	- fixed glPushName((GLuint) (long)addr & 0xffffffff) (64 bits arch)
	- fixed mrelease, don't call openedMenu->close(0): segfault 64 bits arch
	- fixed createSendSocket test == false >= right to call setSendSocket
	(2007/03/23, Philippe Dax)

-- 6.8.11
	- fixed next sdes item in rtp.cpp
	- fixed Source constructor next=NULL in source.cpp
	- fixed no loopback in socket.cpp
	- fided compilation warning in navig.cpp
	(2007/03/22, Philippe Dax)

-- 6.8.10
	- fixed CXXFLAGS in configure.ac
	- fixed segfault in dart.cpp and bullet.cpp
	- added file.{cpp,hh}
	- added md2 into model.cpp
	- revisted md2.{cpp,hh}
	(2007/03/16, Philippe Dax)

-- 6.8.9
	- fixed com/Makefile.am env.cc -> pref.cpp
	- fixed some extra qualification detected by g++-4.1.2 abort compilation
	(2007/02/23, Philippe Dax)

-- 6.8.8
	- fixed AC_CONFIG_SRCDIR vreng.cc -> vreng.cpp
	(2007/02/18, Philippe Dax)

-- 6.8.7
	- fixed obj.cpp avoid negative vertice index
	- added gui.hpp from guiImpl.hpp and gui.hh which are removed
	- renamed guiclass.* to xclass.*
	- removed guimisc.cc now in vreng.cc
	- added putinfo.{cpp,hpp} from gui.cpp
	- added mvt.{cpp,hpp} from navig.cpp
	- added xerr.{cpp,hpp} from xwin.cpp
	- added double-click handling (default action id=0)
	- fixed parsing bugs introduced in 6.8.4
	- renamed all file .cc to .cpp
	- renamed counter.* to hud.*
	(2007/02/18, Philippe Dax)

-- 6.8.6
	- splited landmark.{cc,hh} into 6 files: (axis,grid,counter}.{cc,hh}
	- renamed options.{cc,hh} to pref.{cc,hh}
	- rewrited env.{cc,hh}, class Env added
	- updated http.cc with getShort, getLong and getGLfloat
	- updated configure.ac to be compliant with pkgsrc
	- updated joint points data (man.jp) incompatible with previous releases
	- updated VNP structure to VNP class
	(2007/02/08, Philippe Dax)

-- 6.8.5
	- added bap.{cc,hh} and fap.hh
	- updated man.cc (vertices, normals, texcoords)
	- updated android to load VNP models with scale=1
	- fixed include #include <map> in vicinity.cc with ubit-5.5.0
	- added class Body in body.{cc,hh}
	- fixed wrong polygons drawing for VNP models in vnp.cc
	- added body.{cc,hh} to handle independantly human body joint points
	(2007/01/22, Philippe Dax)

-- 6.8.4
	- revised DTD, NMTOKEN(S) added
	- fixed a bug in color.cc when the colorname does not exist
	- added mode="travelator" in step.cc
	- added mode="escalator" up and down in step.cc
	- added mode="stairs" to build directly stairscase in step.cc
	- added for for the whole scene in light.cc
	- added fog for each object
	- added Parse class to parse objects and solids
	- added handling of http redirects like '301 - Move permanently'
	- added soft rotation when the direction of the guide changes
	- added mode="one-way" attribute for the guide's path (no loop)
	- added color attribute for the guide's path
	- added temporisation delay at each vertice of the guide's path
	- added actions to the guide : show/hide, pause/continue, reset
	- added drawing with stipple lines the path of the guide
	- fixed user takes the same orientation as the guide along the path
	- fixed orientation of the guide along the path
	(2007/01/08, Philippe Dax)

-- 6.8.3
	- added DTD into conf/vre-1.0.dtd
	- updated configure.ac and Makefile.am for distribs tar.bz2 and zip
	- fixed sgi loader (RBGA to RGB)
	- fixed infinite loop in col.cc when intersection occurs with guide
	- fixed drawing in vrelet.cc
	- fixed lightings in light.cc, text.cc and render.cc
	- fixed position and orientation of flag
	- updated vreng icon for Window-manager
	(2006/11/27, Philippe Dax)

-- 6.8.2
	- updated theme.{cc,hh} to be compliant with new ubit-5.4.1 API
	- updated 3ds and ase models drawing in display lists
	- updated gl2ps from 1.2.7 to 1.3.2
	(2006/11/14, Philippe Dax)

-- 6.8.1
	- fixed segfaults in generalIntersect()
	- added collision detection of oblique objects
	- added alternative skins (background colors) to GUI handled by theme.cc
	- added bubble text (welcome and chat) above the avatar's head
	- added Open and Save buttons into File menu
	- added reverse video on buttons
	- added OBJ loader for Wavefront models
	- added target svntags into Makefile.am for maintainer
	(2006/11/09, Philippe Dax)

-- 6.8.0
	- updated Cart to update geometry when the object is dropped
	- added compound solids (chair, table) into palette.cc
	- added sandbox button and callback
	- added builtin sandbox world
	- moved Manip to Move, Goto to Approach
	- added more MySql methods to handle columns
	- updated structures of MySql tables
	- added solid strings now handled by MySql, getGeom, updateGeom
	- fixed manipulations of things in cart.cc
	- added dymamic thing objects are now persistent
	(2006/09/04, Philippe Dax)

== 6.7
-- 6.7.4
	- added audio loaders : wav.cc, au.cc
	- added audio player : vlc, play
	- added sound action and update music.cc consequently
	- removed obsolete applications : slidecast and vrbrowser
	- updated screen of vnc
	- added push object when user collide it
	- fixed free memory in movie.cc
	(2006/07/21, Philippe Dax)

-- 6.7.3
	- fixed model's rotations
	- added persistency for models
	- removed friend related to carrier class
	- added owner for dynamic objects, method removeFromScene()
	- added basic textures and alpha values into the palette
	(2006/07/05, Philippe Dax)
	- added palette to dynamicaly insert objects into the scene (palette.cc)
	(2006/06/28, Basile Anglaret)

-- 6.7.2
	- fixed counter and avatar rendering under Ubit-5.0.2
	(2006/06/21, Eric Lecolinet)
	- added initializMobileObject(lasting)
	(2006/06/21, Philippe Dax)
	- fixed warnings given by autoscan and aclocal
	- added showSolidList()
	(2006/06/21, Eric Varadaradjou)

-- 6.7.1
	- updated Ubit interface
	- fixed resizing GLzone
	(2006/06/20, Eric Lecolinet)
	- updated model with pos="..."
	- added makeSolid()
	(2006/06/19, Philippe Dax)

-- 6.7.0
	- fixed an huge bug about setlocale LC_NUMERIC, now take LC_ALL="C"
	(2006/06/14, Philippe Dax)

== 6.6
-- 6.6.3
	- check ubit version and compare it with required ubit version
	- fixed minor bugs in gui/
	(2006/06/13, Philippe Dax)
	- added history of requests and navigation in history
	(2006/06/12, Eric Varadaradjou)

-- 6.6.2
	- fixed enableBehavior isVisible(false)
	- fixed rezising: update glViewport in render.cc
	- removed all NAVIG actions
	(2006/06/08, Philippe Dax)

-- 6.6.1
	- updated 16 to 24 color bits
	- updated button to select actions by name and no more by index
	- added macro AC_CHECK_FREETYPE in acinclude.m4
	- added check of freetype library in configure.ac
	(2006/06/06, Philippe Dax)

-- 6.6.0
	- reorganization of initobj.{cc,hh}nd general tables of functions
	- fixed the disparition of object bar
	- fixed bug in deleteObject when an object is destroyed in the scene
	(2006/06/05, Philippe Dax)

== 6.5
-- 6.5.13
	- added code in prevision of UBIT5
	- added gui/baz.{cc,hh}
	- fixed addlist in the GUI
	- fixed button action name
	(2006/06/01, Philippe Dax)
	- added gui/vicinity.{cc,hh}
	- added map view with its button
	(2006/05/24, Eric Varadaradjou)

-- 6.5.12
	- fixed adding into avatar and world Gui lists
	(2006/05/09, Eric Varadaradjou)
	- added screenshot output in SVG format
	- updated gl2ps with SVG capability
	- removed --with-ubit from configure.ac
	- added xpm.hh in src/gui/xpm
	(2006/05/09, Philippe Dax)

-- 6.5.11
	- fixed ax angle in book.cc when approaching
	- fixed scrolling in worldpane (Eric Lecolinet)
	- added callback to enter into worlds from an url string
	- added toggle buttons into the panel for axis/grid/overlap/counter
	- fixed bugs: configure.ac, gui/Makefile.am, compilation without ocaml
	(2006/05/04, Philippe Dax)

-- 6.5.10
	- split ubwidgets.cc into many smaller files
	- add car.{cc,hh} taken from draw.cc
	- human.hh becomes man.{cc.hh}
	- add ground level view, update scissor view
	(2006/04/13, Philippe Dax)
	- fix vertical scrolling
	- update Gui to be compliant with the current Ubit
	(2006/03/27, Eric Lecolinet)
	- fix bad WIDTH3D
	(2006/03/19, Philippe Dax)
	- code for what objects are viewed
	(2006/02/27, Eric Varadaradjou)

-- 6.5.9
	- laser selects objects rather than solids
	- solid class rewritten
	(2006/01/13, Damien Bobillot)

-- 6.5.8
	- Ubit interface updated
	(2006/01/05, Eric Lecolinet)
	- added mode="circular" in mirage.cc
	(2006/01/03, Eric Varadaradjou)

-- 6.5.7
	- bug fixed worldname string not null terminated (introduced in 6.5.5)
	- added perpetual movement for guide object: mode="perpetual"
	(2005/12/29, Philippe Dax)
	- don't update MysSql if object has not moved
	- world.cc ubit independant: move sourceDialog to ubwidgets.cc
	(2005/12/18, Philippe Dax)
	- Render class rewritten
	(2005/12/08, Damien Bobillot)

-- 6.5.6
	- changes in gui/ to be compliant with new ubit API
	(2005/11/30, Philippe Dax)
	- allow buffer 3D selection according to a depth level
	(2005/11/29, Eric Varadaradjou)

-- 6.5.5
	- bug fixed in bone/boneAnimator.cc
	- bug fixed about analyseur.ml in ocaml/Makefile.am
	- rtpsess.cc splited into rtp.cc, session.cc, source.cc
	- added ellipse shape
	(2005/11/21, Philippe Dax)
	- rewriting World class
	- merge debug.hh > log.hh
	(2005/11/16, Damien Bobillot)
	- changed request input field
	(2005/11/07, Eric Varadaradjou)
	- added plane object
	- added ground object
	- added missile object
	- added color.cc, handle color names
	(2005/11/01, Philippe Dax)

-- 6.5.4
	- laser published throught the network
	(2005/10/28, Philippe Dax)
	- ocaml designation mecanism
	(2005/10/27, Eric Varadaradjou)

-- 6.5.3
	- fix a compilation bug with ocaml (if HAVE_OCAML)
	- fix user names exchanged between participants
	- fix getRtcp* returning string
	- update cart.cc, icon.cc
	(2005/10/24, Philippe Dax)
	- ocaml updates
	(2005/10/19, Eric Varadaradjou)

-- 6.5.2
	- updates in ocaml/
	(2005/10/13, Eric Varadaradjou)
	- fix stupid compilation bug in col.cc
	(2005/10/12, Philippe Dax)

-- 6.5.1
	- compliance with gcc-4.0
	(2005/10/11, Damien Bobillot)
	- fix compilation error read_request not defined, use HAVE_OCAML
	(2005/10/08, Philippe Dax)

-- 6.5.0
	- collisions for multi-solids
	- Navigator prefered to XNavigator 
	- goto action : 2 compound movements
	- add renderList for specific rendering
	- code of mirror from render.cc now in mirror.cc
	- cleanup 6.4.10
	(2005/10/02, Philippe Dax)

== 6.4
-- 6.4.10
	- goto deplacement is multithreaded and moves without colliding
	- request input on the same panel as chat input
	- gui background color is black, foreground is lightblue
	(2005/07/31, Philippe Dax)
	- add ocaml parsers: lexical, syntaxic, semantic,...
	(2005/06/30, Eric Varadaradjou)
	- re-add ocaml, adapt vreng/Makefile.am
	(2005/06/30, Philippe Dax)

-- 6.4.9
	- remove ocaml from distribution
	(2005/06/16, Philippe Dax)

-- 6.4.8
	- fix mirror
	(2005/06/15, Eric Varadaradjou)
	- co-existence new-look and old-look : option -l,--look old|new
	(2005/06/15, Philippe Dax)
	- gui new-look
	- navigation new-look
	- update theme
	- new xpm files
	(2005/06/07, Mael Leboite)

-- 6.4.7
	- add conditional compilation for 3party/gl2ps
	- new object: area #49 (invisible)
	- call QuickTime under MacOSX when playing Mpeg
	(2005/06/02, Philippe Dax)
	- link against libasmrun.a is ok
	- new subdir src/ocaml
	- first steps with ocaml
	(2005/05/18, Philippe Dax)

-- 6.4.6
	- human avatar representing real user (human.hh, solid.cc, draw.cc)
	- sources managed by subversion-1.2.0-rc1
	- <!DOCTYPE ...> dtd line parsed, new files: xml.{cc,hh}
	(2005/04/25, Eric Varadaradjou)
	- bug fixed if hostname length is bigger than RTPNAME_LEN (rtpsess.cc)
	- AC_SUBST on mysql and xml2 CFLAGS
	- fix RTPNAME_LEN too short if hostname length too large
	(2005/04/24, Philippe Dax)

-- 6.4.5
	- add check libxml2 in configure.ac
	- improve guide with angular position changing along the path
	(2005/04/05, Philippe Dax)

-- 6.4.4
	- fix a bug in src/vreng/Makefile.am
	(2005/03/24, Philippe Dax)

-- 6.4.3
	- add object guide #48 to make automatic visits
	(2005/03/18, Matthieu Hennion, Stephane Curinckx)

-- 6.4.2
	- split vgl.hh in render.hh draw.hh ogl.hh
	- remame class VGContext by Render
	- anti-aliased lines
	- add wired view
	- add object flag #47
	- add displaylist in walls.cc
	- fix a bug on displaylist in mech.cc
	- add new sound rups.au
	- add flash effect when capturing image
	- add &user=<username> in httpd log
	- move src/vgl/bones/ to src/bone/
	- move src/app/vnc/ to src/vnc/
	- pruning code, minor fixes in render.cc
	- minor fixes in world.* (static -> virtual) methods
	- publish laser to network, property PROPLASER
	- laser replication
	(2005/03/08, Philippe Dax)

-- 6.4.1
	- move vgl/glpng/inf* to 3party/glpng/zlib/
	- move vgl/glpng/png* to 3party/glpng/png/
	- move vgl/glpng to 3party/glpng
	- png.cc without glpng code
	- rename 3part to 3party
	(2005/02/15, Philippe Dax)

-- 6.4.0 <-- gnu.org announce
	- fix Copyright terms
	- fix compilation errors under gcc-4.0, thanks to Andreas Jochens
	- capture a sequence of jpeg images to allow to make a clip
	- function VglCopyPixels used by capture added in solid.cc
	- add a button and action to enter into first person view
	- remove dbs and elc objects from distribution
	(2005/02/11, Philippe Dax)

== 6.3
-- 6.3.10
	- add new sounds in snd.cc (clack, camera)
	(2005/02/02, Philippe Dax)

-- 6.3.9
	- Laser ray oriented on the click direction
	(2005/01/28, Eric Varadaradjou)

-- 6.3.8
	- move getSurfVecs from vrelet.cc to 3dw.cc
	- move calcVar from vrelet.cc to matvec.cc
	- fix server vacs to be independent of vreng data versions
	(2005/01/27, Philippe Dax)

-- 6.3.7
	- fix a bug in mech.cc introduced since V-6.3.3
	- fix a bug in anim.cc introduced since V-6.2.10
	- add a laser ray between user'eye and the center of the selected object
	- ttl fixed at 3 sec when moving user towards an object
	- add moveBehind and change moveNear by moveFront
	(2005/01/21, Philippe Dax)

-- 6.3.6
	- decrease messagePane width to adapt the 3D width with the main window
	- add moveNear() callback function in move.cc
	- add moveUserToObject() method to move user in front of an object
	(2005/01/17, Philippe Dax)

-- 6.3.5
	- add capture by glReadPixels() to a JPEG file via saveJPG()
	- add HAVE_JPEG detection in configure.ac
	- add third party-software gl2ps in src/3part/gl2ps/
	(2005/01/14, Philippe Dax)

-- 6.3.4
	- fix a bug in col.cc (step collision didn't work since V-6.2.3)
	- add 2 far viewpoints
	- add flip button to extend control panel in the GUI
	- add saveJPG()
	(2005/01/12, Philippe Dax)

-- 6.3.3 <- OpenGL annouce
	- fix a bug when replicating the full description of a remote avatar
	- add rate argument to -f,--frames to indiquate the maximum of fps
	- dissociate notice and warning messages by different colors
	- capture vreng window using xwd and convert (ImageMagick)
	- add capture button in the control pannel
	- add camera.xpm and movie.xpm
	- add WObject::updateMysSqlPosition() & WObject::flushMySqlPosition()
	(2005/01/07, Philippe Dax)

-- 6.3.2
	- current world fetches the balls in the MySql DB and recreates them
	- add VRSql::getSubstring to get the full string if a pattern matches
	- when a ball is destroyed, delete it from the MySql database
	- make ball object persistent
	- add getCountBall in mysql.cc
	- modify getCountCart in mysql.cc
	- new xpm files for x,y,z positive and negative translations
	- dynamic switching of navigator/manipulator in the control panel
	- move wo/keys.hh in gui/keys.hh
	- add -s,--silent option to disable sound effects
	(2005/01/02, Philippe Dax)

-- 6.3.1
	- Manipulator calls setLspeed and setAspeed in ubnavig.cc
	- add actions setLspeed, setAspeed into carrier.cc
	(2004/12/29, Philippe Dax)

-- 6.3.0
	- add row index parameter to MySql get methods
	- add xpm files to represent translations and rotations on the objects
	- add methods to move objects from the mouse in carrier.cc
	- add manipulator control menu in ubnavig.cc
	- add panel to manipulate position and orientation of objects
	(2004/12/14, Philippe Dax)

== 6.2
-- 6.2.11
        - fix a compilation bug in mirror.cc
	(2004/12/14, Philippe Dax)

-- 6.2.10
	- add tips on buttons into the GUI
	- add MySql methods: deleteRows, insertCol, updateString, getCountRow
	- add engine methods: updatePosToNetwork, updateLasting, testMoving
	(2004/12/10, Philippe Dax)

-- 6.2.9
	- fix small bug in mysql.cc and door.cc
	- add persistency to ball, book
	- Objects gate, board, mirage, step, clip, movie, ftp are now persistent
	(2004/12/06, Philippe Dax)

-- 6.2.8
	- Objects web, doc, lift, music are now persistent
	(2004/12/03, Philippe Dax)

-- 6.2.7
	- Objects thing, host are now persistent
	- add methods to handle position and orientation persistency in mysql.cc
	(2004/11/30, Philippe Dax)

-- 6.2.6
	- fix an HUGE bug: string "Content-Type:" too long from apache2 server
	- fix a bug when VRSql::quit is invoqued, don't delete vrsql
	- remove DBG_THRD bit from debug.hh
	- change line options: -d,--debug mask; -u,--universe; -p,--pseudo, ...
	- add -n,--number number : max of simultaneous threads to download
	(2004/11/27, Philippe Dax)

-- 6.2.5
	- add -U,--universe comand line option
	- add universe.{cc,hh}
	- add COPYRIGHT notice to all files
	(2004/11/21, Philippe Dax)

-- 6.2.4
	- use urlAbs() in world.cc to find world by url
	- fix urlAbs() to take in account the DEF_URL_PFX
	- use macros FREE, DELETE and DELETE2 (delete[]) to free memory
	- add --enable-efence to link against libefence (for memory debugging)
	- fix door.cc by calling the VRMsql destructor to do a mysql_close()
	- add -g,--no-gravity command line option
	- add descr="... description string ..." attribute
	(2004/11/18, Philippe Dax)

-- 6.2.3
	- object wall now mobile and takable like object thing
	- limit cpu usage in the case where the rate fps is too high
	- rename all headers .h to .hh
	- add subdir gui/xpm where are *.xpm images
	- executable name is "vreng" instead of "vreng-ogl"
	- executable script wrapper name is "vre" instead of "vreng"
	- remove CHECK-LIB on -lSM -lICE -lXext from configure.ac
	- remove support for obsoleted TinyGL
	- remove support for obsoleted motif and X11-athena
	- add option --with-httpd-prefix=[DIR] in configure.ac
	- fix a loop in col.cc
	(2004/11/12, Philippe Dax)

-- 6.2.2
	- add -d, --display host for X11 DISPLAY
	- add long options -- style
	- new files options.cc & options.h
	- change macro GEN_URL and DEF_URL_PFX in defaults.h
	- add missing file question.xpm into the distribution
	(2004/11/05, Philippe Dax)

-- 6.2.1
	_ fix a bug in door.cc when MySql is not available
	- split env.cc into env.cc and inits.cc
	- fix some bugs in vac.cc
	- use #include to include xpm image files into theme.cc 
	- new buttons in neon style for the menubar
	- move suffixes of text files built by txt2str from .h to .t
	(2004/10/23, Philippe Dax)

-- 6.2.0
	- requires ubit-4.2.0 -> ftp://ftp.enst.fr/pub/ubit/ubit-4.2.0.tar.gz
	- upgrade gui to be compliant with ubit-4.2.0
	(2004/09/24, Eric Lecolinet)
	- fixed bug in carrier.cc when we leave an object already leaved
	- add roll and tilt camera movement into the control panel
	- increase window size from 480x270 to 600x360 (ratio 16/9)
	- fix boneVect3D.h: compile error under gcc-3.4
	- update mech.cc to use drawing routines from draw.cc
	- modify line shape (length, thick)
	- add pyramid shape
	(2004/09/28, Philippe Dax)

== 6.1
-- 6.1.4
	- update door.cc and vrmysql.cc, remove setSqlState from wobject.cc
	- fix bug in parse.cc where strtok makes a segfault under Linux in libc
	(2004/07/15, Philippe Dax)

-- 6.1.3
	- change mysql server to an other one, because firewall filtering
	- configure.ac revision (CXXFLAGS & lib paths under MocOSX)
	- add setlocale for international environment, but not used now
	(2004/07/12, Philippe Dax)

-- 6.1.2
	- unify each object's state in the state.state variable
	- change a lot of #define to static const
	(2004/06/29, Philippe Dax)

-- 6.1.1
	- new files vac.cc vac.h to handle vac operations
	- fix gate.cc to resolve unknown world urls via the vacs server
	- update chanstr.cc to return strings by reference
	- fix gate.cc to join a group (this bug appeared since version 5.2.0)
	- update ubnavig, vnc, carrier, move for mouseEvent & keyEvent methods
	- move method VglClickToDir from render.cc to clickDirection in solid.cc
	(2004/06/22, Philippe Dax)

-- 6.1.0
	- change <matrix> </matrix> by <transform> </transform>
	- add <scene> </scene>
	- add <local> </local> tags : local coordinate system (push and pop)
	(2004/06/15, Philippe Dax)

== 6.0
-- 6.0.0
	- fix worldname of user forgotten when swapping worlds
	- add ptsize="val" attribute
	- resolve keys Command & Option on an Apple keyboard
	- remove attribute body="url", url becomes a child of android
	- remove attribute java="class", class becomes a child of vrelet
	- anim object gets nbframes value from its solid not from an attribute
	- update statue to be tagged
	- tag </solid> at the end of multi-frame solids
	- add tag <frame for multi-frames solids
	(2004/06/06, Philippe Dax)
	- change attribute refname="..." by use="..." for object button
	- add value shape="point"
	- rotations and translations for multi-solids (3dw.cc, solid.cc)
	(2004/06/08, Philippe Dax)
	- default values for book
	- vrelet: attribute class becomes java
	- tag <vre ...> ... </vre>
	- tag <object ...> ... </object>
	- tag <solid ... />
	(2004/06/02, Philippe Dax)
	- font by default for the text object
	- attributes can be written without an imposed order
	- attribute name="..." is managed as other attributes
	- add tag <meta ... /> inside <head> ... </head>
	- parseGeometry becomes parseSolid
	- parseObject becomes parseTag
	- maintain description file .vre in the cache during the session
	- fix XML comments <!-- ... -->
	(2004/05/03, Philippe Dax)
	- fix syntax of vnc and elc objects
	- update vacs for v6
	- new version of syntax (v6)
	(2004/04/30, Philippe Dax)

++ 5
== 5.2
-- 5.2.0
	- fix http.cc to allow Apache VirtualHost
	- remove /net/vreng prefix
	(2004/04/23, Philippe Dax)
	- view vertical above the user
	- move gcontext.h into vgl.h
	- newlines accepted in the XML object description
	(2004/04/15, Philippe Dax)
	- repeat texture support
	- add ase (Ascii Scene Export) model loader
	- add 3ds (3D Studio Max) model loader
	- view 'turn around' the local user
	- view local user as third person (render.cc, gcontext.h, user.*)
	(2004/03/22, Romain Piegay)
	- use ubit-4.0.2 -> ftp://ftp.enst.fr/pub/ubit/ubit-4.0.2.tar.gz
	- add url.{cc.h} to manipulate urls
	- add new memory counters in stat.*
	- restructuration of http.* and thread.*
	- add --enable-profiling to use gprof tool
	(2004/03/15, Philippe Dax)
	- add --enable-dmalloc with -DMALLOC_FUNC_CHECK in configure.ac
	- new VREP header (version 3), uncompatible with previous version
	- list of transformations in matrix.cc
	- new syntax in matrix.cc
	- fix render.cc vs landmark.cc
	- class lwObject in lwo.cc
	(2004/03/09, Philippe Dax)

== 5.1
-- 5.1.5
	- fix inet_ntop declaration id MacOSX
	- add bug-report email in configure.ac
	- For each object, initFuncList(), renamed init(), is now in its class
	- enums now inside classes
	- add config.h viewer in the help menu
	- smaller font size in the menubar
	- update servers/vjs/Makefile.am
	- fix a bug in col.cc
	(2004/02/23, Philippe Dax)

-- 5.1.4
	- add face parameter in solid.cc to choose CULL_FACE
	- more comments to doxygen
	- redefine some classes in bones/
	- fix blend bug in text.cc introduced with 5.1.3
	- fix stillToTurn bug in book.cc introduced with 5.1.3
	- move models loading from model.{h.cc} to format.{h.cc}
	(2004/02/16, Philippe Dax)

-- 5.1.3
	- fix buffer passwdFile too short in vnc.h
	- add params color, verso, scale to text object
	- update script vreng.in to allow 15 planes on MacOSX
	- update man vreng.1
	- add option -i to show OpenGL infos (GL/GLU versions and extensions)
	- add ogl.cc to check weither OpenGL extensions are supported
	- fix counter color in log.cc by a glDisable(GL_LIGHTING)
	(2004/02/08, Philippe Dax)

-- 5.1.2
	- add time percentages in stat.cc
	- change default texture size from 128x128 to 256x256
	- update texture.cc to don't resize 128x128, 64x64, 32x32 textures
	- update Image constructor, remove type parameter (still/anim)
	- fix Vjc constructor/destructor
	(2004/02/02, Philippe Dax)
	- fix input parameters in the Vnc dialog box
	- add xpm metaltile and woodtile in theme.cc
	(2004/01/30, Eric Lecolinet)

-- 5.1.1
	- update app/vnc
	- update vrmysql such as the object's class name is the MySql table name
	- add TGA loader
	(2004/01/26, Philippe Dax)

-- 5.1.0
	- thanks to chris.coleman@arcucomp.com for fixing minor bugs
	- use automake-1.8.2, autoconf-2.59
	- define HAVE_MYSQL, HAVE_GLU
	- -DDARWIN becomes -DMACOSX
	- replace some #define ... by enum {...};
	- add elc object for further use
	(2004/01/16, Philippe Dax)

== 5.0
-- 5.0.3 <-- www.opengl.org news
	- submit news to www.opengl.org
	- fix bad <solid=...> to solid="..."
	- speed regulation for ball object according to the vreng rate
	- bouncePosition function to bounce mobile objects on an obstacle
	- project user movement of user on an icon
	- use extension if subtype mime is plain
	(2004/01/12, Philippe Dax)

-- 5.0.2
	- use open command under MacOSX to do execs in {browser,pdf,office}.cc
	- fix bad value of ssrc by getMySsrcId() in pkt.cc
	(2003/12/14, Philippe Dax)

-- 5.0.1 <-- Debian 2004
	- fix configure.ac for NetBsd os
	(2003/11/21, Philippe Dax)

-- 5.0.0
	- add snapshot and fingerprint targets in Makefile.am
	- exec open command to launch MacOSX applications
	- add icon flashing
	- interaction with 'ums' (Ubit Mouse Server)
	- xmlize description of objects (parse.cc)
	- add App::checkUrl using wget --spider
	- rename vijs to vjs (Vreng Java Server)
	- rename ifcserver to vjc (Vreng Java Client)
	- rename ifc to vrelet (VREng servLET)
	- rename wobject.h to wo.h, remove wmgt.h
	- add link behavior in gate object
	- remove link object
	- remove obsoleted cell object
	- add earth and moon behaviors in mirage object
	- remove earth and moon objects
	- merge push pop rot trans objects in only one object called matrix
	- merge wmgt.cc and wobject.cc in wobject.cc
	- rename plain object to mirage object (no interactions)
        - remove panel object, wall is equivalent
	- m4 macros for java moved from aclocal.m4 to acinclude.m4
	(2003/10/01, Philippe Dax)

== 4.6
-- 4.6.6 <-- Last release before 5.0.0
        - fix my_inet_ntop, my_inet_pton
	(2003/09/01, Philippe Dax)

-- 4.6.5
	- add VRE_VERSION for description files
	- fix world back and world forward
	(2003/08/27, Philippe Dax)

-- 4.6.4
	- rename baps to vaps (Vreng Animation Parameters Server)
	- rename ifcs to vijs (Vreng Interaction Java Server)
	- rename reflector to vrum (Vreng Reflector Unicast Multicast)
	- change -d by -D option to allow X11 Toolkit command arguments
	- add --enable-standalone to use vreng without network connectivity
	- add --enable-vacs to use locally the vacs server
	- update channel.cc, gui.cc, vreng.cc to use the vacs server
	- add servers/vacs (Vreng Address Cache Server) to distribute channels
	(2003/07/17, Philippe Dax)

-- 4.6.3
	- requires ubit-3.4.1 -> ftp://ftp.enst.fr/pub/ubit/ubit-3.4.1.tar.gz
	(2003/07/03, Eric Lecolinet)
	- settings menu redesigned
	- move net/dates to com/dates
	- remove all obsolete #ifndef VRENGD
	- clock object with argument needle=s|m|h
	(2003/07/10, Philippe Dax)

-- 4.6.2
	- add UBIT_VERSION in configure.ac
	- add format.{cc,h} to handle file extensions and mime subtypes
	- fix clock, remove network operations
	- fix iconRemove to remove local persistent file
	- add roll action to icon
	- add icon persistency for the owner (~/.vreng/icon/*)
	- add iconSave
	- add put and publish file, iconOpen on a published file
	(2003/07/01, Philippe Dax)

-- 4.6.1
	- require ubit-3.4.0 -> ftp://ftp.enst.fr/pub/ubit/ubit-3.4.0.tar.gz
	- fix control menu position in the 3D (observed with ubit-3.3.0)
	(2003/06/19, Eric Lecolinet)
	- add office loader app/office.cc (staroffice, openoffice)
	- add pdf loader app/pdf.cc (acrobat-reader)
	- add object 'icon' to put, manipulate and share documents into the 3D
	- add m4 macros for java into aclocal.m4 and configure.ac
	- resolve reflector.enst.fr
	- minor bug fixes in vrmysql.cc
	- add createVolatileNetObject()
	- bug fixes in channel.cc
	(2003/06/19, Philippe Dax)

-- 4.6.0 <-- www.cygwin.com news
	- require ubit-3.3.0 -> ftp://ftp.enst.fr/pub/ubit/ubit-3.3.0.tar.gz
	- update ubxwin.cc to be compatible with ubit-3.3.0
	(2003/05/14, Eric Lecolinet)
	- control rate by checkRate (stat.cc, guy.cc, water.cc)
	- fix lessFrequency in water.cc
	- add inet4_ntop, inet4_pton functions in vrnetdb.cc
	- toggle counter display in landmark.cc 
	- reorder AC_CHECK_LIB in configure.ac
	(2003/05/28, Philippe Dax)

== 4.5
-- 4.5.7
	- add take and drop actions for ball
	- fix aoiQuit
	- add grid colors
	(2003/05/12, Philippe Dax)

-- 4.5.6
	- fix a huge BUG in move.cc, introduced since Version 4.0.0,
          to enable network publishing for objects in movement
	- classes documentation ala 'doxygen'
	- change returns of GetLine/NextLine
	- add on/off actions in mirror.cc
	- sdrListen now in a new and independent module sdr.cc
	- new functions socketDatragram() and socketStream() in socket.cc
	- rename vrsocket.cc in socket.cc
	- remove obsoleted code about proxy-multicast from channel.cc
	- fix ball up
	(2003/05/07, Philippe Dax)

-- 4.5.5
	- update aoi using Aoi class
	- update vrmysql using Sql class
	- fix --with-ubit-prefix in configure.ac
	- remove wait.h because conflit with /usr/include/wait.h under some OS
	- book now reads in first the number of pages (lines)
	- change integer types to be in accordance with stdint.h
	- add vgl/glu.{cc,h} in the case where GLU is not found
	(2003/04/17, Philippe Dax)

-- 4.5.4
	- baps -v gives current version
	- fix a bug in my_inet_ntop
	- check more functions in configure.ac for vrnetdb.cc
	- fix my_wait for a child process
	- add buttons and dialogs to display README, COPYRIGHT, COPYING,
          Changelog, TODO
	- add gui/txt2str.cc to convert files into strings
	- add check of utime.h in configure.ac
	- set current time for files downloaded in the cache
	- accept empty lines in .vre files
	- remove the last fatal calls
	(2003/04/11, Philippe Dax)

-- 4.5.3
	- build default preferences in ~/.vreng/prefs
	- add callback view world source
	- add callback view worlds list
	- add callback view prefs
	- replace view dialog by view menu containing 4 buttons:
          source, worlds, prefs, axisgrid
	- remove worlds button in the menubar
	- change DEF_HTTP_SERVER with vreng.enst.fr
	- add BMP (Microsoft image format) reader
	(2003/04/01, Philippe Dax)

-- 4.5.2
	- automatic detection of MBone by listening SAP/SDP announcements
	- new format of book's urls, only one line per page
	- fix downloadInCache for jpg, png, sgi
	- update man vreng.1
	(2003/03/24, Philippe Dax)

-- 4.5.1
	- add src/servers/reflector/ tree into the distribution
	- add reflector IP Unicast/Multicast for those who are not on the MBone
	(2003/03/21, Samuel Tardieu)

-- 4.5.0
	- require ubit-3.2.0 -> ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit-3.2.0.tar.gz
	(2003/03/19, Eric Lecolinet)
	- add objects 'book' (#53) and 'sheet' (#54) with appropriated actions
	(2003/03/18, Julien Desreumaux - Sebastien Piraud)

== 4.4
-- 4.4.5
	- add axis and grid landmarks into the 3D with landmark.{cc,h}
	(2003/03/17, Mario Trentini)
	- fix IP_ADD_MEMBERSHIP by struct ip_mreq in configure.ac
	- fix java2 in configure.ac
	- add gl font -mixed-fixed-medium-r-* to display values into the 3D area
	(2003/03/11, Philippe Dax)

-- 4.4.4
	- fix compute of codebase url in ifc.cc
	- add stdint.h if available
	- fix bugs in render.cc
	- fix ifcs/Makefile.am
	(2003/03/07, Philippe Dax)

-- 4.4.3
	- use new autoconf-2.57, automake-1.7
	- remove WANT_WGET
	- add functions: my_inet_ntop, my_inet_pton in vrnetdb.cc
	(2003/03/05, Philippe Dax)

-- 4.4.2
	- using getipnodeby* if available instead of gethostby*{,_r}
	(2003/03/03, Philippe Dax)

-- 4.4.1
	- add manual baps.8
	- add actions into mech
	- fix call to gethostbyname_r()
	(2003/02/28, Philippe Dax)

-- 4.4.0
	- fix an HUGE BUG about tex_xp, tex_xn, tex_yp
	- change all .vre files (v4), but keep the olders (v3) for compatibility
	- new initial world named 'RendezVous.vre' (v4)
	- add function copyPosZAndBB to resolve rotation around X axis
	- add rendering angles of replicated avatars
	- change keysyms, eliminate Shift modifiers, prefer Alt & Meta & Control
	- add alias name for solid shapes
	- add solid attributes: blink, slices, stacks, cylinders, circles
	- add solid shapes: 'cross', 'sphere+disk', 'cone+disk'
	- remove solids: 'boxblend', 'sphereblend' because alpha is detected
	- rename support/ directory to config/ and cleanup it
	- rewrite mirror.cc, add stencil for reflexive solids in render.cc
	- fix texture mapping on the local user
	- use ~/.vreng/cache rather than /tmp for downloading files
	- add manual vreng.1
	- bug fixes applied to be compliant with Debian GNU/Linux distribution
	(2003/02/25, Philippe Dax)

== 4.3
-- 4.3.1 <-- Debian 'sarge'
	- update music object to play midi pieces of music
	- add call to timidity++ player to play midi sounds by http in app/
	- add Worlds button in menubar for futur feature
	- fix GUI settings
	- fix old help string
	(2003/01/20, Philippe Dax)

-- 4.3.0
	- add --enable-baps and --enable-ifcs to compile optionnal servers
	- add Mozilla browser
	- Http class added
	- ImgReader class updated
	(2003/01/07, Philippe Dax)

== 4.2
-- 4.2.0
	- require ubit-2.11.0 -> ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit-2.11.0.tar.gz
	- gui/* compliant with ubit-2.11.0
	(2002/11/20, Eric Lecolinet)

== 4.1
-- 4.1.11
	- fix lift movement
	- fix check of fpsetmask in configure.in
	(2002/11/19, Philippe Dax)

-- 4.1.10
	- add basic support for win32: windoze.{cc,h}
	- config.guess (timestamp 2002/07/09), config.sub (timestamp 2002/07/03)
	- add README.VNC in the distrib
	- minor changes in configure.in
	(2002/11/08, Philippe Dax)

-- 4.1.9
	- add call action to call a lift
	- don't new a World if already exists
	- change world list chaining
	- add entry attribute in gate.cc to comeback with an adequate position
	- add -DREENTRANT in CXXFLAGS if libpthread is there
	(2002/10/21, Philippe Dax)

-- 4.1.8
	- fix EXTRA_DIST in conf/Makefile.am
	- fix configure.in to link with -lpthread
	- class imgReader to read images
	(2002/10/09, Philippe Dax)

-- 4.1.7
	- add code to download jpeg files
	- do --enable-pthread by default
	- uncomment --enable-ipv6 (work in progress)
	(2002/10/07, Philippe Dax)

-- 4.1.6
	- fix bug in configure.in to find libubit in other places
	- move string symbols to other names (string khown by c++)
	- fix blending with glDepthMask
	(2002/10/04, Philippe Dax)

-- 4.1.5
	- blended solids (shape boxblend)
	- new sub-directory servers and move bap and java inside it
	(2002/09/30, Philippe Dax)

-- 4.1.4
	- fix guy.cc and mech.cc if libGLU is not present
	(2002/09/17, Philippe Dax)

-- 4.1.3
	- first port under MacOS X 10.1.x with Xtools successful
	- check libpng and libz, if they exist don't compile vgl/glpng/{png*,z*}
	- fix order of include paths for ubit
	(2002/09/16, Philippe Dax)

-- 4.1.2
	- move content of sdr and mime directories into conf directory
	- delete cfg directory of vre examples from the distribution
	- delete srv directory from the distribution
	- fix order of include paths and java paths in configure.in
	(2002/09/15, Philippe Dax)

-- 4.1.1
	- check if socklen_t exists
	- remove obsoleted dependencies in */.deps/
	(2002/09/11, Philippe Dax)

-- 4.1.0
	- add particle object to produce special effects (rain, snow, fountain)
	- add fish object to swim into the water
	- fix mp3.cc and mpg.cc to handle relative urls
	- fix entry, bgcolor, dbs objects to be unique per world
	(2002/09/06, Philippe Dax)

== 4.0
-- 4.0.3
	- fixes theme.cc (classical and flashy buttons)
	- fixes to open relative url in browser.cc
	- fixes in ifc parsing
	- fixes in selection mode
	(2002/09/01, Philippe Dax)
	- extract code from android.cc to make a vnp (vertices,normals,polygons)
          loader in vgl/vnp.{h,cc}
	(2002/08/01, Philippe Dax)

-- 4.0.2
	- split lwo object into model object and lwo loader
	(2002/07/27, Philippe Dax)

-- 4.0.1
	- move bgcolor structure from render.cc to bgcolor class
	- internal clock handled by the current world
	- selection in 3D on a non solid gives infos on the scene
	- rename cd object to music object
	- rename cinema object to movie object
	(2002/07/27, Philippe Dax)

-- 4.0.0
	- require ubit-2.6.0 -> ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit-2.6.0.tar.gz
	- revised syntax to describe objects (uncompatible with previous releases)
	  ex. (name) position geometry attribute=value ...
	- the entire code is rewritten in C++ ``spirit''
	- water is now transparent
	- add new cart object to bring in a basket small objects
	- add new carrier object to move small objects
	- add classes: Session NetObject Rtp Channel Payload Solid VGContext App
	(2002/07/12, Philippe Dax)

++ 3
== 3.6
-- 3.6.0
	- require ubit-2.4.3 -> ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit-2.4.3.tar.gz
	- require libungif.a
	- gui with new ubit release
	(2002/04/22, Eric Lecolinet)

== 3.5
-- 3.5.7
	- replace fatal() by warning()
	- suppress assert()
	(2002/04/19, Philippe Dax)

-- 3.5.6
	- fixes to run under Cygwin
	(2002/04/10, Philippe Dax)

-- 3.5.5
	- add dbs object for future extensions
	(2002/04/08, Philippe Dax)

-- 3.5.4
	- fix pixmap generation r = g = b (gray) in pgm.cc
	- rename sql.{cc,h} in vrmysql.{cc.h} for further sgbd extensions
	(2002/03/26, Philippe Dax)

-- 3.5.3
	- add pgm loader in pgm.cc
	- check if mysql_error function exists to avoid link-editing errors
	- new bap and fap buttons
	(2002/03/25, Philippe Dax)

-- 3.5.2
	- update bap server to run on Win32/Cygwin
	 (2002/03/01, Jose-Marques Soares - int-evry.fr)

-- 3.5.1
	- option --with-mysql with default=yes in configure.in
	- change int id_objet to string <given_name@world> in the mysql table
	(2002/02/28, Philippe Dax)

-- 3.5.0
	- add mysql client interface in sql.cc to handle objects persistency
	(2002/02/22, Pierre Gros)
	- http errors !=200 now explicitly printed on stderr
	- adapt lightwave object (LWOB) loader lw.cc with HTTP in lwo.cc
	(2002/02/04, Philippe Dax)
	- add new objects 'push', 'pop', 'trans', 'rot'
	  to handle relative transforms in 3D
	- add new object 'lwo' to render LightWave Objects Models
	(2002/01/25, Pierre Gros)

== 3.4
-- 3.4.5
	- suppress AC_C_CONST from configure.in, don't check working const
	  resolve compilation errors with <string.h>
	(2002/01/25, Philippe Dax)

-- 3.4.4
	- all fcntl functions now in vrsocket.cc
	- new files vrsocket.{cc,h} channel independent, remove mcast.cc
	(2002/01/21, Philippe Dax)

-- 3.4.3
	- change static const int XXX = value by #define XXX value
	- suppress floatingpoint.h which conflicts with stdlib.h
	(2002/01/11, Philippe Dax)

-- 3.4.2
	- fix synchronization problem in Unicast with baps server in android.cc
	- fix bug computing send timeout in baps.cc
	- complete fap name in face.h and face.cc
	- includes ubit as <ubit/*.hh>
	- relook sysdep.h, random.cc
	- handle new fap2.1 header in android.cc
	- fix bugs pointed out by Debian testers
	(2002/01/07, Philippe Dax)

-- 3.4.1
	- requires ubit-b1.12.0: ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit.tar.gz
	- fix AC_CHECK_X11 in configure.in to work with ubit, motif and athena
	- fix amwidgets.cc and amxwin.cc to run with recent changes
	(2001/12/03, Philippe Dax)

-- 3.4.0 <-- Debian stable 'woody'
	- requires ubit-b1.11.7: ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit.tar.gz
	- move and rewrite vnc functions from ubnavig.cc to vnc.cc
	- less dependencies with Xt in ubxwin.cc
	- new control menu to navigate in ubwidgets.cc
	(2001/07/30, Eric Lecolinet)

== 3.3
-- 3.3.4
	- fix confused --enable --with in configure.in
	- add with-ubit-prefix=DIR in configure.in
	- suppress handle of colormap in ubxwin.cc
	- fix zombie child in baps.cc
	(2001/11/26, Philippe Dax)

-- 3.3.3
	- no more difference between OpenGL and Mesa3D
	- change all execl to execlp
	- deletion of AC_PATH_PROG in configure.in
	- deletion of helpers/vrpaths.h
	(2001/11/20, Philippe Dax)

-- 3.3.2
	- FAP integration (quick and dirty ;-)) in android.cc and face.cc
	(2001/11/16, Philippe Dax)

-- 3.3.1
	- fix magic formula to read bap files version 3.2 in android.cc
	(2001/11/14, Guillaume Brule - int-evry.fr)
	- fix left knee rotation in android.cc
	- adapt bap translation numbers 181,182,183 in android.cc
	- adapt actions to new gestures in android.cc
	- fix getopt parsing in baps.cc
	- change effects of lighting in render.cc
	- suppress gui/tcl* and gui/glut* from the distribution
	- add color parameter for vnc object
	(2001/11/15, Philippe Dax)

-- 3.3.0
	- replace xresources.cc by getopt in gui.cc
	- do scaling on the fly when downloding models in android.cc
	- moving composed objects in thing.cc
	- add <group>...</group> to group objects in parse.cc
	- add shape type of solid in struct Solid
	- fix args of create_pthread in thread.cc and audio.cc
	- fix immobilty of water after a laps time
	- add comments for model files *.m
	(2001/11/12, Philippe Dax)

== 3.2
-- 3.2.5
	- fix ubxwin.cc: no more reference to colormap
	(2001/10/29, Alexandre Fenyo)
	- update vreng script-shell: no more reference to vreng-tgl (TinyGL)
	- new DBG flags: DBG_MAN, DBG_VNC, DBG_IFC
	- ifcserver: server-port and local-port become optional in vre file
	(2001/10/29, Philippe Dax)

-- 3.2.4
	- fix vreng script-shell about LD_LIBRARY_PATH
	- fix the vnc destructor causing a seg-fault
	- fix baps.cc to broadcast with a ttl scope of 127
	- fix some check-headers in configure.in
	- fix wrong action list in android.cc
	- fix too many open files in baps.cc
	(2001/10/24, Philippe Dax)

-- 3.2.3
	- fix replication side effects by suppressing unusefull WObject copies
	(2001/10/11, Philippe Dax)

-- 3.2.2
	- add skin color parameter to android object
	- check inet_pton in configure.in
	- unification of all http i/o functions in http.cc
	(2001/10/09, Philippe Dax)

-- 3.2.1
	- fix gethostbyaddr() call in baps.cc
	- fix select() CPU looping to become blocking in baps.cc
	- rename 'lara' object to 'android'
	(2001/09/24, Philippe Dax)

-- 3.2.0
	- bap streaming send by IP Multicast to all participants by default
	- add buttons 'yes' and 'no' for face animation
	- add buttons 'smile' and 'sulk' in lara actions
	- face animation called by Lara::changePermanent()
	- face models indirectly pointed by the content of the joint-point url
	- each lara object can define its own face by an url (face=url_face)
	- each lara object can define its own bap server (baps=bap_host)
	(2001/09/21, Philippe Dax)

== 3.1
-- 3.1.0
	- update client/server handshaking to play bap files
	- fix dimension and space position of lara object
	- fix skin color and collide behavior in lara object
	(2001/08/27, Philippe Dax)
	- add face object in conjunction with body's lara
	- add face animation built with bones, zv/bones tree
	(2001/08/03, Yann Renard)

-- 3.0.0	<-- (full C++) - unstable, obsoleted by 4.0.0
	- requires ubit-b1.7.0: ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit.tar.gz
	(2001/07/27, Eric Lecolinet)
	- add src/bap tree containing Mpeg4/bap server
	- add checking java2 path in configure.in
	- update config.guess & config.sub
	(2001/07/27, Philippe Dax)
	- add src/java tree for ifcserver
	(2001/07/27, Mathieu Seigneurin)
	- add downloader in 'lara' for models coming from 3D Studio Max tool
	(2001/07/10, Philippe Dax)
	- new directory src/java
	- add new object ifcserver(#43) (Interface Communication Server)
	- add new object ifc(#42) (Interface Communication)
	(2001/06/29, Mathieu Seigneurin)
	- add new object vnc(#41) (Virtual Network Computing)
	- add new helper directory vnc (Virtual Network Computing)
	- build a CVS tree
	(2001/07/06, Martin Renard, Mathieu Picard, Laurent Maingault,
		       Stephane Ploix, Clement Sommelet)
	- network interface in 'lara' to read Mpeg4 flows in Bap format
	- add in 'lara' an other model built with triangles called 'man'
	- add new object lara(#40) a Lara Croft like
	(2001/06/15, Ankit Jalote)
	- WClass instance class for objects
	- WObject is the top level class for all objects
	- all objects have their own class
	- all *.c source files are now rewritten in C++ (*.cc)
	- add intuitive mouse navigation
	(2001/05/29, Eric Lecolinet, Philippe Dax)

++ 2
== 2.6
-- 2.6.5	<-- stable in C language
	- bug fixes for Motif
	(2001/06/21, Philippe Dax)

-- 2.6.4
	- add intuitive mouse navigation
	- requires ubit-b1.5: ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit.tar.gz
	(2001/05/22, Eric Lecolinet)
	- delete some memset on WObject
	(2001/05/28, Philippe Dax)

-- 2.6.3
	- rewrite 3dw.c interaction between objects and solids
	(2001/05/09, Philippe Dax)

-- 2.6.2
	- minor bug fixes
	(2001/04/28, Philippe Dax)

-- 2.6.1
	- add new object thing(#39) to be dragged and dropped
	(2001/04/09, Philippe Dax)
	- code cinema.c to map Mpeg1 videos on textures in 3D
	(2001/03/30, Raphael Duee, Nicolas Durand, Wakim Hakim, Nicolas Wack)

-- 2.6.0
	- ./configure uses by default Ubit & Mesa3D vs X11-Athena & TinyGL
	(2001/03/26, Philippe Dax)
	- comment Glut & Tcl/Tk targets in configure.in because not supported
	- add AC_CHECK_LIB for libmpeg in configure.in
	- add warning notifications when the avatar collides a gate or a link
	(2001/03/22, Philippe Dax)

== 2.5
-- 2.5.9
	- fix SolidGetSelection for objects with specialRendering
	(2001/03/19, Patrick Bellot)
	- add bbox solid geometry without draw for objects with specialRendering
	- enable drawing with LINES for box, rect, cylinder, disk, octagon
	- add style parameter in geometry description (0=fill,1=lines,2=points)
	- add bookmarks button (add bookmark & list bookmarks)
	(2001/03/18, Philippe Dax)

-- 2.5.8
	- requires ubit-0.14.2: ftp://ftp.enst.fr/pub/unix/gui/ubit/ubit-0.14.2.tar.gz
	- new 3D mouse navigator with accelerators
	- new GUI background (metal)
	(2001/03/18, Eric Lecolinet)

-- 2.5.7
	- fix move.c when an object has no BB neither elementary movement
	- add new object guy(#38) taken from glut/progs/demos/walker
	- CSet (Curve Set) loader
	- fix rotations & translations in mech.c
	(2001/03/12, Philippe Dax)

-- 2.5.6
	- bug fix in stars.c stars_colors
	- bug fix in text.c when mapping the string
	(2001/02/22, Patrick Bellot)

-- 2.5.5
	- replace names MesaGL by Mesa3D, oops!
	- when it'is possible change assert()
	- suppress handling of colors in amxwin.cc
	- change API/GUI for Gui*User and Gui*World
	- fixed bug when receiving RTCP/SDES packets
	(2001/02/19, Philippe Dax)

-- 2.5.4
	- add a memory cache for txf fonts
	- add URLs to retrieve additional softwares in configure.in
	- fix quitWorld() which doesn't do a closeChannel()
	- add renderMaterials() in render.c
	- add new object link(#37) to go in a world with the same channel
	- move static variables of mech, stars, text, water in WObject->ext
	(2001/02/12, Philippe Dax)

-- 2.5.3
	- add material GL_EMISSION, lights GL_SPOT_CUTOFF, GL_*_ATTENUATION
	- add scripts configure-{athena,motif,ubit,glut}-{ogl,mgl,tgl}
	- fix glut*.[hc] files
	- clean up code
	- less include dependencies between modules
	(2001/02/05, Philippe Dax)

-- 2.5.2
	- adapt texfont.c and text.c for TinyGL
	- update generalFunctionList[n].render with (WObject *pobj) in argument
	- delete generalFunctionList[n].light
	- struct Nature in struct WObject
	(2001/01/29, Philippe Dax)

-- 2.5.1
	- add new object cinema(#36) (only its template)
	- add http loader for TXF format fonts taken from Mark J. Kilgard 
	- add new object text(#35) to render strings using texture-mapped fonts
	- add new object light(#34)
	- shininess with only one argument
	- suppress irrelevent solids: user,wall,door,clock,mirror,ring,wheel
	(2001/01/26, Philippe Dax)

-- 2.5.0
	- requires ubit-0.12.4
	- new popup navigator to accelerate linear and angular speeds
	(2001/01/12, Eric Lecolinet)
	- no more documentation in the distribution, use the Web
	- new look 3D zone with 16/9 format (like cinema)
	- add void *datas argument to specialAction and all methods
	- move Move structure into WObject structure
	- all objects with angle a3
	- add new object plain(#33), plain is an inert object
	- suppress objects vrml, cone, disk, wheel, ray, ring, sphere
	- cone, disk, sphere, ray, wheel, ring are solid shapes, not objects
	- objects web and vrml are now merged into web object
	- explicit names and url are now in the WObject structure
	- suppress net-strings from parsing for door, anim, button, clock, lift
	- add parsing functions
	- all objects can be explicitly named in config files
	- popmenu displays instance_name
	- earth turns now in the right sense
	- automatic gothrough when collide a gate
	- new object attr: collision behaviour, viscosity, density, persistency
	- bug fixed in col.c when intersect is in progress
	- bug fixed in RtpDump against loops
	- mech object with anim and walk flags
	- suppress a2 angle from panel
	(2001/01/12, Philippe Dax)

== 2.4
-- 2.4.1
	- requires ubit-0.12.0
	- add actions pause and continue in clip.c and cd.c
	- move vre.in to vreng.in to create script-shell vreng and vre
	- use OGL, MGL, TGL in vreng/Makefile.am to create executables
	- add OGL, MGL, TGL conditional variables in configure.in
	- add 'back' and 'forward' buttons to navigate into visited worlds
	- add callbacks backWorld & forwardWorld
	- worlds are now allocated and chained in a list called worlds
	- add feature in clip.c to get A/V streams played by rtpplay on a server
	(2000/12/17, Philippe Dax)
	- add translucid buttons into the 3D to navigate natively with the mouse
	- add translucid popup menus
	- split xwin.cc into 2 files: ubxwin.cc, amxwin.cc
	(2000/12/15, Eric Lecolinet)

-- 2.4.0
	- add conf/vrengicon.xpm in order to iconize with a Window Manager
	- bugs fixed in bullet.c and dart.c when network replication
	- change {add,update,remove}World in ubwidgets.cc, amwidgets.cc
	- change {Add,Update,Remove}World in gui.cc, gui.h
	- new structures World and Universe in world.h
	(2000/12/07, Philippe Dax)
	- add Popup Menus when selecting an object in the 3D area
	- add contextual colors depending on messages: chat, notice, warning
	  requires ubit-0.11.1
	(2000/11/29, Eric Lecolinet)
	- bug fixed in md2.{ch} if 64 bits architecture
	- add dumpSessions() for debugging
	- add properties (web, world, ssrc, rctpname) for user replicas
	- add helpers/modeller.[ch] to launch an 3D modeler
	- add png.c to download PNG files and apply them on textures
	- merge libglpng from Ben Wyatt into zv/ to downdload png files
	(2000/11/29, Philippe Dax)

== 2.3
-- 2.3.7
	- replace URL files extensions by MIME types to download images
	- split rocket(#16) in 2 different objects: bullet(#16) & dart(#38)
	- add object mech(#39) adapted from Simon Parkinson-Bates's source
	(2000/11/13, Philippe Dax)

-- 2.3.6
	- replace md5 from RSA by md5 from FreeBSD
	(2000/10/24, Philippe Dax)
	- some memory leaks fixed
	(2000/10/14, Philippe Dax)

-- 2.3.5
	- keys to increase/decrease/default linear & angular avatar's speeds
	- methods to increase/decrease/default linear & angular avatar's speeds
	- --enable-pthread is not by default in configure.in
	- fix pre-selected buttons in ubwidgets.cc
	- announce projectile sounds on the network
	- add built sounds drip.au and shout.au
	- add helpers/wget.[hc]
	- add tool helpers/slidecast.[hc] to send/receive slides
	- add preferences file conf/prefs in the distrib
	(2000/09/27, Philippe Dax)

-- 2.3.4
	- --enable-pthread is now the default in configure.in
	- add --enable-permissive in configure.in to compile with --permissive
	- playsound() multithreaded -> better performence
	(2000/09/21, Philippe Dax)

-- 2.3.3
	- clear trailer bits from previous sound (Patrick Bellot)
	- bug fixed in vrnetdb.c if option --enable-pthread
	- ambient sound 'bubbles' when entering into the water
	- suppress snd/ directory from distrib
	- builtin sounds using base64: common/base64.[hc], helpers/snd.[hc]
	(2000/09/18, Philippe Dax)

-- 2.3.2
	- modify configure.in to find 64 bits libraries
	- modify vre script to call vreng appl 64 bits
	(2000/09/14, Philippe Dax)
	- rename objects.h -> wobject.h (because objects.h exists in ssh dist)
	(2000/09/11, Philippe Dax)

-- 2.3.1
	- add --enable-optimize in configure.in
	- deletion of src/include and gui/GLw*
	(2000/09/03, Philippe Dax)
	- bugs fixed in moon.c
	- change long to int for init32 and u_int32 (for compiling with 64 bits)
	(2000/08/23, Patrick Bellot)
	- add object water(#37)
	- profiling time functions startTime, stopTime, clearTime
	- add checking GLU lib in configure.in
	- add solid boxblend
	- bug fixed in drawRect()
	(2000/08/23, Philippe Dax)

-- 2.3.0
	- new UI ubit (Ubiquitous Brick ToolKit)
	- navigation with clicks into arrow buttons (not full mouse navigation)
	- work to do more independent files in gui structure
	- add ubit code (.cc,.hh) in gui/u*
	(2000/07/27, Eric Lecolinet)
	- add --with-ubit in configure
	- new object ftp(#36), to download files (as tarballs)
	(2000/07/23, Philippe Dax)
	- bug fix in x11main.c to authorize 16 bits depth visuals
	(2000/07/13, Mathieu Imrazene)
	- decrease texture size from 256x256 to 128x128 to improve rendering
	(2000/07/07, Mathieu Imrazene)

== 2.2
-- 2.2.3
	- add --enable-64bit into configure.in if gcc-2.96 available
	(2000/07/03, Philippe Dax)
	- update support/config.{sub,guess} taken from gcc-2.95.2
	- update src/*/Makefile.am to build separated libraries for vrengd
	- directory src/lib deleted
	(2000/06/28, Philippe Dax)
	- add SGI-images (la, bw, rgb, rgba, sgi) decoder (sgi.{hc})
	(2000/06/26, Philippe Dax)

-- 2.2.2
	- add common/wait.[hc] to do my_wait()
	- add PHTML type to read webpages.phtml
	- add code to use a Multicast Unicast Proxy (MUP) in mcast.c
	(2000/06/15, Philippe Dax)
	- add detection of Multicast Unicast Proxy 'mcast_proxy'
	- improve httpParser
	- add telnet scheme in http.c
	- bug fixed in moon.c
	(2000/06/11, Philippe Dax)

-- 2.2.1
	- CFLAGS -DVRENG if --enable-vrengd given
	(2000/06/09, Philippe Dax)

-- 2.2.0
	- major reorganization of gui/x11*
	(2000/06/06, Eric Lecolinet)

== 2.1
-- 2.1.0
	- overload netstrings by %d/%d, objTYPE, objID++ to keep uniqueness
	- dismiss loop in genaralIntersect (col.c)
	- repare model.c
	- now slide along walls (wall, board, gate and opened door)
	- fix getSeletion with the good camera->near
	- new object moon(#35), moving permanently along a circonference
	(2000/06/03, Philippe Dax)
	- detection of libz (compression) in configure.in
	- payload length in words in the packet if flags field = 1
	  warning: obsoletes previous versions !!!
	- new functions (payload.c): seekPayload, tellStrInPayload, tellPayload
	- bug fixed in http.c if environment variable no_proxy undefined
	- add stat max_packet & min_packet in stat.c
	- repair lift.c
	- declare user geometry to the network
	(2000/05/27, Philippe Dax)

== 2.0
-- 2.0.3
	- change --enable-thread by default from 'yes' to 'no' (more robust)
	  some core dumps occur inside gethostbyname_r()
	- add parsing of preferences (world, geometry) in env.c
	- change float strings to float in user.h
	(2000/05/26, Philippe Dax)

-- 2.0.2
	- new object clip(#34) to play mpeg clips
	- add mpg helpers using mtvp (with audio) or mpeg_play (without audio)
	- change mp3.c to play in streaming mode
	- translate avatar'eyes at the right location
	(2000/05/25, Philippe Dax)
-- 2.0.1
	- update vre script to run vreng on Solaris2.6
	- add bandwidth info in stat.c integrating IP, UDP, RTP headers
	- clock in 3 clock objects (secondes, minutes, hour)
	(2000/05/23, Philippe Dax)

-- 2.0.0 <-- stable, obsoletes previous releases, obsoleted by 3.0.0

	- all threads stuff in thread.c
	(2000/05/22, Philippe Dax)
	- refresh timeout grows as log(nb_sources) to limit bandwidth
	(2000/05/19, Philippe Dax)
	- bug fixed in http.c if no threads
        (2000/05/20, Samuel Tardieu)
	- add object names for aoi, gate, host, doc, cd, clock
	- new object clock(#33)
	(2000/05/17, Philippe Dax)
	- add ray(#32) object
	- add line solid
	- add mirror(#31) object
	- add sphere(#30) object
	- objects.c (from initobj.c & parse.c)
	(2000/05/16, Philippe Dax)
	- now menubar is independent from paths found by configure
	- add include/paths.h for helper applications
	- unlink /tmp/files_wget_downloaded
	- add cd(#29) object to play piece of music
	- add mp3 launcher (mpg123, xaudio, freeamp)
	- add bgcolor(#28) object to define its own background
	- add wheel(#27) solid & object
	- add ring(#26) solid & object
	- add stars(#25) solid & object
	- add panel(#24) object
	(2000/05/10, Philippe Dax)
	- add disk(#23) solid & object
	- bug fixed in parseName
	- add lib directory where lives libvreng.a
	(2000/05/05, Philippe Dax)
	- add --enable-vrengd into configure.in & vrengd variabe into global.c
	- new src directories common/ and server/
	- add --with-qt into configure.in
	- bug fixed in channel.c, pkt.c, proto.c
	(2000/05/02, Philippe Dax)
	- src_id is now my-ssrcid (IPv4/v6 independent), older was my_hostid
	- add functions getChannelbysa, getFdSendRTP, getFdSendRTCP, getSaRTCP
	- bug fixed in RtpCreateSession
	(2000/05/01, Philippe Dax)
	- change payload type from 105 (dynamic) to 65 (not assigned)
	- mc.c functions into channel.c
	- add vreng/inits.[hc] to groups system initializations
	- relooking of rtpsess.c
	(2000/04/24, Philippe Dax)
	- add IPv6 detection into configure.in
	(2000/04/21, Philippe Dax)
	- rename in helpers html.c -> browser.c, vrml.c -> vrbrowser.c
	- add persistency functions into proto.c
	(2000/04/19, Philippe Dax)
	- new Protocol VREP-V2 - change payload header 4 bytes / old 7 bytes
	- change VREP-V2 command from ascii to binary
	- add xmesa.h into src/include/GL (Linux compiling)
	(2000/04/14, Philippe Dax)
	- add code to handle a manager group common to all applications
	- change configuration files suffixes .cfg to .vre
	(2000/04/12, Philippe Dax)
	- move aux/ to support/ (Windows recognizes aux as a logical device :-))
	- add CYGWIN32 in acconfig.h and configure.in
	(2000/03/27, Philippe Dax)
	- hardcoded sin, cos, tan tables
	(2000/03/22, Philippe Dax)
	- split zview.c into solid.c draw.c render.c
	(2000/03/20, Philippe Dax)
	- fix texture.c; url non null terminated
	(2000/03/17, Patrick Bellot)
	- fix drawCylinder (GLU_FILL style)
	- add --enable-thread into configure.in
	(2000/03/16, Philippe Dax)
	- add src subdir where live all the sources
	- move gui/*menu.[ch] in gui/*gui.[hc]
	- move gui/*init.[ch] in gui/*w3d.[hc]
	- move gui/chat.[ch] in gui/*cb.[hc]
	- move gui/members.[ch] in gui/*cb.[hc]
	- add gui/cb.h (call callbacks)
	- split gui/supportfuncs.[ch] into x11cb.[ch], glutcb.[ch], tkcb.[ch]
	(2000/03/13, Philippe Dax)
	- add --with-motif, --with-glut, --with-tcltk in configure.in
	(2000/03/08, Philippe Dax)
	- merge Glut code written by Jean-Francois Karr (30/06/98) into gui/
	(2000/02/28, Philippe Dax)
	- add object cone(#22) in cone.c
	- add solid cylinder/cone in zview.c
	(2000/02/25, Philippe Dax)
	- merge Tcl/Tk code written by Adrien Felon (13/09/99) into gui/
	- add configure.in.tk of vic-1.0ucl from cs.ucl.ac.uk
	(2000/02/21, Philippe Dax)
	- add include/types.h
	- del gui/callbaks.h & gui/wmgtincl.h
	(2000/02/20, Philippe Dax)

++ 1
== 1.6
-- 1.6.5 <-- stable, obsoletes previous releases, obsoleted by 2.0.0

	- #define FPSET_MASK if FREEBSD defined
	(2000/01/14, Philippe Dax)

-- 1.6.4
	- accept compound RTCP packets
	- bug fix in receive RTCP_SDES
	(1999/12/17, Philippe Dax)

-- 1.6.3
	- bug fix in SendDelta confused SEND_RTP -> SOCK_RTP
	(1999/12/10, Philippe Dax)
	- RTP and RTCP fixes
	(1999/12/10, Samuel Tardieu)

-- 1.6.2
	- change initial port number 62666 to 52666 (Linux Mascarade feature)
	- some minor [hn]to[nh]s fixes
	(1999/12/05, Philippe Dax)

-- 1.6.1

	- add entry(#21) pseudo-object to define user origin in a new world
	(1999/12/01, Philippe Dax)
	- rtp.h upgraded with BIG/LITTLE ENDIAN
	- new keys to zoom/unzomm, to tilt left/right
	- fpsetmask detected by configure
	- some hton[sl] fixes
	(1999/11/04, Philippe Dax)

-- 1.6.0 <-- WARNING: this release is uncompatible with 1.5.8 !!!
		  First release RTP/RTCP, previous was UDP

	- new functions to parse objects description (wmgt/parse.c)
	- bug fixed: compile without any browsers
	- add door status (opened/closed/[un]lock) in wmgt/door.[hc]
	- add mime directory in the distribution to launch vreng by the Web
	(1999/10/19, Philippe Dax)
	- nice the vreng process to run at low priority when cpu conflicts
	- new sdr_plugin: sdr2.plugin.dvr.rtp.vre.vreng
	- new structure Channel to manage multiple Network IOs in net/channel.h
	- reduce NetObjectID in order to transfering less datas on the network
	(1999/10/12, Philippe Dax)
	- new functions: getfdbysa(), getsabysa() in net/channel.c
	(1999/10/07, Philippe Dax)
	- RTP/RTCP session integrated from contribution of Vincent Haverlant
	(1999/10/01, Vincent Haverlant and Philippe Dax)
	- split NetInit() into ChannelInit() and ChannelOpen()
	- split newWorld() into quitChannel() and newChannel()
	- bug fixed in ps.c if GVPATH not defined
	- add file FIXES in the distribution
	(1999/09/24, Philippe Dax)
	- add trace function for debugging
	(1999/08/10, Philippe Dax)

== 1.5
-- 1.5.8 <-- stable, obsoletes previous releases, obsoleted by 1.6.0

	- bug fixed in ps.c while link-editing
	(1999/07/24, Philippe Dax)
	- aoi deletion
	(1999/07/19, Adrien Felon)

-- 1.5.7
	- avatars can meet themselves inside the same Aoi, but not outside
	(1999/07/16, Adrien Felon)
	- bug fixed in stat.c (session time)
	(1999/07/16, Philippe Dax)

-- 1.5.6
	- walls deletion done when quitting walls (walls.[hc])
	(1999/07/13, Adrien Felon)
	- repackaging in gui/: log.[hc], chat.[hc], members.[hc]
	(1999/07/13, Philippe Dax)

-- 1.5.5
	- add method print in doc.c and helper a2ps in ps.c
	- bug fix in zview,c about 2D textures
	(1999/07/12, Philippe Dax)

-- 1.5.4
	- some fixes about aoi (move.c, aoi.c)
	(1999/07/09, Adrien Felon)
	- bug fixed in initSignal for Linux
	- add initEnv() to create ~/.vreng/{prefs,cache/}
	- global.[hc] revisted
	- helper ps.c using wget to retreive url and launch gv/ghostview
	(1999/07/09, Philippe Dax)

-- 1.5.3
	- add dart sub-object as an alternative to bullet in rocket object
	- add object doc(#19) to be read by an Postscript or Pdf interpreter
	- mctools/ becomes helpers/, includes/ -> include/, w_mgt -> wmgt/
	- keys.c separated from widget3D.c
	- add argument -v or --version on command line
	- add initsysinfo() in http.c for tracing URLs
	- update html/vreng.html
	(1999/07/07, Philippe Dax)

-- 1.5.2
	- add object aoi(#18) (Aera of Interest) without intersection when
	  we are in and invisible (w_mgt/aoi.[hc])
	(1999/07/05, Adrien Felon)
	- cell is now a solid without visibilty
	(1999/07/04, Philippe Dax)

-- 1.5.1
	- merging debug flags by shift
	(1999/07/02, Adrien Felon)
	- warning replaces fatal if urlcfg doesn't exist (w_mgt/file.c)
	(1999/07/01, Philippe Dax)

-- 1.5.0
	- add cell(#17) object for thin grain multicast areas (w_mgt/cell.*)
	- work on code readability
	- repackaging w_mgt/ includes/
	- repackaging net/, new files, first bricks to re-implement RTP :-)
	- sender packet format RTP capable
	- receiver compatible with current packet header and futur RTP
	- add info ?version=<version> a the end of the URLs (Samuel Tardieu)
	- buf fixed for 'rect' 2D shade in zview.c
	- all theses changes leads to upgrade medium version from 1.4 to 1.5
	(1999/06/29, Philippe Dax)

== 1.4
-- 1.4.11 <-- obsoleted
	- bug fix floating point exception in freeBSD - add fpsetmask(0L)
	- bug fix in mctools/whiteboard.h
	(1999/06/18, Philippe Dax)

-- 1.4.10
	- change name of wrapper (vreng -> vre) for $prefix/bin
	  and in sdr2.plugins
	- Makefile.am for vreng subdir
	- vre script built by configure & vre.in
	(1999/06/15, Philippe Dax, Samuel Tardieu)

-- 1.4.9
	- bug fix text too long (net/net_text.c)
	(1999/06/10, Philippe Dax)

-- 1.4.8
	- add sound directory snd/ in distribution
	(1999/06/10, Philippe Dax)

-- 1.4.7
	- Change X11 resource class name from VReng to Vreng (gui/initUI.*)
	(1999/06/07, Philippe Dax)

-- 1.4.6
	- add script wrapper to launch vreng.{tgl,ogl}
	  because TinyGL dont't find the Visual True Color on Solaris2.7
	  to compile vreng on Solaris2.7 do configure --with-lib-GL
	(1999/06/04, Philippe Dax)

-- 1.4.5
	- bug fix in ttl import
	- bug fix in audio.c (bad ttl args)
	(1999/05/18, Philippe Dax)

-- 1.4.4
	- add ssh (default) alternative to telnet, xterm.[hc]
	(1999/05/07, Philippe Dax)

-- 1.4.3
	- include glu.h was misssing
	(1999/03/01, Philippe Dax)

-- 1.4.2
	- some repackaging
	- some fixes
	(1998/12/12, Philippe Dax)

-- 1.4.0
	- corrected camera coordinates (still some problems, but better)
	- added walls(#20) independant from objects (for the moment, only
	  'grid' walls allowed).
	- added 'MiMaze' simulation (straight walls + rockets)
	(1998/06/30, Lionel Ulmer)

== 1.3
-- 1.3.7 <-- obsoleted
	- added object naming for doors (to allow an object to call a method
	  from another).
	- changed object geometry loading method.
	- added multiple frames for one model (MD2) without reloading it
	- added 'Animator' object
	- added 'Lift' (in fact, a movable step)
	(1998/06/27, Lionel Ulmer)

-- 1.3.6
	- added frames to geometry, changes in parser to have multiple 
	  geometry for one object.
	- add new objet to test frames 'Button'
	(1998/06/25, Lionel Ulmer)

-- 1.3.5
	- added 'scale' parameter to MD2 loading
	- added .MD2 file loading (Quake 2 models format) and .PCX format for
	  images.
	- changed .PCX loading to a 'streaming' load (instead of saving the
	  data in a file before decoding).
	- did the same for .MD2 files.
	(1998/06/24, Lionel Ulmer)

-- 1.3.4
	- bug fix in OpenGL Widget initialization
	- added 'world initialized' and 'zlib initialized' flags to prevent
	  calling a close function before having done the initialisation
	- bug fix in world changing and quitting
	- quality option now works
	- bug fix in selection 
	(1998/05/27, Lionel Ulmer)

-- 1.3.3
	- Open GL/Mesa porting. Use of GLX in the GUI. autoconf fix
	- renaming of the files zlib.h & zlib.c to zview.h & zview.c
	- major update of TinyGL. Separate distribution
	(1998/05/24, Fabrice Bellard, Lionel Ulmer)

-- 1.3.2
	- bugs fix
	(1998/05/24, Fabrice Bellard)
	(1998/03/25, Stephane Belmon)

-- 1.3.1
	- initial group from SDR or shell-command is kept to be used in the
	  future as a supervisor group (multicast addresses allocation)
	- smooth sliding when an avatar intersects with an other one
	- supress automatic refreshing to don't agress the network
	  and add an ACK from already existing avatars to mark their
	  positions when a new avatar come into the arena
	- don't transmit user and ball deplacements in Z delta < 2cm
	- don't transmit ball angular deplacements to don't agress the network
	- add rtp.h from RFC 1889 (RTP/RTCP)
	- use random.c and md5c.c from RFC 1889 to compute SSRC
	- adapt initNetwork() to initialize RTCP sockets
	- adapt incoming() to manage RTCP packets
	- ObjectName struct with ssrcid member (RTP)
	- ObjectHeader struct with a pointer on source state info (RTCP)
	(1998/03/18, Philippe Dax)

-- 1.3.0
	- button Home
	- action shoot a ball, w_mgt_ball.c
	- go automaticaly into a new world when user intersects a gate
	- webspace vrml browser, vrml.[hc]
	- vrweb vrml browser, vrml.[hc]
	- object vrml, w_mgt_vrml.[hc]
	- random initial position for users and balls
	- ball created from config file -> [ don't work now ]
	- delete sender IP-addr/port from payload, use recvfrom() vs recv()
	- new sdr_plugin RTP: sdr2.plugin.S61.dis.rtp.vr.vreng
	- libXaw3d in configure.in
	(1998/03/12, Philippe Dax)

== 1.2
-- 1.2.9 <-- obsoleted
	- when is active, keep audio tool active through worlds
 	- no_proxy variable
	(1998/02/12, Philippe Dax)

-- 1.2.8
 	- button members
	- RTPnames
	(1997/12/19, Philippe Dax)

-- 1.2.7
 	- bug fix in configure.in
	- bug fix if proxy variable empty
	- mainloop, rescan events
	(1997/12/16, Samuel Tardieu)

-- 1.2.6
	- horizontal menubar
	- popup menus to choose or activate tools
	(1997/12/16, Philippe Dax)

-- 1.2.5
	- bug fix while checking X libraries, configure.in
	- button preferences
	(1997/12/12, Philippe Dax)

-- 1.2.4
	- target install_archive_bin, vreng/Makefile.am
	- bug fix while checking X libraries, configure.in
	- add proxy access: env. variable http://proxy:3128/, gui/http.c
	- discard unknown objects & their descriptions in *.cfg, w_mgt_file.c
	- option -d level
	(1997/12/08, Philippe Dax)

-- 1.2.3
	- new object: web, w_mgt_web.[hc]
	- netscape and mMosaic browsers, browser.[hc]
	- add methods: netscape, mMosaic, mailto
	(1997/12/02, Philippe Dax)

-- 1.2.2
	- bug fix on match Visual for color depth=24
	(1997/11/19, Philippe Dax)

-- 1.2.1
	- match Visual for color depth=8
	(1997/11/18, Philippe Dax)

-- 1.2.0
	- new packaging ala GNU
	- automake, autoconf, configure
	(1997/11/10, Philippe Dax)

== 1.1
-- 1.1.3 <-- obsoleted
	- port on Linux Pentium
	- port on Linux Sparc
	- port on NetBSD-1.2 Sparc
	(1997/11/08, Philippe Dax)

-- 1.1.2
	- audio button, gui/int_supportFuncs.c
	- video button, gui/int_supportFuncs.c
	- vic support, mctools/video.[hc]
	- wb support, mctools/wb.[hc]
	- new object: board, w_mgt_board.[hc]
	(1997/11/07, Philippe Dax)

-- 1.1.1
	- enable # comments in world configuation files, w_mgt_files.c
	- initial world configuration builtin, w_mgt/w_cfg.h
	- file TODO
	- file CHANGES
	(1997/11/03, Philippe Dax)

-- 1.1.0
	- add external audio support with fphone, mctools/audio.[hc]
	- new object: host, w_mgt/w_mgt_host.[hc]
	(1997/10/30, Philippe Dax)

== 1.0
-- 1.0.1
	- bug fix about GET HTTP/1.0, gui/int_supportFuncs.c
	(1997/10/22, Philippe Dax)

-- 1.0.0
	- First public release
	- repackaging
	- inherit of initial TTL for the other worlds, mctools/mc.[hc]
	- add external audio support with vat, mctools/audio.[hc]
	(1997/10/16, Philippe Dax)

++ 0
== 0.4
-- 0.4.6 <-- obsoleted
	- original version
	- (May 1997)

== 0.1
-- 0.0.1
	- first version
	- (Nov 1996)
