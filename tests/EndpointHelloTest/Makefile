# Modern, flexible Makefile for unit tests (Google Test)
# Supports easy addition of services, modules, and dependencies
# All object and dependency files are placed flat in obj/ (not nested)

# ============================================================================
# CONFIGURATION SECTION - Modify this section to add/remove dependencies
# ============================================================================

ROOTDIR = ../..
MODULE_NAME = EndpointHello

# Primary module being tested (automatically included)
PRIMARY_MODULE = $(MODULE_NAME)

# Additional dependencies - add any services, modules, or utilities needed
# Format: ModuleName:FolderName (if folder differs from module name, otherwise just ModuleName)
DEPENDENCIES = ApiModules:ApiModule

# External dependencies (from external/ folder) - uncomment if needed
# EXTERNAL_DEPS = mongoose foo

# ============================================================================
# BUILD CONFIGURATION - Usually no changes needed below this line
# ============================================================================

CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -I$(ROOTDIR)/include -I$(ROOTDIR)/external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
CFLAGS = -I$(ROOTDIR)/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L$(ROOTDIR)/external/googletest/build/lib -lgtest -lgtest_main

OBJDIR = obj
BINDIR = bin

# Process dependencies into source files and object files
DEP_SOURCES = $(foreach dep,$(DEPENDENCIES),$(ROOTDIR)/src/$(word 2,$(subst :, ,$(dep)))/$(word 1,$(subst :, ,$(dep))).cpp)
DEP_OBJECTS = $(foreach dep,$(DEPENDENCIES),$(OBJDIR)/$(word 1,$(subst :, ,$(dep))).o)
DEP_NAMES = $(foreach dep,$(DEPENDENCIES),$(word 1,$(subst :, ,$(dep))))
DEP_FOLDERS = $(foreach dep,$(DEPENDENCIES),$(word 2,$(subst :, ,$(dep))))

# External dependencies (if any)
ifdef EXTERNAL_DEPS
EXT_SOURCES = $(foreach ext,$(EXTERNAL_DEPS),$(ROOTDIR)/external/$(ext)/src/$(ext).cpp)
EXT_OBJECTS = $(foreach ext,$(EXTERNAL_DEPS),$(OBJDIR)/$(ext).o)
EXT_NAMES = $(EXTERNAL_DEPS)
else
EXT_SOURCES =
EXT_OBJECTS =
EXT_NAMES =
endif

# Complete source and object lists
ALL_SOURCES = cases/*.cpp TestMain.cpp $(ROOTDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).cpp $(DEP_SOURCES) $(EXT_SOURCES)
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/TestMain.o $(OBJDIR)/$(PRIMARY_MODULE).o $(DEP_OBJECTS) $(EXT_OBJECTS)
TEST_DEPS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(notdir $(basename $(wildcard cases/*.cpp)))) $(OBJDIR)/TestMain.d $(OBJDIR)/$(PRIMARY_MODULE).d $(foreach dep,$(DEP_NAMES),$(OBJDIR)/$(dep).d) $(foreach ext,$(EXT_NAMES),$(OBJDIR)/$(ext).d)
TEST_BIN = $(BINDIR)/$(MODULE_NAME)Test

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# ============================================================================
# COMPILATION RULES - Generic patterns that work for any module
# ============================================================================

# Pattern rule for C++ test cases in cases/
$(OBJDIR)/%.o: cases/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for the test runner
$(OBJDIR)/TestMain.o: TestMain.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for primary module source
$(OBJDIR)/$(PRIMARY_MODULE).o: $(ROOTDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generic rule for dependency modules
$(OBJDIR)/%.o: $(ROOTDIR)/src/%/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rule for ApiModules (since it has a different folder structure)
$(OBJDIR)/ApiModules.o: $(ROOTDIR)/src/ApiModule/ApiModules.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generic rule for external dependencies
$(OBJDIR)/%.o: $(ROOTDIR)/external/%/src/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for C sources (if any)
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link the test binary
$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

# Include dependency files for incremental builds
-include $(TEST_DEPS)

# ============================================================================
# UTILITY TARGETS
# ============================================================================

# Debug: Print configuration for troubleshooting
debug-config:
	@echo "=== Makefile Configuration ==="
	@echo "MODULE_NAME: $(MODULE_NAME)"
	@echo "PRIMARY_MODULE: $(PRIMARY_MODULE)"
	@echo "DEPENDENCIES: $(DEPENDENCIES)"
	@echo "DEP_NAMES: $(DEP_NAMES)"
	@echo "DEP_SOURCES: $(DEP_SOURCES)"
	@echo "DEP_OBJECTS: $(DEP_OBJECTS)"
ifdef EXTERNAL_DEPS
	@echo "EXTERNAL_DEPS: $(EXTERNAL_DEPS)"
	@echo "EXT_SOURCES: $(EXT_SOURCES)"
	@echo "EXT_OBJECTS: $(EXT_OBJECTS)"
endif
	@echo "TEST_OBJS: $(TEST_OBJS)"
	@echo "TEST_BIN: $(TEST_BIN)"

clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*

.PHONY: all clean debug-config
