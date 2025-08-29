all: clientsimpleTCP serversimpleTCP serverUDPsimple clientUDPsimple poolTCPserver produceserverTCP poolUDPserver produceserverUDP tcpudp
clientsimpleTCP: clientsimpleTCP.o
		gcc -Wall -o $@ $<

serversimpleTCP.o:serversimpleTCP.c
		gcc -Wall -c $< -pthread

serversimpleTCP:serversimpleTCP.o
		gcc -Wall -o $@ $<

clientsimpleTCP.o:clientsimpleTCP.c
		gcc -Wall -c $<

clientUDPsimple: clientUDPsimple.o
		gcc -Wall -o $@ $<

serverUDPsimple.o:serverUDPsimple.c
		gcc -Wall -c $< -pthread

serverUDPsimple:serverUDPsimple.o
		gcc -Wall -o $@ $<

clientUDPsimple.o:clientUDPsimple.c
		gcc -Wall -c $<

poolTCPserver.o:poolTCPserver.c
		gcc -Wall -c $< -pthread

poolTCPserver:poolTCPserver.o
		gcc -Wall -o $@ $<
poolUDPserver.o:poolUDPserver.c
		gcc -Wall -c $< -pthread

poolUDPserver:poolUDPserver.o
		gcc -Wall -o $@ $<		
produceserverTCP.o:produceserverTCP.c
		gcc -Wall -c $< -pthread -lrt

produceserverTCP:produceserverTCP.o
		gcc -Wall -o $@ $<

produceserverUDP.o:produceserverUDP.c
		gcc -Wall -c $< -pthread -lrt

produceserverUDP:produceserverUDP.o
		gcc -Wall -o $@ $<
tcpudp.o:tcpudp.c
		gcc -Wall -c $<
tcpudp:tcpudp.o
		gcc -Wall -o $@ $<
clean:
		rm -rf clientsimpleTCP serversimpleTCP serverUDPsimple clientUDPsimple poolTCPserver poolUDPserver produceserverTCP produceserverUDP tcpudp *.o 