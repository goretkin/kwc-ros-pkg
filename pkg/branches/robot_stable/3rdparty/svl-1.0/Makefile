# STAIR VISION LIBRARY MAKEFILE
# Stephen Gould <sgould@stanford.edu>

#######################################################################
# DO NOT EDIT THIS MAKEFILE UNLESS YOU KNOW WHAT YOU ARE DOING. EDIT
# THE APPROPRIATE MAKEFILE IN THE PROJECT OR SANDBOX DIRECTORY.
#######################################################################

.PHONY: clean external svllibs svlapps

all: depend external svllibs svlapps

external: external/lbfgs/*.cpp external/xmlParser/*.cpp
	cd external/lbfgs && make $(PARALLEL_JOBS) && cd ../..
	cd external/xmlParser && make $(PARALLEL_JOBS) && cd ../..

svlapps:
	cd svl/apps && make $(PARALLEL_JOBS) && cd ../..

svllibs:
	cd svl/lib && make $(PARALLEL_JOBS) && cd ../..

depend:
	-mkdir bin
	cd svl/lib && make depend && cd ../..
	cd svl/apps && make depend && cd ../..

clean:
	cd external/lbfgs && make clean && cd ../..
	cd external/xmlParser && make clean && cd ../..
	cd svl/lib && make clean && cd ../..
	cd svl/apps && make clean && cd ../..
	find -name "*~" -delete
	cd bin && rm -f * && cd ..
