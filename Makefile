CFLAGS=-std=c11 -g -static
SRCS=$(wildcard ./src/*.c)
OBJS=$(SRCS:.c=.o)

mcc2: $(OBJS)
	$(CC) -o mcc2 $(OBJS) $(LDFLAGS)

$(OBJS):

test2: 
	cc -S test/test.c
	cc -o tmp -no-pie test.s -lc

test: mcc2
	./mcc2 -c ./test/test.c > tmp.s
	cc -o tmp2 -no-pie tmp.s -lc
	
clean:
	rm -f mcc2 src/*.o *~ tmp*

.PHONY: test clean
