all: Chad.c
	gcc -Wall -Wextra -std=c99 -o out Chad.c -lGL -lGLU -lglut -lm

clean:
	rm -f out