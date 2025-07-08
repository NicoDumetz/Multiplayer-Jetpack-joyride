##
## EPITECH PROJECT, 2025
## jetpack
## File description:
## Makefile
##

NAME = jetpack

CC = g++

CFLAGS = -Wall -Wextra -Werror -std=c++11 -I src/ -I src/Game/ -I src/Graphics/ -I src/Parser/ -I src/Game/Map/ -I src/Game/Player/ -I src/Game/Zapper/ -I src/Game/Coin/

LDFLAGS += -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network

SRCS = src/main.cpp \
       src/Parser/Parser.cpp \
       src/Game/Player/Player.cpp \
       src/Game/Game.cpp \
       src/Game/Map/MapRenderer.cpp \
       src/Game/Map/Map.cpp \
       src/Game/GameObject.cpp \
       src/Game/Zapper/Zapper.cpp \
       src/Game/Coin/Coin.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re