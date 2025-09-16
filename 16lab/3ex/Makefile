all: broadcastclient broadcastserver multicastclient multicastserver
broadcastclient: broadcastclient.o
		gcc -Wall -o $@ $<

broadcastclient.o:broadcastclient.c
		gcc -Wall -c $<

broadcastserver:broadcastserver.o
		gcc -Wall -o $@ $<

broadcastserver.o:broadcastserver.c
		gcc -Wall -c $<

multicastclient:multicastclient.o
		gcc -Wall -o $@ $<

multicastclient.o:multicastclient.c
		gcc -Wall -c $<

multicastserver:multicastserver.o
		gcc -Wall -o $@ $<

multicastserver.o:multicastserver.c
		gcc -Wall -c $<

clean:
		rm -rf broadcastclient broadcastserver multicastclient multicastserver *.o