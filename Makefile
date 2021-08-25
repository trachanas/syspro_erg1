OBJS 		= main.o functions.o tree.o receiverFunctions.o walletID.o
SOURCE 		= main.c functions.c tree.c receiverFunctions.c walletID.c
HEAD        = main.h receiver.h tree.h walletID.h
OUT 		= bitcoin
CC			= gcc
FLAGS		= -c -w

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

main.o: main.c
	$(CC) $(FLAGS) main.c

functions.o: functions.c 
	$(CC) $(FLAGS) functions.c

tree.o: tree.c
	$(CC) $(FLAGS) tree.c

receiverFunctions.o: receiverFunctions.c
	$(CC) $(FLAGS) receiverFunctions.c

walletID.o: walletID.c
	$(CC) $(FLAGS) walletID.c

clean:
	rm -f $(OBJS) $(OUT)