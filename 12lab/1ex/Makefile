test: main.o
		gcc -Wall -o $@ $< 

main.o: main.c
		gcc -Wall -c $< 

clean:
		rm -rf *.o test