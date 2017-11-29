CC = clang
CFLAGS = -g

SRCS = linkedlist.c talloc.c tokenizer.c parser.c interpreter.c main.c
HDRS = linkedlist.h value.h talloc.h tokenizer.h parser.h interpreter.h
OBJS = $(SRCS:.c=.o)

linkedlist: $(OBJS)
	$(CC)  $(CFLAGS) $^  -o $@

memtest: linkedlist
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$<

tokenizer: $(OBJS)
	$(CC)  $(CFLAGS) main_tokenize.c -o tokenizer

parser: $(OBJS)
	$(CC)  $(CFLAGS) main_parser.c -o parser

interpreter: $(OBJS)
	$(CC)  $(CFLAGS) $^ -o interpreter

%.o : %.c $(HDRS)
	$(CC)  $(CFLAGS) -c $<  -o $@

clean:
	rm -f *.o
	rm -f linkedlist
	rm -f tokenizer
	rm -f parser
	rm -f interpreter
