define BUILD_C_OBJECT
	$(call CHECK_REQUIRED_PATHS)
	$(call is_set,$(1),First argument: Source file missing)
	$(call is_set,$(2),Second argument: Output object file missing)
	$(Q)$(CC) -std=c99 $(CFLAGS) $(CUSTOM_CFLAGS) $(CFLAGS_$(2)) -c $(1) -o $(2)
	@echo 'CC  $(2)'
endef
