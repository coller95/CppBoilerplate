OBJDIR = obj
BINDIR = bin
TEST_SRC = RestfulServerTest.cpp TestMain.cpp ../../src/RestfulServer/RestfulServer.cpp
TEST_OBJS = $(OBJDIR)/RestfulServerTest.o $(OBJDIR)/TestMain.o $(OBJDIR)/RestfulServer.o
TEST_BIN = $(BINDIR)/RestfulServerTest

CXX = g++
CXXFLAGS = -std=c++17 -I../../include -I../../external/googletest/googletest/include -I../../external/googletest/googlemock/include -g -Wall -Wextra
GTEST_LIBS = -L../../external/googletest/build/lib -lgtest -lgtest_main -lpthread

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/RestfulServer.o: ../../src/RestfulServer/RestfulServer.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS)

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/RestfulServerTest
