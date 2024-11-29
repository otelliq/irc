CC = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g

SRC = main.cpp server.cpp server_infos.cpp Channel.cpp Client.cpp Parcing_data.cpp Channel_cmds.cpp 

OBJ = $(SRC:.cpp=.o)

NAME = ft_irc

.SILENT:

all: $(NAME)

$(NAME): $(OBJ)
			printf "\033[32mcompiling...\033[0m\n"
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)
			printf "\033[32mDONE\033[0m\n"

clean:
	rm -f $(OBJ)
		printf "\033[31mcleaning...\033[0m\n"

fclean: clean
	rm -f $(NAME)


re: fclean all

.PHONY: re fclean clean 