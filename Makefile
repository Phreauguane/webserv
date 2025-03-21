NAME	=	webserv

CPP		=	c++ -Wall -Wextra -Werror -std=c++98 -g3

INCLUDE =	include

DEBUG_MODE = -D LOG_LEVEL=TEXT

RELEASE_MODE = -D LOG_LEVEL=INFO

SRC		=	main.cpp			\
			src/Config.cpp		\
			src/Server.cpp		\
			src/Location.cpp	\
			src/Utils.cpp		\
			src/Logger.cpp		\
			src/Client.cpp		\
			src/Request.cpp		\
			src/Response.cpp	\
			src/Cgi.cpp			\
			src/Session.cpp		\
			src/ISessionManager.cpp

all		:
	$(CPP) -I $(INCLUDE) $(DEBUG_MODE) $(SRC) -o $(NAME)

vg		:
	$(CPP) -I $(INCLUDE) -g3 $(DEBUG_MODE) $(SRC) -o $(NAME)

debug	:
	$(CPP) -I $(INCLUDE) $(DEBUG_MODE) $(SRC) -o $(NAME)

clean	:
	rm -f $(NAME)
	rm -f *.log

fclean	:	clean

re		:	fclean all