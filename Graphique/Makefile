SRC	= main.cpp \
	  env.cpp \
	  arrow.cpp

NAME	= quadTester

CXXFLAGS	= -Wall -Wextra -W

LDFLAGS	= -L./ -lglfw -lGL

OBJ	= $(SRC:.cpp=.o)

RM	= rm -f

CXX	= clang++

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(NAME)

clean:
	$(RM) $(OBJ) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

