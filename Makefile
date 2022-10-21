NAME= microserv
SRCS= webserver_test.cpp
#HEADER=
OBJS=$(SRCS:.cpp=.o)
COMPIL= c++
FLAGS= -std=c++98 -Wall -Wextra -Werror

%.o:	%.cpp
	$(COMPIL) $(FLAGS) -c $< -o $@
all:	$(NAME)

$(NAME):	$(OBJS) #$(HEADER)
	$(COMPIL) $(FLAGS) $(OBJS) -o $@

clean:
	rm -rf $(OBJS)

fclean:	clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
