all: client server
clientt: client.o
		gcc -Wall -o $@ $<

client.o:client.c
		gcc -Wall -c $<

server:server.o
		gcc -Wall -o $@ $<

server.o:server.c
		gcc -Wall -c $<


clean:
		rm -rf client server *.o