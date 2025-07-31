include Makefile.build
APPNAME = hello_world

# Libraries and linkage
LIBFLGS += -lpthread -lrt -lm
##LIBFLGS += -lnanomsg -lopdevsdk -lcurl -lssl -lcrypto -lcjson


# 3rd party library example:
# Place your .a or .so files in the lib/ directory
# Place 3rd party headers in lib/include/<libname>/
LIBFLGS += -Llib
# To link with libfoo.a or libfoo.so, and libbar.a or libbar.so, uncomment or add:
# LIBFLGS += -lfoo -lbar

# Add lib/include to the include path (edit Makefile.build if you want this for all builds)
CXXFLAGS_debug   += -Ilib/include
CXXFLAGS_release += -Ilib/include

# Add any additional dependencies or linkage logic below

all: release

debug: $(BIN_DIR_debug)/$(APPNAME)
	@echo "Building debug version of $(APPNAME) for $(BUILD_NAME)"
	@echo "CXXFLAGS: $(CXXFLAGS_debug)"
release: $(BIN_DIR_release)/$(APPNAME)
	@echo "Building release version of $(APPNAME) for $(BUILD_NAME)"
	@echo "CXXFLAGS: $(CXXFLAGS_release)"

$(BIN_DIR_debug)/$(APPNAME): $(OBJECTS_debug)
	@mkdir -p $(BIN_DIR_debug)
	$(CXX) $(CXXFLAGS_debug) -o $@ $^ $(LIBFLGS)

$(BIN_DIR_release)/$(APPNAME): $(OBJECTS_release)
	@mkdir -p $(BIN_DIR_release)
	$(CXX) $(CXXFLAGS_release) -o $@ $^ $(LIBFLGS)

# Compile object files from src/*.cpp
$(BIN_DIR_debug)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_debug) -c $< -o $@

$(BIN_DIR_release)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_release) -c $< -o $@

clean:
	rm -f sh *.elf *.gdb
	rm -rf bin/$(BUILD_NAME)