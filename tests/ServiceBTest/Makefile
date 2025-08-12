# Modern, robust Makefile for ServiceB unit tests (Google Test)
# All object and dependency files are placed flat in obj/ (not nested)

ROOTDIR = ../..
CXX = g++
CC = gcc
CXXFLAGS = -std=c++20 -I$(ROOTDIR)/include -I$(ROOTDIR)/external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
CFLAGS = -I$(ROOTDIR)/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L$(ROOTDIR)/external/googletest/build/lib -lgtest -lgtest_main

OBJDIR = obj
BINDIR = bin
SERVICE = ServiceB

TEST_SRC = cases/*.cpp TestMain.cpp $(ROOTDIR)/src/$(SERVICE)/$(SERVICE).cpp $(ROOTDIR)/src/IoCContainer/IoCContainer.cpp
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/TestMain.o $(OBJDIR)/$(SERVICE).o $(OBJDIR)/IoCContainer.o
TEST_DEPS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(notdir $(basename $(wildcard cases/*.cpp)))) $(OBJDIR)/TestMain.d $(OBJDIR)/$(SERVICE).d $(OBJDIR)/IoCContainer.d
TEST_BIN = $(BINDIR)/$(SERVICE)Test

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Pattern rule for C++ test cases in cases/
$(OBJDIR)/%.o: cases/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for the test runner
$(OBJDIR)/TestMain.o: TestMain.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rule for service source
$(OBJDIR)/$(SERVICE).o: $(ROOTDIR)/src/$(SERVICE)/$(SERVICE).cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rule for IoCContainer source
$(OBJDIR)/IoCContainer.o: $(ROOTDIR)/src/IoCContainer/IoCContainer.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for C sources (if any)
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

-include $(TEST_DEPS)

run: all
	./$(TEST_BIN)
	
clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
