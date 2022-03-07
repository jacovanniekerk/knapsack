CC=gcc

all: knapsack

knapsack: knapsack.c
	$(CC) -ansi -pedantic -Wall -Werror knapsack.c -o knapsack -lm

clean:
	rm -f knapsack 
   
