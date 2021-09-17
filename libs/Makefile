.PHONY: all clean

all: libk.a liballoc.a ds.a
	@printf "(LIBS) Built all kernel archive libraries\n";

libk.a:
	@$(MAKE) --no-print-directory -C libk all
	@mv libk/$@ .

liballoc.a:
	@$(MAKE) --no-print-directory -C liballoc all
	@mv liballoc/$@ .

# data structures
ds.a:
	@$(MAKE) --no-print-directory -C ds all
	@mv ds/$@ .	

clean:
	@rm -f libk.a liballoc.a ds.a
	@$(MAKE) --no-print-directory -C libk clean