ROOTDIR = ../..
CXX = g++
CC = gcc
CXXFLAGS = -std=c++20 -I$(ROOTDIR)/include -I$(ROOTDIR)/external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L$(ROOTDIR)/external/googletest/build/lib -lgtest -lgtest_main
OBJDIR = obj
BINDIR = bin
TEST_SRC = cases/*.cpp TestMain.cpp $(ROOTDIR)/src/ServiceB/ServiceB.cpp
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/TestMain.o $(OBJDIR)/ServiceB.o
TEST_DEPS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(notdir $(basename $(wildcard cases/*.cpp)))) $(OBJDIR)/TestMain.d $(OBJDIR)/ServiceB.d
TEST_BIN = $(BINDIR)/ServiceBTest
all: $(OBJDIR) $(BINDIR) $(TEST_BIN)
$(OBJDIR):
	mkdir -p $(OBJDIR)
$(BINDIR):
	mkdir -p $(BINDIR)
$(OBJDIR)/%.o: cases/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJDIR)/%.o: cases/%.c | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CC) $(CXXFLAGS) -c $< -o $@
$(OBJDIR)/TestMain.o: TestMain.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJDIR)/ServiceB.o: $(ROOTDIR)/src/ServiceB/ServiceB.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread
-include $(TEST_DEPS)
clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
