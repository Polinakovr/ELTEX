CC:=gcc
CFLAGS:=-Wall -Wextra -g 
SRC:= add.c sub.c mul.c div.c calcul.c
OBJ:=$(SRC:.c=.o)


main: $(OBJ) library.a
		$(CC) $(CFLAGS) -o $@ $(OBJ) -L. -lrary

%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

library.a: $(OBJ)
		ar rcs $@ $(OBJ)
		
run: main
		./main
clean:
		rm -rf main *.o library.a