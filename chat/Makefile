CC=gcc
CFLAGS = -Wall -I. -g -std=gnu99
OBJ = client.o server.o func.o
SERV_OBJ = server.o func.o list.o
CLI_OBJ = client.o func.o list.o
DEPS = func.h client.h player.h list.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: server client

server: $(SERV_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

client: $(CLI_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

select: select.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o server client
