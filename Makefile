CFLAGS = -Wall -Wextra -Werror -O3 -g
SRCS = main.c
OBJS = $(SRCS:.c=.o)
NAME = c_debugger

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -L. -lreadline -lcurses -o $(NAME)
clean:
	rm -f $(OBJS)
fclean: clean
	rm -f $(NAME)
re: fclean all
.PHONY: all clean fclean re
