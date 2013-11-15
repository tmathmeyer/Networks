# Ted Meyer
# 03/11/2013

all: client server

server: server.o
	gcc -w -lpthread -g -o server server.c 
	rm server.o

client: client.o
	gcc -w -lpthread -g -o client client.c
	rm client.o

clean:
	rm -f *.o out.txt server client
