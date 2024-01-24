
NAME = webserver

SRCS =	BaseConfig.cpp \
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
		MimeTypeConfig.cpp \
		main.cpp

HEADERS =	BaseConfig.hpp \
			CgiDirective.hpp \
			ClientState.hpp \
			ConfigLoader.hpp \
			ConfigNode.hpp \
			ConfigParser.hpp \
			ConfigTokenizer.hpp \
			ContextNode.hpp \
			DirectiveNode.hpp \
			HttpRequest.hpp \
			HttpResponse.hpp \
			KqueueManager.hpp \
			LocationConfig.hpp \
			Logger.hpp \
			LogicValidator.hpp \
			MimeTypeParser.hpp \
			RequestHandler.hpp \
			ResponseState.hpp \
			ReturnDirective.hpp \
			Server.hpp \
			ServerConfig.hpp \
			ServerManager.hpp \
			SyntaxValidator.hpp \
			TreeBuilder.hpp \
			TryFilesDirective.hpp \
			MimeTypeConfig.hpp


OBJS_DIR = objs/


OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.cpp=.o))


CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address


all: $(NAME)


$(NAME): $(OBJS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)


$(OBJS_DIR)%.o: %.cpp $(HEADERS)
	@mkdir -p objs
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -fr objs
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean fclean re


