.PHONY: all clean

all: libk.a liballoc.a
	@printf "(LIBS) Built all kernel archive libraries\n";

libk.a:
	@$(MAKE) --no-print-directory -C libk all
	@mv libk/$@ .

liballoc.a:
	@$(MAKE) --no-print-directory -C liballoc all
	@mv liballoc/$@ .

clean:
	@rm -f libk.a liballoc.a
	@$(MAKE) --no-print-directory -C libk clean