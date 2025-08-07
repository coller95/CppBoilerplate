# Main Makefile for running all unit tests in subdirectories

SUBDIRS := $(shell find . -maxdepth 1 -type d -not -path '.' -exec test -e '{}/Makefile' \; -print | sed 's|^\./||')

.PHONY: all clean test $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	for dir in $(SUBDIRS); do \
	  $(MAKE) -C $$dir clean; \
	done

test: all
	@echo "Running all unit tests..."
	for dir in $(SUBDIRS); do \
	  for bin in $$dir/bin/*Test; do \
		if [ -x "$$bin" ]; then \
		  echo "Running $$bin"; \
		  ./$$bin; \
		fi; \
	  done; \
	done
