OBJDIR = obj
BINDIR = bin
TEST_SRC = WebServerTest.cpp TestMain.cpp ../../src/WebServer/WebServer.cpp
TEST_OBJS = $(OBJDIR)/WebServerTest.o $(OBJDIR)/TestMain.o $(OBJDIR)/WebServer.o
TEST_BIN = $(BINDIR)/WebServerTest

CXX = g++
CXXFLAGS = -std=c++20 -I../../include -I../../external/googletest/googletest/include -g -Wall -Wextra
GTEST_LIBS = -L../../external/googletest/build/lib -lgtest -lgtest_main

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/WebServer.o: ../../src/WebServer/WebServer.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/WebServerTest
