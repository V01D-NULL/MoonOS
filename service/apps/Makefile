dirs := $(wildcard */)

all:
	@for dir in $(dirs); do \
		$(MAKE) --no-print-directory -C $$dir; \
	done

clean:
	@for dir in $(dirs); do \
		$(MAKE) --no-print-directory -C $$dir clean; \
	done