all: client server
client: client.o
		gcc -Wall -o $@ $< -lc -lpthread

client.o:client.c
		gcc -Wall -c $<

server:server.o
		gcc -Wall -o $@ $<

server.o:server.c
		gcc -Wall -c $<


clean:
		rm -rf client server *.o