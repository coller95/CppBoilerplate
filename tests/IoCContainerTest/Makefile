
OBJDIR = obj
BINDIR = bin
TEST_SRC = IoCContainerTest.cpp TestMain.cpp ../../src/IoCContainer/IoCContainer.cpp
TEST_OBJS = $(OBJDIR)/IoCContainerTest.o $(OBJDIR)/TestMain.o $(OBJDIR)/IoCContainer.o
TEST_DEPS = $(OBJDIR)/IoCContainerTest.d $(OBJDIR)/TestMain.d $(OBJDIR)/IoCContainer.d
TEST_BIN = $(BINDIR)/IoCContainerTest

CXX = g++
CXXFLAGS = -std=c++17 -I../../include -I../../external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L../../external/googletest/build/lib -lgtest -lgtest_main

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/IoCContainer.o: ../../src/IoCContainer/IoCContainer.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

-include $(TEST_DEPS)

run: all
	./$(TEST_BIN)
	
clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
