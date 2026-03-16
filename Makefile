main: main.c display.c
	gcc -g main.c display.c -o main -lncursesw -lm
