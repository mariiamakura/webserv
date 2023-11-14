# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/09 13:47:00 by fhassoun          #+#    #+#              #
#    Updated: 2023/11/14 11:21:29 by fhassoun         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	webserv

CC			=	c++
CFLAGS		=	-Wall -Werror -Wextra -std=c++98 -g3 -fsanitize=address

SRC_DIR		=	source/
OBJ_DIR		=	object/
INC_DIR		=	includes/

SRC_FILES	=	main.cpp Server.cpp Config.cpp helpers.cpp



				
OBJ_FILES	=	$(SRC_FILES:.cpp=.o)

SRC			=	$(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ			=	$(addprefix $(OBJ_DIR), $(OBJ_FILES))

all:  $(NAME)

$(NAME): $(OBJ)
		@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
		@echo "\033[0;32m[OK]\033[0m    \033[0;38;5;199mCompiling Executable\033[0m $(NAME)"

$(OBJ_DIR)%.o:$(SRC_DIR)%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -I $(INC_DIR) -o $@ -c $<
	@echo "\033[0;32m[OK]\033[0m    \033[0;38;5;199mCompiling\033[0m $(<F)"

clean:
	@rm -rf $(OBJ)
	@echo "\033[0;32m[OK]\033[0m    \033[0;38;5;44mRemoving objects\033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo "\033[0;32m[OK]\033[0m    \033[0;38;5;44mRemoving executable\033[0m"

re: fclean all 

.PHONY: all clean fclean re 