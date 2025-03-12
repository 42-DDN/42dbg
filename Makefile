CC = cc
CFLAGS = -Wall -Wextra -Werror -I./includes -O3 -g
SRCDIR = srcs
SRCS = main.c check_cmd.c utils_str.c
SRCS := $(addprefix $(SRCDIR)/,$(SRCS))
OBJS = $(SRCS:%.c=%.o)
HEADERS = dbg.h utils.h
NAME = 42dbg

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -L. -lreadline -lcurses -o $(NAME)
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	@rm -f $(OBJS)
fclean: clean
	@rm -f $(NAME)
re: fclean all
.PHONY: all clean fclean re
