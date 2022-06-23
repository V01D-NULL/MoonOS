.PHONY: all clean

all: libk.a ds.a libgraphics.a
	@printf "(LIBS) Built all kernel archive libraries\n";

libk.a:
	@$(MAKE) --no-print-directory -C libk all
	@mv libk/$@ .

# data structures
ds.a:
	@$(MAKE) --no-print-directory -C ds all
	@mv ds/$@ .	

libgraphics.a:
	@$(MAKE) --no-print-directory -C libgraphics all
	@mv libgraphics/$@ .	

clean:
	@rm -f libk.a ds.a libgraphics.a
	@$(MAKE) --no-print-directory -C libk clean
	@$(MAKE) --no-print-directory -C ds clean
	@$(MAKE) --no-print-directory -C libgraphics clean