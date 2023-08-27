# Compiler
CC = gcc

# Compile Options
CFLAGS = -I -g -pthread 

# make server
SERVER = server

# Object files 
OBJ = server.o helper.o client.o

$(SERVER) : $(OBJ) 
	$(CC) $(CFLAGS) $(OBJ) -o $(SERVER)  -lrt -lm

run : $(SERVER)
	./$(SERVER) 4 4 10 2 

clean:
	rm -f $(OBJ) $(SERVER) output_* .nfs*