all: clientsimpleTCP serversimpleTCP
clientsimpleTCP: clientsimpleTCP.o
		gcc -Wall -o $@ $<

serversimpleTCP.o:serversimpleTCP.c
		gcc -Wall -c $< -pthread

serversimpleTCP:serversimpleTCP.o
		gcc -Wall -o $@ $<

clientsimpleTCP.o:clientsimpleTCP.c
		gcc -Wall -c $<

clean:
		rm -rf clientsimpleTCP serversimpleTCP *.o