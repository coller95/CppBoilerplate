# Makefile for printHello unit tests using Google Test

CXX = g++
CXXFLAGS = -std=c++17 -I../../include -I../../external/googletest/googletest/include -I../../external/googletest/googletest -pthread -g



# Link against prebuilt Google Test libraries
GTEST_LIB_DIR = ../../external/googletest/build/lib
GTEST_LIBS = $(GTEST_LIB_DIR)/libgtest.a $(GTEST_LIB_DIR)/libgtest_main.a



# Add the implementation file for printHello
OBJDIR = obj
BINDIR = bin
TEST_SRC = printHelloTest.cpp test_main.cpp ../../src/printHello.cpp
TEST_OBJS = $(OBJDIR)/printHelloTest.o $(OBJDIR)/test_main.o $(OBJDIR)/printHello.o
TEST_BIN = $(BINDIR)/printHelloTest



all: $(OBJDIR) $(BINDIR) $(TEST_BIN)




# Create obj directory if it doesn't exist

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Rule for local .cpp files
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for printHello.o from ../../src/printHello.cpp
$(OBJDIR)/printHello.o: ../../src/printHello.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


gtest.o: $(GTEST_SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

gtest_main.o: $(GTEST_MAIN)
	$(CXX) $(CXXFLAGS) -c $< -o $@



$(TEST_BIN): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/printHelloTest
