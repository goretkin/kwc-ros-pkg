all: build

SVN_DIR = plplot-svn
SVN_URL = https://plplot.svn.sf.net/svnroot/plplot/trunk 
SVN_REVISION = -r 8746
include $(shell rospack find mk)/svn_checkout.mk

CMAKE = `rospack find cmake`/cmake/bin/cmake 
CMAKE_ARGS = -DCMAKE_INSTALL_PREFIX=$(PWD)/plplot \
	-DPLD_wxwidgets:BOOL=ON

build: SVN_UP plplot

configured: $(SVN_DIR) Makefile
	cd $(SVN_DIR) && mkdir -p build
	cd $(SVN_DIR)/build &&  $(CMAKE) $(CMAKE_ARGS) ..
	touch configured

.PHONY: plplot
plplot: configured
	cd $(SVN_DIR)/build && make install

clean:
	rm -rf $(SVN_DIR)/build
	rm -rf plplot configured
wipe: clean
	rm -rf plplot-svn
