include config.mk

CC = gcc
CFLAGS = -Wall -O2 -Iinclude
AR = ar
ARFLAGS = rcs
TARGET = libstringutils.a
SRCDIR = src
OBJDIR = obj
OBJS = $(OBJDIR)/stringutils.o
PREFIX ?= /usr/local

all: $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/stringutils.o: $(SRCDIR)/stringutils.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

install: $(TARGET)
	install -d $(PREFIX)/lib $(PREFIX)/include
	install -m 644 $(TARGET) $(PREFIX)/lib/
	install -m 644 include/stringutils.h $(PREFIX)/include/

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all install clean
