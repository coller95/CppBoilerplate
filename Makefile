
# Modern, robust Makefile for CppBoilerplate
include Makefile.build

APPNAME = hello_world

# Directories
BIN_DIR_debug   = bin/$(BUILD_NAME)/debug
BIN_DIR_release = bin/$(BUILD_NAME)/release

# Source file discovery
MAIN_SOURCES = $(shell find src -name '*.cpp')
EXTERNAL_CPP_SOURCES = $(wildcard external/*/src/*.cpp)
WEBSERVER_IMPL_SOURCES = $(wildcard src/WebServer/impl/*.cpp)
EXTERNAL_C_SOURCES = $(wildcard external/*/src/*.c)

# All source files
SRCFILES = $(MAIN_SOURCES) $(EXTERNAL_CPP_SOURCES) $(EXTERNAL_C_SOURCES) $(WEBSERVER_IMPL_SOURCES)

# Generate object file names for different build types
OBJECTS_debug = $(MAIN_SOURCES:src/%.cpp=$(BIN_DIR_debug)/src/%.o) \
			   $(EXTERNAL_CPP_SOURCES:external/%.cpp=$(BIN_DIR_debug)/external/%.o) \
			   $(EXTERNAL_C_SOURCES:external/%.c=$(BIN_DIR_debug)/external/%.o) \
			   $(WEBSERVER_IMPL_SOURCES:src/%.cpp=$(BIN_DIR_debug)/src/%.o)

OBJECTS_release = $(MAIN_SOURCES:src/%.cpp=$(BIN_DIR_release)/src/%.o) \
				 $(EXTERNAL_CPP_SOURCES:external/%.cpp=$(BIN_DIR_release)/external/%.o) \
				 $(EXTERNAL_C_SOURCES:external/%.c=$(BIN_DIR_release)/external/%.o) \
				 $(WEBSERVER_IMPL_SOURCES:src/%.cpp=$(BIN_DIR_release)/src/%.o)

# Include dependency files for incremental builds
-include $(OBJECTS_debug:.o=.d)
-include $(OBJECTS_release:.o=.d)

# Include directories and library configuration
INCLUDE_DIRS = -Iinclude -Ilib/include $(foreach dir,$(wildcard external/*/include),-I$(dir)) -Iexternal/mongoose/include
LIB_DIRS     = -Llib 
LDLIBS       = -lpthread -lrt -lm

# Include additional linker flags if available
-include Ldflags.build
LDFLAGS = $(LIB_DIRS) $(LDLIBS)

# Compiler settings for C++ files (C++17 standard)
CXXFLAGS_base = -std=c++17 -MMD -MP $(INCLUDE_DIRS) -Wall -Wextra
CXXFLAGS_debug   = -g -O0 -DDEBUG $(CXXFLAGS_base)
CXXFLAGS_release = -O2 -DNDEBUG $(CXXFLAGS_base)

# Compiler settings for C files
CFLAGS_base = -MMD -MP $(INCLUDE_DIRS) -Wall -Wextra
CFLAGS_debug = -g -O0 -DDEBUG $(CFLAGS_base)
CFLAGS_release = -O2 -DNDEBUG $(CFLAGS_base)

# Default target
all: release

# Build targets
debug: $(BIN_DIR_debug)/$(APPNAME)
	@echo "✅ Successfully built debug version of $(APPNAME) for $(BUILD_NAME)"
	@echo "   Executable: $(BIN_DIR_debug)/$(APPNAME)"

release: $(BIN_DIR_release)/$(APPNAME)
	@echo "✅ Successfully built release version of $(APPNAME) for $(BUILD_NAME)"
	@echo "   Executable: $(BIN_DIR_release)/$(APPNAME)"

# Linking rules
$(BIN_DIR_debug)/$(APPNAME): $(OBJECTS_debug)
	@mkdir -p $(BIN_DIR_debug)
	@echo "🔗 Linking debug executable..."
	$(CXX) $(CXXFLAGS_debug) -o $@ $^ $(LDFLAGS)

$(BIN_DIR_release)/$(APPNAME): $(OBJECTS_release)
	@mkdir -p $(BIN_DIR_release)
	@echo "🔗 Linking release executable..."
	$(CXX) $(CXXFLAGS_release) -o $@ $^ $(LDFLAGS)


# Compilation rules for C++ source files
$(BIN_DIR_debug)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "🔨 Compiling (debug): $<"
	$(CXX) $(CXXFLAGS_debug) -c $< -o $@

$(BIN_DIR_release)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "🔨 Compiling (release): $<"
	$(CXX) $(CXXFLAGS_release) -c $< -o $@

# Compilation rules for C source files
$(BIN_DIR_debug)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "🔨 Compiling C (debug): $<"
	$(CC) $(CFLAGS_debug) -c $< -o $@

$(BIN_DIR_release)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "🔨 Compiling C (release): $<"
	$(CC) $(CFLAGS_release) -c $< -o $@

# Cleanup targets
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -f sh *.elf *.gdb
	rm -rf bin/$(BUILD_NAME)
	@echo "✅ Clean completed"

# Utility targets
copy:
	@echo "📁 Copying executable to APP directory..."
	@mkdir -p APP
	cp $(BIN_DIR_release)/$(APPNAME) APP/ 2>/dev/null || true
	@echo "✅ Copy completed"

clean_copy:
	@echo "🗑️  Removing copied executable..."
	rm -rf APP/$(APPNAME)
	@echo "✅ Cleanup completed"


# Execution targets
run_debug: debug
	@echo "🚀 Launching debug build: $(BIN_DIR_debug)/$(APPNAME)"
	@$(BIN_DIR_debug)/$(APPNAME)

run_release: release
	@echo "🚀 Launching release build: $(BIN_DIR_release)/$(APPNAME)"
	@$(BIN_DIR_release)/$(APPNAME)

# For backward compatibility, 'run' launches release build
run: run_release

# Include test build system
include test.build