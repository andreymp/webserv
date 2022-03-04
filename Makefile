NAME = webserv

SRCS =	main.cpp \
		utils.cpp \
		cnf/Config.cpp \
		server/Server.cpp \
		request/Request.cpp \
		response/Response.cpp \
		response/ResponseHeader.cpp \
		server_handler/ServerHandler.cpp \

INC	= 	cnf/Config.hpp \
		server/Server.hpp \
		request/Request.hpp \
		server_handler/ServerHandler.hpp \
		webserv.hpp \

OBJS = $(SRCS:.cpp=.o)
OBJS_DIR = .obj

CPP	= c++
CPP_FLAGS = #-fsanitize=address #-Wall -Wextra -Werror -std=c++98 -g 
RM	= rm -rf

.PHONY:			all clean fclean re

$(OBJS_DIR)/%.o:		%.cpp Makefile
						@mkdir -p $(OBJS_DIR)
						@mkdir -p	$(OBJS_DIR)/cnf \
									$(OBJS_DIR)/server \
									$(OBJS_DIR)/request \
									$(OBJS_DIR)/server_handler \
									$(OBJS_DIR)/response
						$(CPP) $(CPP_FLAGS) -c $< -o $@

all:			$(NAME)

$(NAME):		$(addprefix $(OBJS_DIR)/, $(OBJS)) $(INC)
				$(CPP) $(CPP_FLAGS) $(addprefix $(OBJS_DIR)/, $(OBJS)) -o $(NAME)

clean:
				@$(RM) $(OBJS_DIR)

fclean:			clean
				@$(RM) $(NAME)

re:				fclean $(NAME)