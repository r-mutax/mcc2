CFLAGS=-std=c11 -g -static
SRCS=$(wildcard ./src/*.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)

TESTS=$(wildcard ./test/c/*.c)
TEST_OBJS=$(TESTS:.c=.o)
TEST_SELF_OBJS := $(patsubst ./test/c/%.c, ./selfhost/test/c/%.o, $(TESTS))

mcc2: $(OBJS)
	$(CC) -o mcc2 $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -MD -o $@ $<

-include $(DEPS)

test/c/%.o: test/c/%.c
	./mcc2 -c $< -o $@.s -i ./test/testinc -i ./src -d PREDEFINED_MACRO -x plvar
	cc -c -o $@ $@.s -static

test : mcc2 $(TEST_OBJS)
	cc -o test.exe $(TEST_OBJS)
	./test.exe

test2: mcc2
	./mcc2 -c ./dev/test2.c -o ./tmp.s -i ./test/testinc -i ./src -x plvar
	cc -o ./dev/tmp -no-pie tmp.s -lc
	./dev/tmp

tmp: mcc2
	cc -o tmp -no-pie tmp.s -lc
	./tmp

./selfhost/mcc2t: mcc2
	./mcc2 -c ./src/builtin_def.c -d PREDEFINED_MACRO -o ./selfhost/builtin_def.s -i ./src -x plvar
	cc -c -o ./selfhost/builtin_def.o -no-pie ./selfhost/builtin_def.s -lc -MD

	./mcc2 -c ./src/error.c -d PREDEFINED_MACRO -o ./selfhost/error.s -i ./src -x plvar
	cc -c -o ./selfhost/error.o -no-pie ./selfhost/error.s -lc -MD

	./mcc2 -c ./src/file.c -d PREDEFINED_MACRO -o ./selfhost/file.s -i ./src -x plvar
	cc -c -o ./selfhost/file.o -no-pie ./selfhost/file.s -lc -MD

	./mcc2 -c ./src/semantics.c -d PREDEFINED_MACRO -o ./selfhost/semantics.s -i ./src -x plvar
	cc -c -o ./selfhost/semantics.o -no-pie ./selfhost/semantics.s -lc -MD

	cc -o ./selfhost/mcc2t $(OBJS) $(LDFLAGS)

self: ./selfhost/mcc2t

./selfhost/test/c/%.o: test/c/%.c
	./selfhost/mcc2t -c $< -o $@.s -i ./test/testinc -i ./src -d PREDEFINED_MACRO -x plvar
	cc -c -o $@ $@.s -static

selft: self $(TEST_SELF_OBJS)
	cc -o test.exe $(TEST_SELF_OBJS)
	./test.exe

clean:
	rm -f mcc2 src/*.o *~ tmp* src/*.d test/c/*.o test.exe test/c/*.s ./selfhost/*.o ./selfhost/*.s ./selfhost/mcc2

.PHONY: test clean tmp test2 test3 test4 self selft
