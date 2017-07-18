BIN1 := fxdecode
BIN2 := serialread
BIN3 := fxhost

BUILDDIR := .build
CFLAGS := -ggdb
.DEFAULT_GOAL := all
LDFLAGS := -lm
LD := cc

COMMON := $(BUILDDIR)/trame.o $(BUILDDIR)/fusexutil.o

all: $(BUILDDIR)/$(BIN1) $(BUILDDIR)/$(BIN2) $(BUILDDIR)/$(BIN3)

$(BUILDDIR)/$(BIN3): $(COMMON) $(BUILDDIR)/$(BIN3).o
	$(LD) $^ $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: %.c 
	@mkdir -p $(BUILDDIR)
	cc $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r .build
