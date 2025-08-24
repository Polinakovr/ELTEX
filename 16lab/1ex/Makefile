all:localTCPclient localTCPserver localUDPclient localUDPserver inetTCPserver inetTCPclient inetUDPclient inetUDPserver
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
		
inetTCPclient: inetTCPclient.o
		gcc -Wall -o $@ $^

inetTCPserver: inetTCPserver.o
		gcc -Wall -o $@ $^ -g 

inetTCPserver.o: inetTCPserver.c
		gcc -Wall -c $< 

inetTCPclient.o: inetTCPclient.c
		gcc -Wall -c $< 

inetUDPclient: inetUDPclient.o
		gcc -Wall -o $@ $^

inetUDPserver: inetUDPserver.o
		gcc -Wall -o $@ $^ -g 

inetUDPserver.o: inetUDPserver.c
		gcc -Wall -c $< 

inetUDPclient.o: inetUDPclient.c
		gcc -Wall -c $< 	
clean:
		rm -rf *.o localTCPclient localTCPserver localUDPclient localUDPserver inetTCPserver inetTCPclient inetUDPclient inetUDPserver