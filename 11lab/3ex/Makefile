
test: main.o loader.o routines.o task.txt
		gcc -Wall -g -o  $@ $< -pthread
		
task.txt:
		touch $@

main.o: main.c loader.o
		gcc -Wall -g -c $< 

loader.o: loader.c
		gcc -Wall -g -c $< 

routines.o: routines.c
		gcc -Wall -g -c $< 

clean:
		rm -rf *.o test task.txt