all: propgrid

VERSION = propgrid-1.4.0
WXPROPGRID_ROOT = $(shell rospack find wxpropgrid)

$(VERSION)/Makefile: $(VERSION)/Makefile.in $(VERSION)/wg_changelist.txt $(VERSION)/configure
	cd $(VERSION) && \
		export CFLAGS='-g -D__NOTWXPYTHON__ -g' && \
		export CXXFLAGS='-g -D__NOTWXPYTHON__ -g' && \
		./configure --prefix=$(WXPROPGRID_ROOT)/propgrid_install

$(VERSION): $(VERSION)/Makefile
	cd $(VERSION) && \
		make && \
		make install

propgrid: $(VERSION) 

clean:
	cd $(VERSION) && \
		make clean && \
		rm -f Makefile
	rm -rf propgrid_install

wipe: clean

.PHONY: clean
