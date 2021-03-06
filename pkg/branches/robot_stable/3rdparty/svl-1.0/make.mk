# MAKE INCLUDE FILE FOR STAIR VISION LIBRARY
# Stephen Gould <sgould@stanford.edu>

#######################################################################
# DO NOT EDIT THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING. IF YOU
# WANT TO OVERRIDE ANY OF THESE SETTINGS DO SO IN YOUR make.local FILE.
#######################################################################

OS = $(shell uname)
BIN_PATH = $(LASIK_PATH)/bin
EXT_PATH = $(LASIK_PATH)/external
INC_PATH = $(LASIK_PATH)/include
SVL_PATH = $(LASIK_PATH)/svl

# force 32-bit compile (set to 1 in make.local)
FORCE32BIT = 0
ifeq (,$(findstring x86_64,$(MACHTYPE)))
  FORCE32BIT = 1
endif

# matlab build (set to 1 in make.local)
BUILD_MEX_APPS = 0

# use shared libraries (set to 1 in make.local)
SVL_SHARED_LIBS = 0

# platform flags
PLAT_CFLAGS = -D__LINUX__
PLAT_LFLAGS = 

OPENGL_LFLAGS = -lGL -lGLU
JPEG_LFLAGS = -ljpeg

# link to OpenCV should be in external/opencv
OPENCV_CFLAGS = -I$(EXT_PATH)/opencv/include/opencv
OPENCV_LFLAGS = -L$(EXT_PATH)/opencv/lib -lcv -lhighgui -lcxcore -lml

# link to wxWidgets should be in external/wxGTK
WXDIR := $(EXT_PATH)/wxGTK
WX_CFLAGS = `$(WXDIR)/buildgtk/wx-config --cflags`
WX_LFLAGS = `$(WXDIR)/buildgtk/wx-config --libs core,base,gl`

# link to include/boost should be in external/boost_headers
BOOST_CFLAGS = -I$(EXT_PATH)/boost_headers

# initialize extra cflags
EXTRA_CFLAGS = -I$(INC_PATH) -I$(EXT_PATH)

# include local user make.local to override any settings
ifeq ($(wildcard $(LASIK_PATH)/make.local), $(LASIK_PATH)/make.local)
  -include $(LASIK_PATH)/make.local
endif

# extra compiler and linker flags
ifeq ($(USE_OPENGL), 1)
  EXTRA_LFLAGS += $(OPENGL_LFLAGS)
endif

ifeq ($(USE_OPENCV), 1)
  EXTRA_CFLAGS += $(OPENCV_CFLAGS)
  EXTRA_LFLAGS += $(OPENCV_LFLAGS) $(JPEG_LFLAGS)
endif

ifeq ($(USE_LIBJPEG), 1)
  EXTRA_LFLAGS += $(JPEG_LFLAGS)
endif

ifeq ($(USE_WX), 1)
  EXTRA_CFLAGS += $(WX_CFLAGS)
  EXTRA_LFLAGS += $(WX_LFLAGS)
endif

ifeq ($(USE_BOOST), 1)
  EXTRA_CFLAGS += $(BOOST_CFLAGS)
endif

# SVL libraries
STATICSVL = $(BIN_PATH)/libsvlBase.a $(BIN_PATH)/libsvlDevel.a \
	$(BIN_PATH)/libsvlDeprecated.a $(BIN_PATH)/libsvlML.a \
	$(BIN_PATH)/libsvlPGM.a $(BIN_PATH)/libsvlVision.a

SHAREDSVL = $(BIN_PATH)/libsvlBase.so.1.0.0 $(BIN_PATH)/libsvlDevel.so.1.0.0 \
	$(BIN_PATH)/libsvlDeprecated.so.1.0.0 $(BIN_PATH)/libsvlML.so.1.0.0 \
	$(BIN_PATH)/libsvlPGM.so.1.0.0 $(BIN_PATH)/libsvlVision.so.1.0.0

EXTLIB = $(EXT_PATH)/xmlParser/xmlParser.a $(EXT_PATH)/lbfgs/lbfgs.a

ifeq ($(SVL_SHARED_LIBS), 0)
  LIBSVL = $(STATICSVL)
else
  LIBSVL = $(SHAREDSVL)
  EXTRA_LFLAGS += -Wl,-rpath,$(BIN_PATH) -L$(BIN_PATH) \
	-lsvlBase -lsvlDevel -lsvlDeprecated -lsvlML -lsvlPGM -lsvlVision
endif

# compiler and linker flags
ifeq ($(FORCE32BIT), 0)
  CFLAGS = -g -O3 -fPIC $(PLAT_CFLAGS) $(EXTRA_CFLAGS) -Wall
  LFLAGS = -g -lm -lpthread $(PLAT_LFLAGS) $(EXTRA_LFLAGS) $(LIBSVL) $(EXTLIB)
else
  CFLAGS = -g -O3 -m32 $(PLAT_CFLAGS) $(EXTRA_CFLAGS) -Wall
  LFLAGS = -g -lm -m32 -lpthread $(PLAT_LFLAGS) $(EXTRA_LFLAGS) $(LIBSVL) $(EXTLIB)
endif

#CFLAGS += -pg
#LFLAGS += -pg
CCC = g++
OBJ = $(SRC:.cpp=.o)

