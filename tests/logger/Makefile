# Makefile for Logger unit tests using Google Test

CXX = g++
CXXFLAGS = -std=c++17 -I../../include \
	-I../../external/googletest/googletest/include \
	-I../../external/googletest/googletest \
	-I../../external/googletest/googlemock/include \
	-I../../external/googletest/googlemock \
	-pthread -g

GTEST_LIB_DIR = ../../external/googletest/build/lib
GTEST_LIBS = $(GTEST_LIB_DIR)/libgtest.a $(GTEST_LIB_DIR)/libgtest_main.a $(GTEST_LIB_DIR)/libgmock.a $(GTEST_LIB_DIR)/libgmock_main.a

OBJDIR = obj
BINDIR = bin
TEST_SRC = loggerTest.cpp test_main.cpp ../../src/Logger.cpp
TEST_OBJS = $(OBJDIR)/loggerTest.o $(OBJDIR)/test_main.o $(OBJDIR)/Logger.o
TEST_BIN = $(BINDIR)/loggerTest

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/Logger.o: ../../src/Logger.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/loggerTest
