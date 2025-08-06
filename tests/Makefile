# Main Makefile for running all unit tests in subdirectories

SUBDIRS := printHello

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
	  if [ -x $$dir/bin/printHelloTest ]; then \
		 echo "Running $$dir/bin/printHelloTest"; \
		 ./$$dir/bin/printHelloTest; \
	  fi; \
	done
