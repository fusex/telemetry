BIN      := genpack
BUILDDIR := .build
CXXFLAGS := -Wall -ggdb -std=c++11 -pthread -I..
.DEFAULT_GOAL := all
LD := g++
CXX := g++

COMMON := $(BUILDDIR)/trame.o $(BUILDDIR)/fusexutil.o

all: $(BUILDDIR)/$(BIN)

$(BUILDDIR)/trame.o: ../trame.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(BUILDDIR)/fusexutil.o: ../fusexutil.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(BUILDDIR)/$(BIN).o: genpack.cpp 
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(BUILDDIR)/$(BIN): $(BUILDDIR)/$(BIN).o $(COMMON) 
	$(LD) $^ $(LDFLAGS) -o $@

clean:
	$(RM) -r .build
