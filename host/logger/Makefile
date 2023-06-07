.DEFAULT_GOAL := all
BIN      := logger
BUILDDIR := .build
CXXFLAGS := -Wall -ggdb -std=c++11 -pthread 
LD := g++
CXX := g++

all: $(BUILDDIR)/$(BIN)

$(BUILDDIR)/$(BIN) : $(BIN).cpp main.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) -r .build
