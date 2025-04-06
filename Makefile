.IGNORE:
.SILENT:

NAME	=	webserv

CPP		=	c++ -Wall -Wextra -Werror -std=c++98

INCLUDE =	include

DEBUG_MODE = -D LOG_LEVEL=TEXT

RELEASE_MODE = -D LOG_LEVEL=INFO

MAKEFLAGS += --no-print-directory

SRC			=	main.cpp		\
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

TESTER_SRC	=	tester_src/main.cpp	\

OBJ		=	$(SRC:.cpp=.o)
DEBUG_OBJ = $(SRC:.cpp=.debug.o)
VG_OBJ = $(SRC:.cpp=.vg.o)

# Couleurs
GREEN	=	\e[0;32m
RED		=	\e[0;31m
RESET	=	\e[0m
BOLD	=	\e[1m
CYAN	=	\e[0;36m
YELLOW  =   \e[0;33m

# Barre de chargement
BAR_SIZE = 63
SLEEP_TIME = 0.005

define TITLE
echo          "$(GREEN) 🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬰🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒 "
echo          ""
echo          "$(GREEN) 🭁🭞🭜🭘                                                       🭣🭧🭓🭌"
echo          "$(GREEN) 🭡                                                             🭖"
echo  "$(GREEN) 🭛$(CYAN) ██╗    ██╗███████╗██████╗ ███████╗███████╗██████╗ ██╗   ██╗ $(GREEN)🭦$(RESET)"
echo           "$(CYAN)   ██║    ██║██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██║   ██║  $(RESET)"
echo           "$(CYAN)   ██║ █╗ ██║█████╗  ██████╔╝███████╗█████╗  ██████╔╝██║   ██║  $(RESET)"
echo           "$(CYAN)   ██║███╗██║██╔══╝  ██╔══██╗╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝  $(RESET)"
echo           "$(CYAN)   ╚███╔███╔╝███████╗██████╔╝███████║███████╗██║  ██║ ╚████╔╝   $(RESET)"
echo  "$(GREEN) 🭀$(CYAN)  ╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝   $(GREEN)🭋$(RESET)"
echo          "$(GREEN) 🭐                                                             🭅"
echo          "$(GREEN) 🭒🭍🭑🬽                                                       🭈🭆🭂🭝"
echo 		  ""
endef

define TITLE_DEBUG
echo          "$(YELLOW) 🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬰🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒 "
echo          ""
echo          "$(YELLOW) 🭁🭞🭜🭘                                                       🭣🭧🭓🭌"
echo          "$(YELLOW) 🭡                                                             🭖"
echo  "$(YELLOW) 🭛$(CYAN) ██╗    ██╗███████╗██████╗ ███████╗███████╗██████╗ ██╗   ██╗ $(YELLOW)🭦$(RESET)"
echo           "$(CYAN)   ██║    ██║██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██║   ██║  $(RESET)"
echo           "$(CYAN)   ██║ █╗ ██║█████╗  ██████╔╝███████╗█████╗  ██████╔╝██║   ██║  $(RESET)"
echo           "$(CYAN)   ██║███╗██║██╔══╝  ██╔══██╗╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝  $(RESET)"
echo           "$(CYAN)   ╚███╔███╔╝███████╗██████╔╝███████║███████╗██║  ██║ ╚████╔╝   $(RESET)"
echo  "$(YELLOW) 🭀$(CYAN)  ╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝   $(YELLOW)🭋$(RESET)"
echo          "$(YELLOW) 🭐                                                             🭅"
echo          "$(YELLOW) 🭒🭍🭑🬽                                                       🭈🭆🭂🭝"
echo 		  ""
endef

define TITLE_DEL
echo          "$(RED) 🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬡🬰🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒🬒 "
echo          ""
echo          "$(RED) 🭁🭞🭜🭘                                                       🭣🭧🭓🭌"
echo          "$(RED) 🭡                                                             🭖"
echo  "$(RED) 🭛$(CYAN) ██╗    ██╗███████╗██████╗ ███████╗███████╗██████╗ ██╗   ██╗ $(RED)🭦$(RESET)"
echo           "$(CYAN)   ██║    ██║██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██║   ██║  $(RESET)"
echo           "$(CYAN)   ██║ █╗ ██║█████╗  ██████╔╝███████╗█████╗  ██████╔╝██║   ██║  $(RESET)"
echo           "$(CYAN)   ██║███╗██║██╔══╝  ██╔══██╗╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝  $(RESET)"
echo           "$(CYAN)   ╚███╔███╔╝███████╗██████╔╝███████║███████╗██║  ██║ ╚████╔╝   $(RESET)"
echo  "$(RED) 🭀$(CYAN)  ╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝   $(RED)🭋$(RESET)"
echo          "$(RED) 🭐                                                             🭅"
echo          "$(RED) 🭒🭍🭑🬽                                                       🭈🭆🭂🭝"
echo 		  ""
endef

TOTAL_FILES = $(words $(SRC))
COUNTER = 0
PERCENT = 0

%.o: %.cpp
	@$(CPP) -I $(INCLUDE) -c $< -o $@ 2>> errors.log
	@$(eval COUNTER=$(shell echo $$(($(COUNTER) + 1))))
	@$(eval DONE=$(shell echo $$(($(COUNTER) * $(BAR_SIZE) / $(TOTAL_FILES)))))
	@printf "$(GREEN)"
	@printf "\r🮤"
	@for i in `seq 1 $(DONE)`; do printf "█"; done
	@for i in `seq 1 $$(($(BAR_SIZE) - $(DONE)))`; do printf "░"; done
	@printf "🮥"
	@printf "$(RESET)"

%.debug.o: %.cpp
	@$(CPP) $(DEBUG_MODE) -g3 -I $(INCLUDE) -c $< -o $@ 2>> errors.log
	@$(eval COUNTER=$(shell echo $$(($(COUNTER) + 1))))
	@$(eval DONE=$(shell echo $$(($(COUNTER) * $(BAR_SIZE) / $(TOTAL_FILES)))))
	@printf "$(YELLOW)"
	@printf "\r🮤"
	@for i in `seq 1 $(DONE)`; do printf "█"; done
	@for i in `seq 1 $$(($(BAR_SIZE) - $(DONE)))`; do printf "░"; done
	@printf "🮥"
	@printf "$(RESET)"

all: clear $(NAME)

$(NAME): pre_compile $(OBJ)
	@$(CPP) $(OBJ) -o $(NAME) 2>> errors.log
	@echo "$(GREEN)$(BOLD)\n\n 🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮  Done!  🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯\n$(RESET)"
	@echo "$(RED)"
	@cat errors.log
	@echo "$(RESET)"

pre_compile: clear
	@$(call TITLE)
	@printf "$(GREEN)"
	@printf "🮤"
	@for i in `seq 1 $(BAR_SIZE)`; do printf "░"; done
	@printf "🮥"
	@printf "$(RESET)"
	@$(eval COUNTER=0)

debug: pre_compile_special $(DEBUG_OBJ)
	@$(CPP) -I $(INCLUDE) $(DEBUG_MODE) -g3 $(TESTER_SRC) -o tester 2>> errors.log
	@$(CPP) $(DEBUG_MODE) -g3 $(DEBUG_OBJ) -o $(NAME)  2>> errors.log
	@echo "$(YELLOW)$(BOLD)\n\n 🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮  Done!  🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯\n$(RESET)"
	@echo "$(RED)"
	@cat errors.log
	@echo "$(RESET)"

pre_compile_special: clear
	@$(call TITLE_DEBUG)
	@printf "$(YELLOW)"
	@printf "🮤"
	@for i in `seq 1 $(BAR_SIZE)`; do printf "░"; done
	@printf "🮥"
	@printf "$(RESET)"
	@$(eval COUNTER=0)

clear:
	@clear

clean: clear
	@$(call TITLE_DEL)
	@for i in `seq 1 $(BAR_SIZE)`; do \
		printf "$(RED)"; \
		printf "\r🮤"; \
		for j in `seq 1 $$i`; do printf "█"; done; \
		for j in `seq 1 $$(($(BAR_SIZE) - $$i))`; do printf "░"; done; \
		printf "🮥"; \
		printf "$(RESET)"; \
		sleep $(SLEEP_TIME); \
	done
	@printf "$(RED)"
	@printf "\r🮤"
	@for i in `seq 1 $(BAR_SIZE)`; do printf "█"; done
	@printf "🮥\n"
	@printf "$(RESET)"
	@for i in $(OBJ) $(DEBUG_OBJ) $(VG_OBJ); do \
		if [ -f $$i ]; then \
			rm -f $$i; \
		fi; \
	done
	@rm -f *.log
	@echo "$(RED)$(BOLD)\n 🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮  Done!  🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯$(RESET)"

fclean: clear
	@$(call TITLE_DEL)
	@for i in `seq 1 $(BAR_SIZE)`; do \
		printf "$(RED)"; \
		printf "\r🮤"; \
		for j in `seq 1 $$i`; do printf "█"; done; \
		for j in `seq 1 $$(($(BAR_SIZE) - $$i))`; do printf "░"; done; \
		printf "🮥"; \
		printf "$(RESET)"; \
		sleep $(SLEEP_TIME); \
	done
	@printf "$(RED)"
	@printf "\r🮤"
	@for i in `seq 1 $(BAR_SIZE)`; do printf "█"; done
	@printf "🮥\n"
	@printf "$(RESET)"
	@for i in $(OBJ) $(DEBUG_OBJ) $(VG_OBJ); do \
		if [ -f $$i ]; then \
			rm -f $$i; \
		fi; \
	done
	@if [ -f $(NAME) ]; then \
		rm -f $(NAME); \
	fi
	@rm -f *.log
	@rm -f tester
	@echo "$(RED)$(BOLD)\n 🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮  Done!  🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯$(RESET)"

re:
	@make fclean
	@make all

dre:
	@make fclean
	@make debug

git:
	@make fclean
	@echo "$(CYAN)"
	@git add .
	@git commit -m "Auto commit"
	@git push
	@echo "$(GREEN)$(BOLD)\n 🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮🬮  Done!  🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯🬯\n$(RESET)"

.PHONY: all clean fclean re vg debug pre_compile pre_compile_special clear git