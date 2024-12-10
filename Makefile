all: 3DLighting.c
	gcc -Wall -Wextra -std=c99 -o out 3DLighting.c -lGL -lGLU -lglut -lm

clean:
	rm -f out