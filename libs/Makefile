.PHONY: all clean

all: libk.a
	@printf "[LIBS] Built all kernel archive libraries\n";

libk.a:
	$(MAKE) --no-print-directory -C libk all
	@mv libk/$@ .

clean:
	@rm -f libk.a
	@$(MAKE) --no-print-directory -C libk clean