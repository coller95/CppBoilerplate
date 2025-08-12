# Makefile for PrintHelloTest unit tests using Google Test

CXX = g++
CXXFLAGS = -std=c++17 -I../../include -I../../include/PrintHello -I../../external/googletest/googletest/include -I../../external/googletest/googletest -pthread -g -MMD -MP

# Link against prebuilt Google Test libraries
GTEST_LIB_DIR = ../../external/googletest/build/lib
GTEST_LIBS = $(GTEST_LIB_DIR)/libgtest.a $(GTEST_LIB_DIR)/libgtest_main.a


# Add the implementation file for PrintHello (modular path)
OBJDIR = obj
BINDIR = bin
TEST_SRC = PrintHelloTest.cpp TestMain.cpp ../../src/PrintHello/PrintHello.cpp
TEST_OBJS = $(OBJDIR)/PrintHelloTest.o $(OBJDIR)/TestMain.o $(OBJDIR)/PrintHello.o
TEST_DEPS = $(OBJDIR)/PrintHelloTest.d $(OBJDIR)/TestMain.d $(OBJDIR)/PrintHello.d
TEST_BIN = $(BINDIR)/PrintHelloTest

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Pattern rule for C++ sources in this directory
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rule for sources outside test dir
$(OBJDIR)/PrintHello.o: ../../src/PrintHello/PrintHello.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

-include $(TEST_DEPS)

run: all
	./$(TEST_BIN)
	
clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
