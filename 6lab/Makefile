CC:=gcc
CFLAGS:=-Wall -Wextra -g 
run: main
		./main
main: main.o
		$(CC) $(CFLAGS)  $^ -o $@

main.o: main.c library.h
		$(CC) $(CFLAGS) -c  $< -o $@

clean:
		rm -rf main *.o