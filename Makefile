##
## EPITECH PROJECT, 2025
## Makefile
## File description:
## Makefile
##


SRC_COMMON = src/server/Server.cpp \
			 src/client/Client.cpp \
			 src/Utils/Utils.cpp \
			 src/Parser/Parser.cpp \
			 src/Protocole/Protocole.cpp

SRC_SERVER = src/main_server.cpp $(SRC_COMMON)

SRC_CLIENT = src/main_client.cpp \
			 src/Game/Game.cpp $(SRC_COMMON)

OBJ_SERVER = $(SRC_SERVER:.cpp=.o)
OBJ_CLIENT = $(SRC_CLIENT:.cpp=.o)

NAME_SERVER = jetpack_server
NAME_CLIENT = jetpack_client

CXX = g++
CXXFLAGS = -Wall -Wextra -pthread -I./src
LDFLAGS_CLIENT = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network


all: server client

server: $(OBJ_SERVER)
	$(CXX) $(OBJ_SERVER) -o $(NAME_SERVER) $(CXXFLAGS)

client: $(OBJ_CLIENT)
	$(CXX) $(OBJ_CLIENT) -o $(NAME_CLIENT) $(CXXFLAGS) $(LDFLAGS_CLIENT)

clean:
	rm -f $(OBJ_SERVER) $(OBJ_CLIENT)

fclean: clean
	rm -f $(NAME_SERVER) $(NAME_CLIENT)

re: fclean all