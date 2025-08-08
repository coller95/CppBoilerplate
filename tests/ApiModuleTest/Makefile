# Modern, robust Makefile for ApiModule unit tests (Google Test)
# All object and dependency files are placed flat in obj/ (not nested)

CXX = g++
CC = gcc
CXXFLAGS = -std=c++20 -I../../include -I../../external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
CFLAGS = -I../../include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L../../external/googletest/build/lib -lgtest -lgtest_main

OBJDIR = obj
BINDIR = bin

TEST_SRC = cases/*.cpp ../../src/ApiModule/ApiModules.cpp ../../src/EndpointHello/EndpointHello.cpp
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/ApiModules.o $(OBJDIR)/EndpointHello.o
TEST_DEPS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(notdir $(basename $(wildcard cases/*.cpp)))) $(OBJDIR)/ApiModules.d $(OBJDIR)/EndpointHello.d
TEST_BIN = $(BINDIR)/ApiModuleTest

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Pattern rule for C++ test cases in cases/
$(OBJDIR)/%.o: cases/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rule for sources outside test dir
$(OBJDIR)/ApiModules.o: ../../src/ApiModule/ApiModules.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(OBJDIR)/EndpointHello.o: ../../src/EndpointHello/EndpointHello.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for C sources (if any)
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

-include $(TEST_DEPS)

clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
