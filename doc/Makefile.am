help2man = @HELP2MAN@

info_TEXINFOS =	flex.texi
man_MANS = flex.1

MAINTAINERCLEANFILES = \
	flex.1

CLEANFILES = \
	flex.hks \
	flex.ops

EXTRA_DIST = \
	$(man_MANS)

@MAINTAINER_MODE_TRUE@$(man_MANS): $(top_srcdir)/main.c
@MAINTAINER_MODE_TRUE@	for i in $(man_MANS) ; do \
@MAINTAINER_MODE_TRUE@	$(help2man) --name='fast lexical analyzer generator' --section=`echo $$i|rev|cut -f 1 -d .|rev` ../flex > $$i ; \
@MAINTAINER_MODE_TRUE@	done
