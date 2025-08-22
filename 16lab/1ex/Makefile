all:localTCPclient localTCPserver
localTCPclient: localTCPclient.o
		gcc -Wall -o $@ $^

localTCPserver: localTCPserver.o
		gcc -Wall -o $@ $^ -g 

localTCPserver.o: localTCPserver.c
		gcc -Wall -c $< 

localTCPclient.o: localTCPclient.c
		gcc -Wall -c $< 
clean:
		rm -rf *.o localTCPclient localTCPserver