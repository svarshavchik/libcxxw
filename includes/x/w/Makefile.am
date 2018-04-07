headerdir=$(includedir)/x/w

SUBDIRS=deps impl metrics

header_HEADERS = $(shell cd $(srcdir) && echo *.H)

EXTRA_HEADERS =

dist-hook:
	rm -f $(distdir)/*_inc.H

CLEANFILES = $(shell cd $(srcdir) && echo *_inc.H)
