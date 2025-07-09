CC:=gcc
CFLAGS:= -Wall -Wextra
test: main.o listdirectory.o drawpanel.o initpanels.o
		$(CC) $(CFLAFS) -o $@ $^ -lncurses

listdirectory.o: listdirectory.c
		$(CC) $(CFLAFS) -c  $<

main.o: main.c 
		$(CC) $(CFLAFS) -c $<		 

drawpanel.o: drawpanel.c
		$(CC) $(CFLAFS) -c  $<

initpanels.o: initpanels.c
		$(CC) $(CFLAFS) -c  $<

clean:
		rm -rf *.o test