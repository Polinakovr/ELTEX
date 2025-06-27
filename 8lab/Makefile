CC:=gcc
CFLAGS:=-Wall -Wextra -g 
SRC:= add.c sub.c mul.c div.c 
OBJ:=$(SRC:.c=.o)

run: calcul
		./calcul
		
calcul: calcul.o library.so
		$(CC) $(CFLAGS) -o $@ calcul.o -L. -lrary -Wl,-rpath,.

library.so: $(OBJ)
		$(CC) -shared -o $@ $(OBJ)

%.o: %.c
		$(CC) $(CFLAGS) -fPIC -c $< -o $@

clean:
		rm -rf calcul *.o library.so