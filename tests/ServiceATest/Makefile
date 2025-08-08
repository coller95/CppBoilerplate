
CXX = g++
CXXFLAGS = -std=c++20 -I../../include -I../../external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L../../external/googletest/build/lib -lgtest -lgtest_main
OBJDIR = obj
BINDIR = bin
TEST_SRC = cases/*.cpp TestMain.cpp ../../src/ServiceA/ServiceA.cpp
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/TestMain.o $(OBJDIR)/ServiceA.o
TEST_DEPS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(notdir $(basename $(wildcard cases/*.cpp)))) $(OBJDIR)/TestMain.d $(OBJDIR)/ServiceA.d
TEST_BIN = $(BINDIR)/ServiceATest
all: $(OBJDIR) $(BINDIR) $(TEST_BIN)
$(OBJDIR):
	mkdir -p $(OBJDIR)
$(BINDIR):
	mkdir -p $(BINDIR)
$(OBJDIR)/%.o: cases/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for sources in ../../src/ (for ServiceA and future modules)
$(OBJDIR)/%.o: ../../src/%/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/TestMain.o: TestMain.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

# Explicit rule for ServiceA implementation
$(OBJDIR)/ServiceA.o: ../../src/ServiceA/ServiceA.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
-include $(TEST_DEPS)
clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
