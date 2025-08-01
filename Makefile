include Makefile.build

APPNAME = hello_world

# Directories
BIN_DIR_debug   = bin/$(BUILD_NAME)/debug
BIN_DIR_release = bin/$(BUILD_NAME)/release

# Collect sources
MAIN_SOURCES = $(wildcard src/*.cpp)
EXTERNAL_CPP_SOURCES = $(wildcard external/*/src/*.cpp)
EXTERNAL_C_SOURCES = $(wildcard external/*/src/*.c)
SRCFILES = $(MAIN_SOURCES) $(EXTERNAL_CPP_SOURCES) $(EXTERNAL_C_SOURCES)

# Generate object file names
OBJECTS_debug = $(MAIN_SOURCES:src/%.cpp=$(BIN_DIR_debug)/src/%.o) \
               $(EXTERNAL_CPP_SOURCES:external/%.cpp=$(BIN_DIR_debug)/external/%.o) \
               $(EXTERNAL_C_SOURCES:external/%.c=$(BIN_DIR_debug)/external/%.o)
OBJECTS_release = $(MAIN_SOURCES:src/%.cpp=$(BIN_DIR_release)/src/%.o) \
                 $(EXTERNAL_CPP_SOURCES:external/%.cpp=$(BIN_DIR_release)/external/%.o) \
                 $(EXTERNAL_C_SOURCES:external/%.c=$(BIN_DIR_release)/external/%.o)

# Dependency files
-include $(OBJECTS_debug:.o=.d)
-include $(OBJECTS_release:.o=.d)

# Compiler and linker flags
INCLUDE_DIRS = -Iinclude -Ilib/include $(foreach dir,$(wildcard external/*/include),-I$(dir))
LIB_DIRS     = -Llib
LDLIBS       = -lpthread -lrt -lm

CXXFLAGS_base = -MMD -MP $(INCLUDE_DIRS)
CXXFLAGS_debug   = -g -O0 -DDEBUG $(CXXFLAGS_base)
CXXFLAGS_release = -O2 -DNDEBUG $(CXXFLAGS_base)
LDFLAGS = $(LIB_DIRS) $(LDLIBS)

all: release

debug: $(BIN_DIR_debug)/$(APPNAME)
	@echo "Building debug version of $(APPNAME) for $(BUILD_NAME)"
	@echo "CXXFLAGS: $(CXXFLAGS_debug)"

release: $(BIN_DIR_release)/$(APPNAME)
	@echo "Building release version of $(APPNAME) for $(BUILD_NAME)"
	@echo "CXXFLAGS: $(CXXFLAGS_release)"

# Link
$(BIN_DIR_debug)/$(APPNAME): $(OBJECTS_debug)
	@mkdir -p $(BIN_DIR_debug)
	$(CXX) $(CXXFLAGS_debug) -o $@ $^ $(LDFLAGS)

$(BIN_DIR_release)/$(APPNAME): $(OBJECTS_release)
	@mkdir -p $(BIN_DIR_release)
	$(CXX) $(CXXFLAGS_release) -o $@ $^ $(LDFLAGS)

# Compiler settings for C files
CC = $(CROSS_COMPILE_PREFIX)gcc
CFLAGS_base = -MMD -MP $(INCLUDE_DIRS)
CFLAGS_debug = -g -O0 -DDEBUG $(CFLAGS_base)
CFLAGS_release = -O2 -DNDEBUG $(CFLAGS_base)

# Compile rules for C++ source files
$(BIN_DIR_debug)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_debug) -c $< -o $@

$(BIN_DIR_release)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_release) -c $< -o $@

# Compile rules for C source files
$(BIN_DIR_debug)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_debug) -c $< -o $@

$(BIN_DIR_release)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_release) -c $< -o $@

clean:
	rm -f sh *.elf *.gdb
	rm -rf bin/$(BUILD_NAME)