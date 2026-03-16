main: main.c display.c utils.c
	gcc -g main.c display.c utils.c -o main -lncursesw -lm
