AUTOMAKE_OPTIONS = foreign no-dependencies

man_MANS = vreng.1 vacs.8 vaps.8 vrel.1 vrl.1 vred.1

EXTRA_DIST = vreng.1 vacs.8 vaps.8 vrel.1 vrl.1 vred.1

TOP=$(top_srcdir)

SUFFIXES: .man .1 .8

.1.man:
	@man ./$<.1 >$(TOP)/$<.man
.8.man:
	@man ./$<.8 >$(TOP)/$<.man

all: vreng vred vrel vrl vacs vaps

vreng:
	@man ./vreng.1 >$(TOP)/vreng.man
vred:
	@man ./vred.1 >$(TOP)/vred.man
vrel:
	@man ./vrel.1 >$(TOP)/vrel.man
vrl:
	@man ./vrl.1 >$(TOP)/vrl.man
vacs:
	@man ./vacs.8 >$(TOP)/vacs.man
vaps:
	@man ./vaps.8 >$(TOP)/vaps.man

clean:
	@-rm $(TOP)/*.man
