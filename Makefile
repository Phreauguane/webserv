NAME	=	webserv

CPP		=	c++ -Wall -Wextra -Werror -std=c++98 -g3

INCLUDE =	include

SRC		=	main.cpp			\
			src/Config.cpp		\
			src/Server.cpp		\
			src/Location.cpp	\
			src/Utils.cpp
			

all		:
	$(CPP) -I $(INCLUDE) $(SRC) -o $(NAME)

clean	:
	rm -f $(NAME)

fclean	:	clean

re		:	fclean all