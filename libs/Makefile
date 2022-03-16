.PHONY: all clean

all: libk.a liballoc.a ds.a libgraphics.a userspace.a
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

libgraphics.a:
	@$(MAKE) --no-print-directory -C libgraphics all
	@mv libgraphics/$@ .	

userspace.a:
	@$(MAKE) --no-print-directory -C userspace all
	@mv userspace/$@ .

clean:
	@rm -f libk.a liballoc.a ds.a libgraphics.a
	@$(MAKE) --no-print-directory -C libk clean
	@$(MAKE) --no-print-directory -C liballoc clean
	@$(MAKE) --no-print-directory -C ds clean
	@$(MAKE) --no-print-directory -C libgraphics clean