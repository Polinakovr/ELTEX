
test: main.o output.txt
		gcc -Wall -o  $@ $<

output.txt:
		touch output.txt

main.o: main.c 
		gcc -Wall -c $< 

clean: 
		rm -rf *.o output.txt test