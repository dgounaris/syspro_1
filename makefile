OBJS = main.o bc_transaction.o transaction.o wallet.o service.o
SOURCE = main.c bc_transaction.c transaction.c wallet.c service.c
HEADER = bc_transaction.h transaction.h wallet.h service.h
OUT = main
CC = gcc
FLAGS = -o3 -lm

exec: $(OBJS)
	$(CC) $(OBJS) $(FLAGS) -o main

$(OUT): $(OBJS)
	$(CC) $(OBJS) -c $@

clean:
	rm -f $(OBJS) $(OUT)
