# Modern, flexibl# Additional dependencies - add any services, modules, or utilities needed
# Format: ModuleName:FolderName (if folder differs from module name, otherwise just ModuleName)
# Examples: Logger IocContainer ServiceA ApiModule:ApiModule
DEPENDENCIES = WebServerkefile for unit tests (Google Test & Google Mock)
# Supports easy addition of services, modules, and dependencies
# Object files are organized by source origin:
#   obj/test/     - test files (cases/, TestMain.cpp)
#   obj/src/      - our source code (modules, services, etc.)
#   obj/external/ - external dependencies

# ============================================================================
# CONFIGURATION SECTION - Modify this section to add/remove dependencies
# ============================================================================

ROOTDIR = ../..
MODULE_NAME = WebServerBackendPlaceholder

# Primary module being tested (automatically included)
PRIMARY_MODULE = $(MODULE_NAME)

# Additional dependencies - add any services, modules, or utilities needed
# Format: ModuleName:FolderName (if folder differs from module name, otherwise just ModuleName)
# Examples: Logger IoCContainer ServiceA ApiModule:ApiModule
DEPENDENCIES = 

# External dependencies - flexible configuration for any external library
# Add external source files that need to be compiled with your tests
# Examples:
#   EXTERNAL_SOURCES = external/mongoose/src/mongoose.c external/sqlite/sqlite3.c
#   EXTERNAL_SOURCES = /usr/local/src/mylib.cpp ../shared/utils.cpp
EXTERNAL_SOURCES = 

# External include paths - add any custom include directories
# Examples:
#   EXTERNAL_INCLUDES = -Iexternal/mongoose/include -Iexternal/sqlite/include
#   EXTERNAL_INCLUDES = -I/usr/local/include -I../shared/include
EXTERNAL_INCLUDES = -I$(ROOTDIR)/external/googletest/googletest/include 
EXTERNAL_INCLUDES += -I$(ROOTDIR)/external/googletest/googlemock/include

# External library paths and libraries
# Examples:
#   EXTERNAL_LIBS = -Lexternal/foo/lib -lfoo -lbar
#   EXTERNAL_LIBS = -L/usr/local/lib -lsqlite3 -lpthread
EXTERNAL_LIBS = -L$(ROOTDIR)/external/googletest/build/lib 
EXTERNAL_LIBS += -lgtest -lgtest_main 
EXTERNAL_LIBS += -lgmock -lgmock_main

# ============================================================================
# BUILD CONFIGURATION - Usually no changes needed below this line
# ============================================================================

CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -I$(ROOTDIR)/include $(EXTERNAL_INCLUDES) -g -Wall -Wextra -MMD -MP
CFLAGS = -I$(ROOTDIR)/include $(EXTERNAL_INCLUDES) -g -Wall -Wextra -MMD -MP
LDLIBS = $(EXTERNAL_LIBS)

OBJDIR = obj
BINDIR = bin

# Process dependencies into source files and object files
# Handle both simple format (Name) and complex format (Name:Folder)
DEP_SOURCES = $(foreach dep,$(DEPENDENCIES),$(ROOTDIR)/src/$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep))/$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)).cpp)
DEP_OBJECTS = $(foreach dep,$(DEPENDENCIES),$(OBJDIR)/src/$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep))/$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)).o)
DEP_NAMES = $(foreach dep,$(DEPENDENCIES),$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)))
DEP_FOLDERS = $(foreach dep,$(DEPENDENCIES),$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep)))

# Process external sources into object files (flexible user-defined sources)
EXT_OBJECTS = $(foreach src,$(EXTERNAL_SOURCES),$(OBJDIR)/external/$(notdir $(basename $(src))).o)

# Complete source and object lists (organized by source origin)
ALL_SOURCES = cases/*.cpp TestMain.cpp $(ROOTDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).cpp $(DEP_SOURCES) $(EXTERNAL_SOURCES)
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/test/cases/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/test/TestMain.o $(OBJDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).o $(DEP_OBJECTS) $(EXT_OBJECTS)
TEST_DEPS = $(patsubst cases/%.cpp,$(OBJDIR)/test/cases/%.d,$(wildcard cases/*.cpp)) $(OBJDIR)/test/TestMain.d $(OBJDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).d $(foreach dep,$(DEPENDENCIES),$(OBJDIR)/src/$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep))/$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)).d)
TEST_BIN = $(BINDIR)/$(MODULE_NAME)Test

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# ============================================================================
# COMPILATION RULES - Organized by source origin
# ============================================================================

# Test files: obj/test/
# Pattern rule for C++ test cases in cases/
$(OBJDIR)/test/cases/%.o: cases/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for the test runner
$(OBJDIR)/test/TestMain.o: TestMain.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Source files: obj/src/
# Rule for primary module source
$(OBJDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).o: $(ROOTDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generic rule for dependency modules (single % in target per GNU Make requirements)
$(OBJDIR)/src/%.o: $(ROOTDIR)/src/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# External files: obj/external/
# Flexible rule for user-defined external sources (any path, any extension)
# Note: External sources don't generate dependency files to avoid auto-generation issues
EXTERNAL_CXXFLAGS = $(filter-out -MMD -MP,$(CXXFLAGS))
EXTERNAL_CFLAGS = $(filter-out -MMD -MP,$(CFLAGS))

$(OBJDIR)/external/%.o: | $(OBJDIR)
	mkdir -p $(dir $@)
	$(eval SOURCE_FILE := $(filter %/$(notdir $(basename $@)).cpp %/$(notdir $(basename $@)).c,$(EXTERNAL_SOURCES)))
	$(if $(filter %.cpp,$(SOURCE_FILE)),$(CXX) $(EXTERNAL_CXXFLAGS) -c $(SOURCE_FILE) -o $@,$(CC) $(EXTERNAL_CFLAGS) -c $(SOURCE_FILE) -o $@)

# Link the test binary
$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS) -pthread

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
ifneq ($(EXTERNAL_SOURCES),)
	@echo "EXTERNAL_SOURCES: $(EXTERNAL_SOURCES)"
	@echo "EXT_OBJECTS: $(EXT_OBJECTS)"
endif
ifneq ($(EXTERNAL_INCLUDES),)
	@echo "EXTERNAL_INCLUDES: $(EXTERNAL_INCLUDES)"
endif
ifneq ($(EXTERNAL_LIBS),)
	@echo "EXTERNAL_LIBS: $(EXTERNAL_LIBS)"
endif
	@echo "TEST_OBJS: $(TEST_OBJS)"
	@echo "TEST_BIN: $(TEST_BIN)"
	@echo ""
	@echo "=== Directory Structure ==="
	@echo "obj/test/     - test files (cases/, TestMain.cpp)"
	@echo "obj/src/      - our source code (modules, services, etc.)"
	@echo "obj/external/ - external dependencies (user-defined)"

run: all
	@./$(TEST_BIN)

clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*

.PHONY: all clean debug-config run
