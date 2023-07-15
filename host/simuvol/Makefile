.DEFAULT_GOAL := all
BIN      := simuvol
BUILDDIR := .build
CXXFLAGS := -Wall -ggdb 
LD := gcc
CXX := gcc

all: $(BUILDDIR)/$(BIN)

$(BUILDDIR)/$(BIN) : $(BIN).c
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) -r .build
