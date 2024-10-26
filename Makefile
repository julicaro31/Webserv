NAME = webserv
OBJ_DIR	= obj

SRCS = main.cpp \
		configuration_file/ConfigBlock.cpp \
		configuration_file/Directives.cpp

OBJS = ${SRCS:%.cpp=$(OBJ_DIR)/%.o}
INCLUDE = includes
CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++11 -Iconfiguration_file

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c -o $@ $^

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re