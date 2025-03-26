NAME	:= webserv
CC		:= c++
FLAGS	:= -Wall -Wextra -Werror -Iinclude -std=c++17


OBJ_DIR	:= obj
SRC_DIR	:= src

HDRS	:= $(find include -name "*.hpp")

#keep main at the end
FILES	:=	parsing_helper/ParsingHelper \
			configuration_file/ConfigBlock \
			configuration_file/Location \
			configuration_file/Directives \
			Server ServerManager \
			HttpParser \
			Scanner \
			Token \
			debug \
			Logger \
			main

SRCS	:= $(addprefix ${SRC_DIR}/,$(FILES:=.cpp))

OBJS	:= $(addprefix $(OBJ_DIR)/,$(FILES:=.o))


all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS) $(HDRS)
	$(CC) $(OBJS) $(FLAGS) -o $(NAME)


$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
