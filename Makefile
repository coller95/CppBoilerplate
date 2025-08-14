
# Modern, robust Makefile for CppBoilerplate
include Platform.build

APPNAME = hello_world

# Multi-tiered build output modes for LLM context optimization
# VERBOSE=minimal   - Ultra-minimal output (default, best performance)
# VERBOSE=standard  - Standard agent output (moderate context)
# VERBOSE=debug     - Agent + debugging context (troubleshooting)
# VERBOSE=human     - Full human-friendly output (verbose)
# VERBOSE=silent    - Errors only (critical only)
#
# Backward compatibility:
# VERBOSE=0 -> minimal, VERBOSE=1 -> human
VERBOSE ?= minimal

# Map legacy numeric values to named modes
ifeq ($(VERBOSE),0)
    override VERBOSE = minimal
endif
ifeq ($(VERBOSE),1)
    override VERBOSE = human
endif

# Output formatting based on mode
ifeq ($(VERBOSE),human)
    # Human mode: Colorful, descriptive output
    MSG_PREFIX = @echo
    SUCCESS_MSG = "✅ Successfully built"
    COMPILE_MSG = "🔨 Compiling"
    LINK_MSG = "🔗 Linking"
    CLEAN_MSG = "🧹 Cleaning build artifacts..."
    COPY_MSG = "📁 Copying executable to APP directory..."
    LAUNCH_MSG = "🚀 Launching"
    QUIET = @
    COMPILE_QUIET = @
else ifeq ($(VERBOSE),debug)
    # Debug mode: Agent + compilation context
    MSG_PREFIX = @echo "[BUILD]"
    SUCCESS_MSG = "BUILT"
    COMPILE_MSG = "COMPILE"
    LINK_MSG = "LINK"
    CLEAN_MSG = "CLEAN"
    COPY_MSG = "COPY"
    LAUNCH_MSG = "RUN"
    QUIET = @
    COMPILE_QUIET = @
else ifeq ($(VERBOSE),standard)
    # Standard mode: Balanced agent output
    MSG_PREFIX = @echo "[BUILD]"
    SUCCESS_MSG = "BUILT"
    COMPILE_MSG = ""
    LINK_MSG = "LINK"
    CLEAN_MSG = "CLEAN"
    COPY_MSG = "COPY"
    LAUNCH_MSG = "RUN"
    QUIET = @
    COMPILE_QUIET = @
else ifeq ($(VERBOSE),minimal)
    # Minimal mode: Ultra-minimal for best performance
    MSG_PREFIX = @echo "[BUILD]"
    SUCCESS_MSG = ""
    COMPILE_MSG = ""
    LINK_MSG = ""
    CLEAN_MSG = "CLEAN"
    COPY_MSG = ""
    LAUNCH_MSG = "RUN"
    QUIET = @
    COMPILE_QUIET = @
else ifeq ($(VERBOSE),silent)
    # Silent mode: Errors only
    MSG_PREFIX = @true
    SUCCESS_MSG = ""
    COMPILE_MSG = ""
    LINK_MSG = ""
    CLEAN_MSG = ""
    COPY_MSG = ""
    LAUNCH_MSG = ""
    QUIET = @
    COMPILE_QUIET = @
else
    # Fallback to minimal for unknown modes
    MSG_PREFIX = @echo "[BUILD]"
    SUCCESS_MSG = ""
    COMPILE_MSG = ""
    LINK_MSG = ""
    CLEAN_MSG = "CLEAN"
    COPY_MSG = ""
    LAUNCH_MSG = "RUN"
    QUIET = @
    COMPILE_QUIET = @
endif

# Directories
BIN_DIR_debug   = bin/$(BUILD_NAME)/debug
BIN_DIR_release = bin/$(BUILD_NAME)/release

# Source file discovery (automatic + configurable)
MAIN_SOURCES = $(shell find src -name '*.cpp')

# Include build configurations
-include Compiler.build
-include Project.build

# All source files (main + external + additional from Project.build)
SRCFILES = $(MAIN_SOURCES) $(EXTERNAL_CPP_SOURCES) $(EXTERNAL_C_SOURCES) $(ADDITIONAL_SOURCES)

# Generate object file names for different build types
OBJECTS_debug = $(MAIN_SOURCES:src/%.cpp=$(BIN_DIR_debug)/src/%.o) \
			   $(EXTERNAL_CPP_SOURCES:external/%.cpp=$(BIN_DIR_debug)/external/%.o) \
			   $(EXTERNAL_C_SOURCES:external/%.c=$(BIN_DIR_debug)/external/%.o)

OBJECTS_release = $(MAIN_SOURCES:src/%.cpp=$(BIN_DIR_release)/src/%.o) \
				 $(EXTERNAL_CPP_SOURCES:external/%.cpp=$(BIN_DIR_release)/external/%.o) \
				 $(EXTERNAL_C_SOURCES:external/%.c=$(BIN_DIR_release)/external/%.o)

# Include dependency files for incremental builds
-include $(OBJECTS_debug:.o=.d)
-include $(OBJECTS_release:.o=.d)

# Base library configuration (can be extended in Project.build)
LIB_DIRS     += -Llib 
LDLIBS       += -lpthread -lrt -lm
# Project.build already included above - contains additional library flags

# Include directories (base + additional from Project.build)
INCLUDE_DIRS = -Iinclude -Ilib/include $(ADDITIONAL_INCLUDES)

# Compiler settings for C++ files (standard defined in Platform.build)
CXXFLAGS_base = -std=$(CXX_STANDARD) -MMD -MP $(INCLUDE_DIRS) -Wall -Wextra
CXXFLAGS_debug   = -g -O0 -DDEBUG $(CXXFLAGS_base)
CXXFLAGS_release = -O2 -DNDEBUG $(CXXFLAGS_base)

# Compiler settings for C files
CFLAGS_base = -MMD -MP $(INCLUDE_DIRS) -Wall -Wextra
CFLAGS_debug = -g -O0 -DDEBUG $(CFLAGS_base)
CFLAGS_release = -O2 -DNDEBUG $(CFLAGS_base)

# Linker settings (library directories and libraries)
LDFLAGS = $(LIB_DIRS) $(LDLIBS)

# Default target
all: release

# Build targets
debug: $(BIN_DIR_debug)/$(APPNAME)
ifeq ($(VERBOSE),human)
	@echo "✅ Successfully built debug version of $(APPNAME) for $(BUILD_NAME)"
	@echo "   Executable: $(BIN_DIR_debug)/$(APPNAME)"
else ifeq ($(VERBOSE),debug)
	@echo "[BUILD] BUILT debug $(APPNAME) $(BUILD_NAME)"
else ifeq ($(VERBOSE),standard)
	@echo "[BUILD] BUILT debug $(APPNAME) $(BUILD_NAME)"
else ifeq ($(VERBOSE),minimal)
	@echo "[BUILD] $(APPNAME)"
else ifneq ($(VERBOSE),silent)
	@echo "[BUILD] $(APPNAME)"
endif

release: $(BIN_DIR_release)/$(APPNAME)
ifeq ($(VERBOSE),human)
	@echo "✅ Successfully built release version of $(APPNAME) for $(BUILD_NAME)"
	@echo "   Executable: $(BIN_DIR_release)/$(APPNAME)"
else ifeq ($(VERBOSE),debug)
	@echo "[BUILD] BUILT release $(APPNAME) $(BUILD_NAME)"
else ifeq ($(VERBOSE),standard)
	@echo "[BUILD] BUILT release $(APPNAME) $(BUILD_NAME)"
else ifeq ($(VERBOSE),minimal)
	@echo "[BUILD] $(APPNAME)"
else ifneq ($(VERBOSE),silent)
	@echo "[BUILD] $(APPNAME)"
endif

# Linking rules
$(BIN_DIR_debug)/$(APPNAME): $(OBJECTS_debug)
	@mkdir -p $(BIN_DIR_debug)
ifeq ($(VERBOSE),human)
	@echo "🔗 Linking debug executable..."
	$(CXX) $(CXXFLAGS_debug) -o $@ $^ $(LDFLAGS)
else ifeq ($(VERBOSE),debug)
	@echo "[BUILD] LINK debug $(APPNAME)"
	@$(CXX) $(CXXFLAGS_debug) -o $@ $^ $(LDFLAGS)
else ifeq ($(VERBOSE),standard)
	@echo "[BUILD] LINK debug $(APPNAME)"
	@$(CXX) $(CXXFLAGS_debug) -o $@ $^ $(LDFLAGS)
else ifneq ($(VERBOSE),silent)
	@$(CXX) $(CXXFLAGS_debug) -o $@ $^ $(LDFLAGS)
else
	@$(CXX) $(CXXFLAGS_debug) -o $@ $^ $(LDFLAGS)
endif

$(BIN_DIR_release)/$(APPNAME): $(OBJECTS_release)
	@mkdir -p $(BIN_DIR_release)
ifeq ($(VERBOSE),human)
	@echo "🔗 Linking release executable..."
	$(CXX) $(CXXFLAGS_release) -o $@ $^ $(LDFLAGS)
else ifeq ($(VERBOSE),debug)
	@echo "[BUILD] LINK release $(APPNAME)"
	@$(CXX) $(CXXFLAGS_release) -o $@ $^ $(LDFLAGS)
else ifeq ($(VERBOSE),standard)
	@echo "[BUILD] LINK release $(APPNAME)"
	@$(CXX) $(CXXFLAGS_release) -o $@ $^ $(LDFLAGS)
else ifneq ($(VERBOSE),silent)
	@$(CXX) $(CXXFLAGS_release) -o $@ $^ $(LDFLAGS)
else
	@$(CXX) $(CXXFLAGS_release) -o $@ $^ $(LDFLAGS)
endif


# Compilation rules for C++ source files
$(BIN_DIR_debug)/%.o: %.cpp
	@mkdir -p $(dir $@)
ifeq ($(VERBOSE),1)
	@echo "🔨 Compiling (debug): $<"
	$(CXX) $(CXXFLAGS_debug) -c $< -o $@
else
	$(COMPILE_QUIET)$(CXX) $(CXXFLAGS_debug) -c $< -o $@
endif

$(BIN_DIR_release)/%.o: %.cpp
	@mkdir -p $(dir $@)
ifeq ($(VERBOSE),1)
	@echo "🔨 Compiling (release): $<"
	$(CXX) $(CXXFLAGS_release) -c $< -o $@
else
	$(COMPILE_QUIET)$(CXX) $(CXXFLAGS_release) -c $< -o $@
endif

# Compilation rules for C source files
$(BIN_DIR_debug)/%.o: %.c
	@mkdir -p $(dir $@)
ifeq ($(VERBOSE),1)
	@echo "🔨 Compiling C (debug): $<"
	$(CC) $(CFLAGS_debug) -c $< -o $@
else
	$(COMPILE_QUIET)$(CC) $(CFLAGS_debug) -c $< -o $@
endif

$(BIN_DIR_release)/%.o: %.c
	@mkdir -p $(dir $@)
ifeq ($(VERBOSE),1)
	@echo "🔨 Compiling C (release): $<"
	$(CC) $(CFLAGS_release) -c $< -o $@
else
	$(COMPILE_QUIET)$(CC) $(CFLAGS_release) -c $< -o $@
endif

# Cleanup targets
clean:
ifeq ($(VERBOSE),1)
	@echo "🧹 Cleaning build artifacts..."
	rm -f sh *.elf *.gdb
	rm -rf bin/$(BUILD_NAME)
	@echo "✅ Clean completed"
else
	@echo "[BUILD] CLEAN"
	@rm -f sh *.elf *.gdb
	@rm -rf bin/$(BUILD_NAME)
endif

# Utility targets
copy:
ifeq ($(VERBOSE),1)
	@echo "📁 Copying executable to APP directory..."
	@mkdir -p APP
	cp $(BIN_DIR_release)/$(APPNAME) APP/ 2>/dev/null || true
	@echo "✅ Copy completed"
else
	@echo "[BUILD] COPY $(APPNAME)"
	@mkdir -p APP
	@cp $(BIN_DIR_release)/$(APPNAME) APP/ 2>/dev/null || true
endif

clean_copy:
ifeq ($(VERBOSE),1)
	@echo "🗑️  Removing copied executable..."
	rm -rf APP/$(APPNAME)
	@echo "✅ Cleanup completed"
else
	@echo "[BUILD] CLEAN_COPY"
	@rm -rf APP/$(APPNAME)
endif


# Execution targets
run_debug: debug
ifeq ($(VERBOSE),1)
	@echo "🚀 Launching debug build: $(BIN_DIR_debug)/$(APPNAME)"
	@$(BIN_DIR_debug)/$(APPNAME)
else
	@echo "[BUILD] RUN debug $(APPNAME)"
	@$(BIN_DIR_debug)/$(APPNAME)
endif

run_release: release
ifeq ($(VERBOSE),1)
	@echo "🚀 Launching release build: $(BIN_DIR_release)/$(APPNAME)"
	@$(BIN_DIR_release)/$(APPNAME)
else
	@echo "[BUILD] RUN release $(APPNAME)"
	@$(BIN_DIR_release)/$(APPNAME)
endif

# For backward compatibility, 'run' launches release build
run: run_release

# Include test build system
include Tests.build