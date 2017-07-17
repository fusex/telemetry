BIN1 := fxdecode
BIN2 := serialread
BIN3 := fxhost

BUILDDIR := .build
CFLAGS := -ggdb
.DEFAULT_GOAL := all

all: $(BUILDDIR)/$(BIN1) $(BUILDDIR)/$(BIN2) $(BUILDDIR)/$(BIN3)

$(BUILDDIR)/%: %.c
	@mkdir -p $(BUILDDIR)
	cc $< -o $@

clean:
	$(RM) -r .build
