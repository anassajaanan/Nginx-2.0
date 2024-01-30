# Project Name
NAME = webserver
DEBUG_NAME = $(NAME)
RELEASE_NAME = $(NAME)_release

# Source and Include Directories
# replace it later with src/
SRCS_DIR = ./
# replace it later with include/
INCL_DIR = ./

# Source and Header Files
SRCS = 	BaseConfig.cpp CgiDirective.cpp ClientState.cpp ConfigLoader.cpp ConfigNode.cpp ConfigParser.cpp ConfigTokenizer.cpp ContextNode.cpp \
		DirectiveNode.cpp HttpRequest.cpp HttpResponse.cpp KqueueManager.cpp LocationConfig.cpp Logger.cpp LogicValidator.cpp MimeTypeConfig.cpp \
		MimeTypeParser.cpp RequestHandler.cpp ResponseState.cpp ReturnDirective.cpp Server.cpp ServerConfig.cpp ServerManager.cpp SyntaxValidator.cpp \
		TreeBuilder.cpp TryFilesDirective.cpp CgiHandler.cpp main.cpp EventPoller.cpp

HEADERS = $(wildcard $(INCL_DIR)*.hpp)

# Compiler and Common Flags
CXX = clang++
CXXFLAGS_COMMON = -Wall -Wextra -Werror -std=c++98 -I$(INCL_DIR)

# Build Directories
BUILD_DIR = build/
DEBUG_DIR = $(BUILD_DIR)debug/
RELEASE_DIR = $(BUILD_DIR)release/

# Object and Dependency Files for Different Configurations
OBJS_DEBUG = $(addprefix $(DEBUG_DIR), $(SRCS:.cpp=.o))
DEPS_DEBUG = $(OBJS_DEBUG:.o=.d)

OBJS_RELEASE = $(addprefix $(RELEASE_DIR), $(SRCS:.cpp=.o))
DEPS_RELEASE = $(OBJS_RELEASE:.o=.d)

# Development Flags
CXXFLAGS_DEV = $(CXXFLAGS_COMMON) -g3 -Wpedantic -Wshadow -Wcast-align -Wcast-qual -Wunused \
			-Wmissing-prototypes -Woverloaded-virtual -Wmisleading-indentation -Wnon-virtual-dtor\
			-fsanitize=address -fsanitize=undefined -fstack-protector-strong -fstrict-overflow

# Production Flags
CXXFLAGS_PROD = $(CXXFLAGS_COMMON) -O3 -march=native -flto -fstack-protector-strong -D_FORTIFY_SOURCE=2

# Default Build (Set to debug)
all: debug

# Debug Build
debug: CXXFLAGS = $(CXXFLAGS_DEV)
debug: $(DEBUG_NAME)

# Release Build
prod: CXXFLAGS = $(CXXFLAGS_PROD)
prod: $(RELEASE_NAME)

# Linking for Debug
$(DEBUG_NAME): $(OBJS_DEBUG)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Linking for Release
$(RELEASE_NAME): $(OBJS_RELEASE)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilation for Debug
$(DEBUG_DIR)%.o: $(SRCS_DIR)%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Compilation for Release
$(RELEASE_DIR)%.o: $(SRCS_DIR)%.cpp $(HEADERS)
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

.PHONY: all debug prod clean fclean re