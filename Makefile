CC := gcc

SUBDIRS := $(patsubst %/,%,$(wildcard src/*/))
TEST_SRCS := $(wildcard src/*/test-*.c)
TEST_BINS := $(TEST_SRCS:.c=)

.PHONY: all clean module test $(SUBDIRS)

all: module test

module: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

test: $(TEST_BINS)

$(TEST_BINS): %: %.c
	$(CC) -o $@ $<

clean:
	for d in $(SUBDIRS); do $(MAKE) -C $$d clean; done
	$(RM) $(TEST_BINS)