# TODO: integrate with ROS build system
CC = g++
CFLAGS = -c -g -Wall
# OpenCV
CV_LATEST = $(shell rospack find opencv_latest)/opencv
CFLAGS += -I$(CV_LATEST)/include/opencv
LDFLAGS = -L$(CV_LATEST)/lib -lcv -lhighgui
# Boost
CFLAGS += -I$(BOOST_ROOT)/include
LDFLAGS += -L$(BOOST_ROOT)/lib -lboost_program_options-gcc42-mt -lboost_filesystem-gcc42-mt
#LDFLAGS += -lboost_thread-gcc42-mt
# Descriptor
CALONDER = $(shell rospack find calonder_descriptor)
CFLAGS += -I/usr/include -I$(CALONDER)/include
LDFLAGS += -L$(CALONDER)/lib -lcalonder
# Star detector
STAR_DIR = $(shell rospack find star_detector)
CFLAGS += -I/usr/include -I$(STAR_DIR)/include -I$(STAR_DIR)/src/test
LDFLAGS += -L$(STAR_DIR)/lib -lstarfeature
# GSL
#CFLAGS += -DHAVE_GSL
#LDFLAGS += -L/usr/lib -lgsl -lgslcblas
# SIFT (get rid of this?)
CFLAGS += -I$(SIFT_ROOT)
LDFLAGS += -L$(SIFT_ROOT)/lib -lsift
# Optimization settings
CFLAGS += -O3 -DNDEBUG
#CFLAGS += -march=native
#CFLAGS += -mfpmath=sse
#CFLAGS += -ffast-math
#CFLAGS += -fomit-frame-pointer
#CFLAGS += -fno-rtti
#CFLAGS += -O0

SOURCES = detectors.cpp
OBJECTS = $(SOURCES:.cpp=.o)
PROGRAMS = baseset_test recognition_test show_base_set patch_test
PROGRAMS += match_benchmark
#PROGRAMS += matcher_test rtree_test classifier_test write_posteriors

all: $(PROGRAMS)

.SECONDEXPANSION:

$(PROGRAMS): $$@.o $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $< -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(PROGRAMS) *.o
