
OBJDIR = obj
BINDIR = bin
TEST_SRC = WebServerTest.cpp TestMain.cpp ../../src/WebServer/WebServer.cpp ../../src/WebServer/impl/Factory.cpp ../../src/WebServer/impl/MongooseWebServerImpl.cpp ../../external/mongoose/src/mongoose.c ../../src/IoCContainer/IoCContainer.cpp
TEST_OBJS = $(OBJDIR)/WebServerTest.o $(OBJDIR)/TestMain.o $(OBJDIR)/WebServer.o $(OBJDIR)/Factory.o $(OBJDIR)/MongooseWebServerImpl.o $(OBJDIR)/mongoose.o $(OBJDIR)/IoCContainer.o
TEST_DEPS = $(OBJDIR)/WebServerTest.d $(OBJDIR)/TestMain.d $(OBJDIR)/WebServer.d $(OBJDIR)/Factory.d $(OBJDIR)/MongooseWebServerImpl.d $(OBJDIR)/mongoose.d $(OBJDIR)/IoCContainer.d
TEST_BIN = $(BINDIR)/WebServerTest

CXX = g++
CXXFLAGS = -std=c++20 -I../../include -I../../external/googletest/googletest/include -I../../external/mongoose/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L../../external/googletest/build/lib -lgtest -lgtest_main

CC = gcc
CFLAGS = -I../../include -I../../external/mongoose/include -g -Wall -Wextra -MMD -MP

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/WebServer.o: ../../src/WebServer/WebServer.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/Factory.o: ../../src/WebServer/impl/Factory.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/MongooseWebServerImpl.o: ../../src/WebServer/impl/MongooseWebServerImpl.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/mongoose.o: ../../external/mongoose/src/mongoose.c | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/IoCContainer.o: ../../src/IoCContainer/IoCContainer.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

-include $(TEST_DEPS)

clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
