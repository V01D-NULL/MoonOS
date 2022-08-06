###
# Todo: Come up with a better way of doing this. Hardcoding the archives is just..no.
#

.PHONY: all clean

all: base.a moon-ds.a moon-io.a moon-sys.a
	@printf "(LIBS) Built all kernel archive libraries\n\n";

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

moon-sys.a:
	@$(MAKE) --no-print-directory -C moon-sys all
	@mv moon-sys/$@ .

clean:
	@rm -f base.a moon-ds.a moon-io.a moon-sys.a
	@$(MAKE) --no-print-directory -C base clean
	@$(MAKE) --no-print-directory -C moon-ds clean
	@$(MAKE) --no-print-directory -C moon-io clean
	@$(MAKE) --no-print-directory -C moon-sys clean
