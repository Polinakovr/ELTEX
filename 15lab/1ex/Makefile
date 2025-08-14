all: main main_to_signal
main: main.o
		gcc -Wall -o $@ $<

main.o:main.c
		gcc -Wall -c $<

main_to_signal: main_to_signal.o
		gcc -Wall -o $@ $<

main_to_signal.o:main_to_signal.c
		gcc -Wall -c $<

clean:
		rm -rf main main_to_signal *.o