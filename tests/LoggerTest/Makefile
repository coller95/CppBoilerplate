# Makefile for LoggerTest unit tests using Google Test

CXX = g++
CXXFLAGS = -std=c++17 -I../../include -I../../external/googletest/googletest/include -I../../external/googletest/googletest -I../../external/googletest/googlemock/include -I../../external/googletest/googlemock -pthread -g -MMD -MP

GTEST_LIB_DIR = ../../external/googletest/build/lib
GTEST_LIBS = $(GTEST_LIB_DIR)/libgtest.a $(GTEST_LIB_DIR)/libgtest_main.a $(GTEST_LIB_DIR)/libgmock.a $(GTEST_LIB_DIR)/libgmock_main.a

OBJDIR = obj
BINDIR = bin
TEST_SRC = LoggerTest.cpp TestMain.cpp ../../src/Logger/Logger.cpp
TEST_OBJS = $(OBJDIR)/LoggerTest.o $(OBJDIR)/TestMain.o $(OBJDIR)/Logger.o
TEST_DEPS = $(OBJDIR)/LoggerTest.d $(OBJDIR)/TestMain.d $(OBJDIR)/Logger.d
TEST_BIN = $(BINDIR)/LoggerTest

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/Logger.o: ../../src/Logger/Logger.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

-include $(TEST_DEPS)

run: all
	./$(TEST_BIN)
	
clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
