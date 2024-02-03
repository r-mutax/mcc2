CFLAGS=-std=c11 -g -static
SRCS=$(wildcard ./src/*.c)
OBJS=$(SRCS:.c=.o)

mcc2: $(OBJS)
	$(CC) -o mcc2 $(OBJS) $(LDFLAGS)

$(OBJS):

test: mcc2
	./mcc2 -c ./test/test.c > tmp.s
	cc -o tmp -no-pie tmp.s -lc
	cat ./tmp.s
	
clean:
	rm -f mcc2 src/*.o *~ tmp*

.PHONY: test clean
