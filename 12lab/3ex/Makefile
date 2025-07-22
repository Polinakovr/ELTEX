test: main.o
		gcc -Wall -g -o  $@ $< 

main.o: main.c
		gcc -Wall -g -c $< 

clean:
		rm -rf *.o test