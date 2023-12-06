IDIR := src
SRCDIR := src
ODIR := obj
LDIR :=

BLACKTDIR = ../blackt
LIBDISCASTERDIR = ../libdiscaster

AR=ar
CXX=g++
# Compile only
CDEFINES = 
#CLIBS = -lpng
CFLAGS = -std=gnu++11 -O2 -Wall -L${LIBDISCASTERDIR} -ldiscaster -L${BLACKTDIR} -lblackt
CINCLUDES = -I${BLACKTDIR}/src -I${LIBDISCASTERDIR}/src
CXXFLAGS=$(CFLAGS) $(CDEFINES) $(CINCLUDES) -I$(IDIR)

LIBDEPS := $(LIBDISCASTERDIR)/libdiscaster.a $(BLACKTDIR)/libblackt.a

TOOLSRCS := $(wildcard $(SRCDIR)/*.cpp)
TOOLSINDIR := $(patsubst %.cpp,%,$(TOOLSRCS))
TOOLS := $(notdir $(TOOLSINDIR))

.SECONDEXPANSION:

all: $(BLACKTDIR)/libblackt.a $(LIBDISCASTERDIR)/libdiscaster.a $(TOOLS)

$(BLACKTDIR)/libblackt.a: $(BLACKTDIR)/src/**/*.cpp
	make -C ${BLACKTDIR} all

$(LIBDISCASTERDIR)/libdiscaster.a: $(LIBDISCASTERDIR)/src/**/*.cpp
	make -C ${LIBDISCASTERDIR} all

$(TOOLS): $(SRCDIR)/$$@.cpp $(LIBDEPS) $(BLACKTDIR)/libblackt.a
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $(notdir $@) $(CXXFLAGS)

.PHONY: cleanme clean

cleanme:
	rm -f $(TOOLS)

clean: cleanme
#	rm -f $(LIB)
#	rm -rf $(ODIR)
	make -C ${BLACKTDIR} clean
	make -C ${LIBDISCASTERDIR} clean
