PACKAGE = flex
LINGUAS = da es fr ko ru sv

MSGFMT = msgfmt
XGETTEXT = xgettext
MSGMERGE = msgmerge

LOCALEDIR = $(prefix)/share/locale

POTFILES =		\
	../ccl.c	\
	../dfa.c	\
	../ecs.c	\
	../gen.c	\
	../initscan.c	\
	../libmain.c	\
	../libyywrap.c	\
	../main.c	\
	../misc.c	\
	../nfa.c	\
	../skel.c	\
	../sym.c	\
	../tblcmp.c	\
	../yylex.c

.SUFFIXES: .po .mo .pot

%.mo: %.po
	$(MSGFMT) -o $@ $<

%.pox: %.po $(PACKAGE).pot
	$(MSGMERGE) -o $@ $^

MO = $(addsuffix .mo,$(LINGUAS))
POX = $(addsuffix .pox,$(LINGUAS))


all: $(MO)
update: $(POX)

$(PACKAGE).pot: $(POTFILES)
	$(XGETTEXT) -k_ -o $@ $^

install: $(MO)
	for i in $(MO) ; do \
	  t=$(DESTDIR)/$(LOCALEDIR)/`basename $$i .mo`/LC_MESSAGES ;\
	  install -d $$t ;\
	  install -m 644 $$i $$t/$(PACKAGE).mo ;\
	done

clean:
	$(RM) $(MO) *~ $(DOMAIN).pot
