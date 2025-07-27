all: client server

client:client.o 
		gcc -Wall -o $@ $^ -lncurses

server:server.o 
		gcc -Wall -o $@ $^ -lncurses

client.o:client.c
		gcc -Wall -c $<
		
server.o:server.c
		gcc -Wall -c $<
clean:
		rm -rf *.o client server
		