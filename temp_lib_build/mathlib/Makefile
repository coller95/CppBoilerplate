include config.mk

CC = gcc
CFLAGS = -fPIC -Wall -O2 -Iinclude
LDFLAGS = -shared
TARGET = libmathlib.so
SRCDIR = src
OBJDIR = obj
OBJS = $(OBJDIR)/mathlib.o
PREFIX ?= /usr/local

all: $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/mathlib.o: $(SRCDIR)/mathlib.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

install: $(TARGET)
	install -d $(PREFIX)/lib $(PREFIX)/include
	install -m 755 $(TARGET) $(PREFIX)/lib/
	install -m 644 include/mathlib.h $(PREFIX)/include/

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all install clean
