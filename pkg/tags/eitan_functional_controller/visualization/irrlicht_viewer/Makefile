.PHONY: install_lib all clean

CPP = g++

# Default Target
all: build_lib viewer clouder

headers = $(filter %.hh %.h, $(1))
noheaders = $(filter-out %.hh %.h, $(1))
nosources = $(filter-out %.cc %.c, $(1))
objects = $(filter %.o, $(1:%.c=%.o) $(1:%.cc=%.o)) $(call noheaders,$(call nosources,$(1)))



CPPFLAGS = $(shell rospack --lang=cpp --attrib=cflags export irrlicht_viewer)
LDFLAGS =  $(shell rospack --lang=cpp --attrib=lflags export irrlicht_viewer)

CUSTOM_NODES = $(shell ls CustomNodes/*.cc)

ILRENDER_LIB = libirrlicht_viewer.a

# Generic Targets
%.o: %.cc
	$(CPP) $(CPPFLAGS) -O2 -c $< -o $@

# Library Targets


$(ILRENDER_LIB):  $(call objects,$(CUSTOM_NODES)) ILRender.o ILClient.o
	ar -rcs $(ILRENDER_LIB) $^

#g++ $(CPPFLAGS) -o $@  $(call objects,$(CUSTOM_NODES))

# Executable Targets
test: ILRenderTest.cc ILRender.o ILClient.o $(ILRENDER_LIB) 
	$(CPP) $(CPPFLAGS) -o $@ $^  $(LDFLAGS)

viewer: viewer.cc $(ILRENDER_LIB) 
	$(CPP) $(CPPFLAGS) -o $@ $^  $(LDFLAGS)

build_lib: $(ILRENDER_LIB)

# Special Targets

clean:
	rm -rf *.o *.a
	rm -rf test viewer clouder
	cd CustomNodes && rm -rf *.o


clouder: cloudGenerator.cc $(ILRENDER_LIB) 
	$(CPP) $(CPPFLAGS) -o $@ $^  $(LDFLAGS)
