SUBPROJECTS :=  \
	base \
	rocket \
	host/simuvol \
	host/genpack \
	host/logger \
	host

all:
	$(foreach SUBPRJ,$(SUBPROJECTS), make -C $(SUBPRJ);)

clean:
	$(foreach SUBPRJ,$(SUBPROJECTS), make -C $(SUBPRJ) $@;)

$(SUBPROJECTS):
	make -C $@

.PHONY: $(SUBPROJECTS)
