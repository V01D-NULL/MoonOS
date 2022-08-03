.PHONY: all clean

all: base.a moon-ds.a moon-io.a
	@printf "(LIBS) Built all kernel archive libraries\n";

base.a:
	@$(MAKE) --no-print-directory -C base all
	@mv base/$@ .

# data structures
moon-ds.a:
	@$(MAKE) --no-print-directory -C moon-ds all
	@mv moon-ds/$@ .	

moon-io.a:
	@$(MAKE) --no-print-directory -C moon-io all
	@mv moon-io/$@ .	

clean:
	@rm -f base.a moon-ds.a moon-io.a:
	@$(MAKE) --no-print-directory -C base clean
	@$(MAKE) --no-print-directory -C moon-ds clean
	@$(MAKE) --no-print-directory -C moon-io clean
