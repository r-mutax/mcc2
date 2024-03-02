CFLAGS=-std=c11 -g -static
SRCS=$(wildcard ./src/*.c)
OBJS=$(SRCS:.c=.o)

mcc2: $(OBJS)
	$(CC) -o mcc2 $(OBJS) $(LDFLAGS)

$(OBJS):

test: mcc2
	./mcc2 -c ./test/test.c -o ./tmp.s
	cc -o tmp -no-pie tmp.s -lc
	./tmp

test2: mcc2
	./mcc2 -c ./test/test2.c -o ./tmp.s
	cc -o tmp -no-pie tmp.s -lc
	./tmp

clean:
	rm -f mcc2 src/*.o *~ tmp*

.PHONY: test clean
