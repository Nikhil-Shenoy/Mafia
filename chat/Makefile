CC=gcc
CFLAGS = -I. -g
OBJ = client.o server.o select.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: server client

server: server.o
	$(CC) -o $@ $^ $(CFLAGS)

client: client.o
	$(CC) -o $@ $^ $(CFLAGS)

select: select.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o server client
