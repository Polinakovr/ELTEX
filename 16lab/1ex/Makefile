all:localTCPclient localTCPserver localUDPclient localUDPserver
localTCPclient: localTCPclient.o
		gcc -Wall -o $@ $^

localTCPserver: localTCPserver.o
		gcc -Wall -o $@ $^ -g 

localTCPserver.o: localTCPserver.c
		gcc -Wall -c $< 

localTCPclient.o: localTCPclient.c
		gcc -Wall -c $< 
localUDPclient: localUDPclient.o
		gcc -Wall -o $@ $^

localUDPserver: localUDPserver.o
		gcc -Wall -o $@ $^ -g 

localUDPserver.o: localUDPserver.c
		gcc -Wall -c $< 

localUDPclient.o: localUDPclient.c
		gcc -Wall -c $< 
clean:
		rm -rf *.o localTCPclient localTCPserver localUDPclient localUDPserver