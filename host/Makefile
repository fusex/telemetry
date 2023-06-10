BIN := fxhost

BUILDDIR := .build
CFLAGS := -Wall -ggdb -std=c++11 -pthread -I. -D_IS_HOST
.DEFAULT_GOAL := all
LDFLAGS := -lm -lpthread
LD := g++
CXX := g++

COMMON := $(BUILDDIR)/logger.o $(BUILDDIR)/trame.o
# $(BUILDDIR)/fusexutil.o

all: $(BUILDDIR)/$(BIN)

$(BUILDDIR)/$(BIN): $(COMMON) $(BUILDDIR)/$(BIN).o
	$(LD) $^ $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: %.cpp 
	@mkdir -p $(BUILDDIR)
	cc $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r .build
