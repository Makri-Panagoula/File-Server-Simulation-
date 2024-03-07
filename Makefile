# Compiler
CC = gcc
HEADERS = ../headers
SRC = ./source

# Compile Options
CFLAGS = -I -g -pthread -I$(HEADERS)

# make server
SERVER = server

# Object files 
OBJ = $(SRC)/server.o $(SRC)/helper.o $(SRC)/client.o

$(SERVER) : $(OBJ) 
	$(CC) $(CFLAGS) $(OBJ) -o $(SERVER)  -lrt -lm

run : $(SERVER)
	./$(SERVER) 4 4 10 2 

clean:
	rm -f $(OBJ) $(SERVER) output_* .nfs*