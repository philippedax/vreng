AUTOMAKE_OPTIONS = foreign no-dependencies

man_MANS = vreng.1 vacs.8 vaps.8 vrel.1 vrl.1 vred.1

EXTRA_DIST = vreng.1 vacs.8 vaps.8 vrel.1 vrl.1 vred.1

TOP=$(top_srcdir)
MAN=$(TOP)/man

SUFFIXES: .man .1 .8

.1.man:
	@man ./$<.1 >$(MAN)/$<.man
.8.man:
	@man ./$<.8 >$(MAN)/$<.man

all: man

man:
	@-mkdir $(MAN) >/dev/null 2>&1
	@man ./vreng.1 >$(MAN)/vreng.man
	@man ./vred.1 >$(MAN)/vred.man
	@man ./vrel.1 >$(MAN)/vrel.man
	@man ./vrl.1 >$(MAN)/vrl.man
	@man ./vacs.8 >$(MAN)/vacs.man
	@man ./vaps.8 >$(MAN)/vaps.man

clean:
	@-rm $(MAN)/*.man
