BIN := fxhost

BUILDDIR := .build
CFLAGS := -Wall -ggdb -pthread -I. -D_IS_HOST -ffixed-point
CXXFLAGS := -Wall -ggdb -std=c++11 -pthread -I. -D_IS_HOST
.DEFAULT_GOAL := all
LDFLAGS := -lm -lpthread
CC := clang
LD := clang++

COMMON := $(BUILDDIR)/logger.o $(BUILDDIR)/trame.o $(BUILDDIR)/real.o
# $(BUILDDIR)/fusexutil.o

all: $(BUILDDIR)/$(BIN)

$(BUILDDIR)/$(BIN): $(COMMON) $(BUILDDIR)/$(BIN).o
	$(LD) $^ $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: %.c*
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r .build
