CFLAGS=-std=c11 -g -static
SRCS=$(wildcard ./src/*.c)
OBJS=$(SRCS:.c=.o)

mcc2: $(OBJS)
	$(CC) -o mcc2 $(OBJS) $(LDFLAGS)

$(OBJS):

test2: 
	gcc -S test/test.c
	cc -o tmp -no-pie test.s -lc

test: mcc2
	./mcc2 -c ./test/test.c > tmp.s
	gcc -o tmp -no-pie tmp.s -lc
	getconf GNU_LIBC_VERSION
	
clean:
	rm -f mcc2 src/*.o *~ tmp*

.PHONY: test clean
