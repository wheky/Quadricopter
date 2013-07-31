
CXX       ?= g++
CXXFLAGS  += -W -Wall -Wextra -DDMP_FIFO_RATE=9 -O2
NAME      = server
SRC       = main.cpp			\
	    Network.cpp 		\
	    streamGyro/MPU6050.cpp	\
	    streamGyro/demo_dmp.cpp	\
	    streamGyro/I2Cdev.cpp


all: depend $(NAME)

depend: .depend

.depend: $(SRC)
	    @$(RM) .depend
		@$(CXX) $(CXXFLAGS) -MM $^ > .depend

include .depend


OBJ     = $(SRC:.cpp=.o)

$(NAME): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all depend clean fclean all re
