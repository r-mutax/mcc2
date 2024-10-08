CFLAGS=-std=c11 -g -static
SRCS=$(wildcard ./src/*.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)

TESTS=$(wildcard ./test/c/*.c)
TEST_OBJS=$(TESTS:.c=.o)

mcc2: $(OBJS)
	$(CC) -o mcc2 $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -MD -o $@ $<

-include $(DEPS)

test/c/%.o: test/c/%.c
	./mcc2 -c $< -o $@.s -i ./test/testinc -d PREDEFINED_MACRO
	cc -c -o $@ $@.s -static

test : mcc2 $(TEST_OBJS)
	cc -o test.exe $(TEST_OBJS)
	./test.exe

test2: mcc2
	./mcc2 -c ./dev/test2.c -o ./tmp.s -i ./test/testinc
	cc -o ./dev/tmp -no-pie tmp.s -lc
	./dev/tmp

test4: mcc2
	./mcc2 -c ./test/test2.c -o ./tmp.s -d PREDEFINED_MACRO -E

tmp: mcc2
	cc -o tmp -no-pie tmp.s -lc
	./tmp

clean:
	rm -f mcc2 src/*.o *~ tmp* src/*.d test/c/*.o test.exe test/c/*.s

.PHONY: test clean tmp test2 test3 test4
