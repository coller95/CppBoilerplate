# Makefile for PrintHelloTest unit tests using Google Test

CXX = g++
CXXFLAGS = -std=c++17 -I../../include -I../../external/googletest/googletest/include -I../../external/googletest/googletest -pthread -g

# Link against prebuilt Google Test libraries
GTEST_LIB_DIR = ../../external/googletest/build/lib
GTEST_LIBS = $(GTEST_LIB_DIR)/libgtest.a $(GTEST_LIB_DIR)/libgtest_main.a

# Add the implementation file for printHello
OBJDIR = obj
BINDIR = bin
TEST_SRC = PrintHelloTest.cpp TestMain.cpp ../../src/PrintHello.cpp
TEST_OBJS = $(OBJDIR)/PrintHelloTest.o $(OBJDIR)/TestMain.o $(OBJDIR)/PrintHello.o
TEST_BIN = $(BINDIR)/PrintHelloTest

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Rule for local .cpp files
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for PrintHello.o from ../../src/PrintHello.cpp
$(OBJDIR)/PrintHello.o: ../../src/PrintHello.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/PrintHelloTest
