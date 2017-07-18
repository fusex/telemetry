#BIN1 := fxdecode
#BIN2 := serialread
BIN3 := fxhost

BUILDDIR := .build
CFLAGS := -ggdb -std=c++11 -pthread
.DEFAULT_GOAL := all
LDFLAGS := -lm -lpthread
LD := g++
CXX := g++

COMMON := $(BUILDDIR)/logger.o $(BUILDDIR)/trame.o $(BUILDDIR)/fusexutil.o

#all: $(BUILDDIR)/$(BIN1) $(BUILDDIR)/$(BIN2) $(BUILDDIR)/$(BIN3)
all: $(BUILDDIR)/$(BIN3)

$(BUILDDIR)/$(BIN3): $(COMMON) $(BUILDDIR)/$(BIN3).o
	$(LD) $^ $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: %.cpp 
	@mkdir -p $(BUILDDIR)
	cc $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r .build
