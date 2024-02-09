# Project Name
NAME = webserver
DEBUG_NAME = $(NAME)_debug
RELEASE_NAME = $(NAME)

# Detect Operating System
UNAME_S := $(shell uname -s)

# replace it later with include/
INCL_DIR = src/


# Source Files Organized by Directory
SRC_DIRS = cgi config event_polling http logging parsing server
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard src/$(dir)/*.cpp))
SRCS += src/main.cpp


# Compiler and Common Flags
CXX = clang++

CXXFLAGS_COMMON = -Wall -Wextra -Werror -I$(INCL_DIR)

# Build Directories
BUILD_DIR = build/
DEBUG_DIR = $(BUILD_DIR)debug/
RELEASE_DIR = $(BUILD_DIR)release/

# Object Files for Different Configurations
OBJS_DEBUG = $(SRCS:src/%.cpp=$(DEBUG_DIR)%.o)
OBJS_RELEASE = $(SRCS:src/%.cpp=$(RELEASE_DIR)%.o)

# Include Dependency Files
DEPS_DEBUG = $(OBJS_DEBUG:.o=.d)
DEPS_RELEASE = $(OBJS_RELEASE:.o=.d)

# Development Flags
ifeq ($(UNAME_S),Linux)
	CXXFLAGS_DEV = $(CXXFLAGS_COMMON) -g3 -Wpedantic -Wshadow -Wcast-align -Wcast-qual -Wunused \
		-Wmissing-prototypes -Woverloaded-virtual -Wmisleading-indentation -Wnon-virtual-dtor\
		-fstack-protector-strong -fstrict-overflow
endif

ifeq ($(UNAME_S),Darwin)
	CXXFLAGS_DEV = $(CXXFLAGS_COMMON) -g3 -Wpedantic -Wshadow -Wcast-align -Wcast-qual -Wunused \
		-Wmissing-prototypes -Woverloaded-virtual -Wmisleading-indentation -Wnon-virtual-dtor\
		-fsanitize=address -fsanitize=undefined -fstack-protector-strong -fstrict-overflow
endif

# Production Flags
CXXFLAGS_PROD = $(CXXFLAGS_COMMON) -O3 -march=native -flto -fstack-protector-strong -D_FORTIFY_SOURCE=2

# Default Build (Set to debug)
all: prod

# Debug Build
debug: CXXFLAGS = $(CXXFLAGS_DEV)
debug: $(DEBUG_NAME)

# Release Build
prod: CXXFLAGS = $(CXXFLAGS_PROD)
prod: $(RELEASE_NAME)

# Linking for Debug
$(DEBUG_NAME): $(OBJS_DEBUG)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Linking for Release
$(RELEASE_NAME): $(OBJS_RELEASE)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $^


# Compilation for Debug and Release
$(DEBUG_DIR)%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(RELEASE_DIR)%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Include Dependency Files
-include $(DEPS_DEBUG) $(DEPS_RELEASE)

# Clean Objects and Dependencies
clean:
	rm -rf $(BUILD_DIR)

# Full Clean (Objects, Dependencies, and Binaries)
fclean: clean
	rm -f $(DEBUG_NAME) $(RELEASE_NAME)

# Rebuild
re: fclean all


# Valgrind Execution
valgrind: debug
	@if which valgrind valgrind >/dev/null 2>&1 && [ "$$(uname -s)" = "Linux" ]; then \
		valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --trace-children=yes ./$(DEBUG_NAME); \
	else \
		echo "Valgrind not found or not on Linux"; \
	fi

.PHONY: all debug prod clean fclean re