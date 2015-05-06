CC=gcc
CFLAGS = -I. -g
OBJ = client.o server.o func.o
SERV_OBJ = server.o func.o
CLI_OBJ = client.o func.o
SEC_OBJ = second.o func.o
DEPS = func.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: server client second

server: $(SERV_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

client: $(CLI_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

second: $(SEC_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


select: select.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o server client select
