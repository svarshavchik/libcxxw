headerdir=$(includedir)/libcxxw-@ABIVER@-@LIBCXX_VERSION@/x/w

SUBDIRS=deps impl metrics

header_HEADERS = $(shell cd $(srcdir) && echo *.H)

EXTRA_HEADERS =

APPEARANCE_STYLESHEETS = $(shell cd $(srcdir)/../../../appearance && echo *_appearance.xml)

APPEARANCE_XML=$(patsubst %.xml,%.inc.H,$(APPEARANCE_STYLESHEETS))

BUILT_SOURCES=$(APPEARANCE_XML)

dist-hook:
	rm -f $(distdir)/*_inc.H

CLEANFILES = $(shell cd $(srcdir) && echo *_inc.H) $(APPEARANCE_XML)

$(APPEARANCE_XML): %.inc.H: ../../../appearance/%.xml ../../../appearance/create_headers.xsl
	xsltproc $(srcdir)/../../../appearance/create_headers.xsl $(srcdir)/$(<) >$@.tmp && mv -f $@.tmp $@
