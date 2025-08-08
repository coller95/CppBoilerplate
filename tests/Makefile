# Main Makefile for running all unit tests in subdirectories
# Supports the flexible Makefile system and provides comprehensive test management

# Auto-discover test subdirectories that have Makefiles
SUBDIRS := $(shell find . -maxdepth 1 -type d -not -path '.' -exec test -e '{}/Makefile' \; -print | sed 's|^\./||')

# Phony targets for clarity and reliability
.PHONY: all clean test help debug-all stats $(SUBDIRS)
.PHONY: test-run-% test-make-% test-clean-% test-debug-%

# ============================================================================
# INDIVIDUAL TEST TARGETS - Run specific test modules
# ============================================================================

# Build and run tests for a specific module
.PHONY: test-run-%
test-run-%:
	@echo "🧪 Building and running tests for $*..."
	@if [ ! -d "$*" ]; then \
		echo "❌ Error: Test directory '$*' does not exist"; \
		echo "Available test modules: $(SUBDIRS)"; \
		exit 1; \
	fi
	$(MAKE) -C $* all
	@absdir="$$(cd $* && pwd)"; \
	test_count=0; \
	passed_count=0; \
	for bin in "$$absdir"/bin/*Test; do \
		if [ -x "$$bin" ]; then \
			test_count=$$((test_count + 1)); \
			echo "▶️  Running $$(basename $$bin)..."; \
			if "$$bin"; then \
				passed_count=$$((passed_count + 1)); \
				echo "✅ $$(basename $$bin) passed"; \
			else \
				echo "❌ $$(basename $$bin) failed"; \
			fi; \
		fi; \
	done; \
	if [ $$test_count -eq 0 ]; then \
		echo "⚠️  No test executables found in $*/bin/"; \
	else \
		echo "📊 Tests completed: $$passed_count/$$test_count passed"; \
		if [ $$passed_count -ne $$test_count ]; then \
			exit 1; \
		fi; \
	fi

# Build tests for a specific module (without running)
.PHONY: test-make-%
test-make-%:
	@echo "🔧 Building tests for $*..."
	@if [ ! -d "$*" ]; then \
		echo "❌ Error: Test directory '$*' does not exist"; \
		echo "Available test modules: $(SUBDIRS)"; \
		exit 1; \
	fi
	$(MAKE) -C $* all

# Clean build artifacts for a specific module
.PHONY: test-clean-%
test-clean-%:
	@echo "🧹 Cleaning test artifacts for $*..."
	@if [ ! -d "$*" ]; then \
		echo "❌ Error: Test directory '$*' does not exist"; \
		echo "Available test modules: $(SUBDIRS)"; \
		exit 1; \
	fi
	$(MAKE) -C $* clean

# Debug configuration for a specific module
.PHONY: test-debug-%
test-debug-%:
	@echo "🐛 Debug configuration for $*..."
	@if [ ! -d "$*" ]; then \
		echo "❌ Error: Test directory '$*' does not exist"; \
		echo "Available test modules: $(SUBDIRS)"; \
		exit 1; \
	fi
	@if grep -q "debug-config:" "$*/Makefile" 2>/dev/null; then \
		$(MAKE) -C $* debug-config; \
	else \
		echo "⚠️  Module $* does not support debug-config target"; \
		echo "💡 If this is a flexible Makefile, add the debug-config target"; \
	fi

# ============================================================================
# BATCH TARGETS - Work with all test modules
# ============================================================================

# Build all test modules
all: $(SUBDIRS)
	@echo "✅ All test modules built successfully"

# Build a specific test module
$(SUBDIRS):
	@echo "🔧 Building $@..."
	$(MAKE) -C $@

# Clean all test artifacts
clean:
	@echo "🧹 Cleaning all test artifacts..."
	@for dir in $(SUBDIRS); do \
		echo "  Cleaning $$dir..."; \
		$(MAKE) -C $$dir clean; \
	done
	@echo "✅ All test artifacts cleaned"

# Run all tests with comprehensive reporting
test: all
	@echo "🧪 Running all unit tests..."
	@echo "Found test modules: $(SUBDIRS)"
	@echo ""
	@total_modules=0; \
	passed_modules=0; \
	total_tests=0; \
	passed_tests=0; \
	failed_modules=""; \
	for dir in $(SUBDIRS); do \
		echo "📁 Testing module: $$dir"; \
		total_modules=$$((total_modules + 1)); \
		module_tests=0; \
		module_passed=0; \
		module_failed=false; \
		for bin in $$dir/bin/*Test; do \
			if [ -x "$$bin" ]; then \
				module_tests=$$((module_tests + 1)); \
				total_tests=$$((total_tests + 1)); \
				echo "  ▶️  Running $$(basename $$bin)..."; \
				if "$$bin"; then \
					module_passed=$$((module_passed + 1)); \
					passed_tests=$$((passed_tests + 1)); \
					echo "    ✅ PASSED"; \
				else \
					echo "    ❌ FAILED"; \
					module_failed=true; \
				fi; \
			fi; \
		done; \
		if [ $$module_tests -eq 0 ]; then \
			echo "  ⚠️  No test executables found"; \
		elif [ "$$module_failed" = true ]; then \
			echo "  📊 Module result: $$module_passed/$$module_tests tests passed - ❌ FAILED"; \
			failed_modules="$$failed_modules $$dir"; \
		else \
			echo "  📊 Module result: $$module_passed/$$module_tests tests passed - ✅ PASSED"; \
			passed_modules=$$((passed_modules + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "=========================================="; \
	echo "📊 FINAL RESULTS:"; \
	echo "  Modules: $$passed_modules/$$total_modules passed"; \
	echo "  Tests:   $$passed_tests/$$total_tests passed"; \
	if [ $$passed_modules -ne $$total_modules ]; then \
		echo "  ❌ Failed modules:$$failed_modules"; \
		echo "=========================================="; \
		exit 1; \
	else \
		echo "  ✅ All tests passed!"; \
		echo "=========================================="; \
	fi

# ============================================================================
# UTILITY TARGETS - Help and debugging
# ============================================================================

# Show help information
help:
	@echo "📋 CppBoilerplate Test Makefile"
	@echo ""
	@echo "🎯 Available targets:"
	@echo "  all                    - Build all test modules"
	@echo "  test                   - Build and run all tests with comprehensive reporting"
	@echo "  clean                  - Clean all test artifacts"
	@echo ""
	@echo "🔧 Individual module targets:"
	@echo "  test-run-<module>      - Build and run tests for a specific module"
	@echo "  test-make-<module>     - Build tests for a specific module (without running)"
	@echo "  test-clean-<module>    - Clean artifacts for a specific module"
	@echo "  test-debug-<module>    - Show debug configuration for a specific module"
	@echo ""
	@echo "📁 Available test modules:"
	@for dir in $(SUBDIRS); do \
		echo "  - $$dir"; \
	done
	@echo ""
	@echo "💡 Examples:"
	@echo "  make test-run-LoggerTest     # Run Logger tests"
	@echo "  make test-debug-ServiceATest # Debug ServiceA test configuration"
	@echo "  make test                    # Run all tests with full reporting"

# Debug all module configurations
debug-all:
	@echo "🐛 Debug configuration for all modules:"
	@echo ""
	@for dir in $(SUBDIRS); do \
		echo "📁 Module: $$dir"; \
		if grep -q "debug-config:" "$$dir/Makefile" 2>/dev/null; then \
			$(MAKE) -C $$dir debug-config 2>/dev/null || echo "  ⚠️  Debug failed"; \
		else \
			echo "  ⚠️  No debug-config target available"; \
		fi; \
		echo ""; \
	done

# Show project statistics
stats:
	@echo "📊 Test Project Statistics:"
	@echo ""
	@module_count=0; \
	test_file_count=0; \
	cpp_line_count=0; \
	for dir in $(SUBDIRS); do \
		if [ -d "$$dir" ]; then \
			module_count=$$((module_count + 1)); \
			if [ -d "$$dir/cases" ]; then \
				case_count=$$(find "$$dir/cases" -name "*.cpp" 2>/dev/null | wc -l); \
				test_file_count=$$((test_file_count + case_count)); \
				lines=$$(find "$$dir/cases" -name "*.cpp" -exec wc -l {} + 2>/dev/null | tail -1 | awk '{print $$1}' || echo 0); \
				cpp_line_count=$$((cpp_line_count + lines)); \
			fi; \
		fi; \
	done; \
	echo "  📁 Test modules: $$module_count"; \
	echo "  📄 Test files:  $$test_file_count"; \
	echo "  📝 Lines of test code: $$cpp_line_count"; \
	echo ""
