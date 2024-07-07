# =========================================================================
#   Unity - A Test Framework for C
#   ThrowTheSwitch.org
#   Copyright (c) 2007-24 Mike Karlesky, Mark VanderVoord, & Greg Williams
#   SPDX-License-Identifier: MIT
# =========================================================================
-include ../../Make.conf

CLEANUP = rm -f
MKDIR = mkdir -p
TARGET_EXTENSION=out

UNITY_ROOT=deps/Unity

CFLAGS=-std=c11
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition
CFLAGS += -DUNITY_OUTPUT_COLOR 
CFLAGS += -I ../../
CFLAGS += -ffreestanding

# Find all test files
TEST_FILES=$(shell find src -name '*.test.c')
TEST_RUNNERS=$(patsubst src/%.test.c,src/test_runners/%.test.runner.c,$(TEST_FILES))
TEST_TARGETS=$(patsubst src/%.test.c,build/%.$(TARGET_EXTENSION),$(TEST_FILES))

# Include Unity source
UNITY_SRC=$(UNITY_ROOT)/src/unity.c

INC_DIRS = 				 \
	-Isrc 				 \
	-I$(UNITY_ROOT)/src  \
	-I ../.. 			 \
	-I ../../libs/		 \
	-I ../../kernel/	 \
	-I ../../kernel/arch/$(ARCH)	 \
	-I ../../kernel/platform/$(BOARD)/$(ARCH)	 \

SYMBOLS=

all: clean default

default: $(TEST_TARGETS)
	@echo "All tests built successfully."
	@echo "Run individual tests or use 'make run_all' to run all tests."

build/%.$(TARGET_EXTENSION): src/%.test.c src/test_runners/%.test.runner.c $(UNITY_SRC)
	@$(MKDIR) $(dir $@)
	$(eval PROD_SRC = $(wildcard ../../$(patsubst src/%.test.c,%.c,$<)))
	$(CC) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $< $(PROD_SRC) $(UNITY_SRC) src/test_runners/$*.test.runner.c -o $@


src/test_runners/%.test.runner.c: src/%.test.c
	@$(MKDIR) $(dir $@)
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb $< $@

run_all: $(TEST_TARGETS)
	@for test in $(TEST_TARGETS); do \
		echo "Running $$test"; \
		./$$test; \
	done

clean:
	$(CLEANUP) -r build
	$(CLEANUP) -r src/test_runners

ci: CFLAGS += -Werror
ci: run_all

.PHONY: all default run_all clean ci