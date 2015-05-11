CC=gcc
CFLAGS = -Wall -I. -g -std=gnu99
OBJ = server.o func.o
SERV_OBJ = server.o sock.o util.o ../structures/player.o ../structures/gameFlow.o
CLI_OBJ = func.o list.o
DEPS = func.h player.h list.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: server

server: $(SERV_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

select: select.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o server ../structures/*.o
