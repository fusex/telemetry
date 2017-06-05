BIN := fxdecode
BUILDDIR := .build
CFLAGS := -ggdb
.DEFAULT_GOAL := $(BUILDDIR)/$(BIN)

$(BUILDDIR)/$(BIN): $(BIN).c
	@mkdir -p $(BUILDDIR)
	@cc $< -o $@

clean:
	$(RM) -r .build
