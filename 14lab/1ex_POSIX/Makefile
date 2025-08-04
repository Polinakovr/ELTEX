all:client server
client: client.o
		gcc -Wall -o $@ $^ -lrt

server: server.o
		gcc -Wall -o $@ $^ -lrt

server.o: server.c
		gcc -Wall -c $< 

client.o: client.c
		gcc -Wall -c $< 
clean:
		rm -rf *.o client server