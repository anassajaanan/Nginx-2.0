
NAME = webserver

SRCS = BaseConfig.cpp \
		CgiDirective.cpp \
		ClientState.cpp \
		ConfigLoader.cpp \
		ConfigNode.cpp \
		ConfigParser.cpp \
		ConfigTokenizer.cpp \
		ContextNode.cpp \
		DirectiveNode.cpp \
		HttpRequest.cpp \
		HttpResponse.cpp \
		KqueueManager.cpp \
		LocationConfig.cpp \
		Logger.cpp \
		LogicValidator.cpp \
		MimeTypeParser.cpp \
		RequestHandler.cpp \
		ResponseState.cpp \
		ReturnDirective.cpp \
		Server.cpp \
		ServerConfig.cpp \
		ServerManager.cpp \
		SyntaxValidator.cpp \
		TreeBuilder.cpp \
		TryFilesDirective.cpp \
		main.cpp

OBJS = $(SRCS:.cpp=.o)

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean fclean re


