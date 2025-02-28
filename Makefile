CFLAGS=-std=c11 -g -static
SRCS=$(wildcard ./src/*.c)
OBJS=$(SRCS:.c=.o)
SELF_OBJS=$(patsubst ./src/%.c, ./selfhost/%.o, $(SRCS))
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
	./mcc2 -c $< -o $@.s -i ./test/testinc -i ./src -d PREDEFINED_MACRO -x plvar -g
	cc -c -o $@ $@.s -static

test : mcc2 $(TEST_OBJS)
	cc -o test.exe $(TEST_OBJS)
	./test.exe

test2: mcc2
	./mcc2 -c ./dev/test2.c -o ./tmp.s -i ./test/testinc -i ./src -x plvar -g
	cc -o ./dev/tmp -no-pie tmp.s -lc
	./dev/tmp

tmp: mcc2
	cc -o tmp -no-pie tmp.s -lc
	./tmp

./selfhost/mcc2t: mcc2
	./mcc2 -c ./src/builtin_def.c -d PREDEFINED_MACRO -o ./selfhost/builtin_def.s -i ./src -x plvar -g
	cc -c -o ./selfhost/builtin_def.o -no-pie ./selfhost/builtin_def.s -lc -MD

	./mcc2 -c ./src/error.c -d PREDEFINED_MACRO -o ./selfhost/error.s -i ./src -x plvar -g
	cc -c -o ./selfhost/error.o -no-pie ./selfhost/error.s -lc -MD

	./mcc2 -c ./src/file.c -d PREDEFINED_MACRO -o ./selfhost/file.s -i ./src -x plvar -g
	cc -c -o ./selfhost/file.o -no-pie ./selfhost/file.s -lc -MD

	./mcc2 -c ./src/gen_ir.c -d PREDEFINED_MACRO -o ./selfhost/get_ir.s -i ./src -x plvar -g
	cc -c -o ./selfhost/gen_ir.o -no-pie ./selfhost/get_ir.s -lc -MD

	./mcc2 -c ./src/semantics.c -d PREDEFINED_MACRO -o ./selfhost/semantics.s -i ./src -x plvar -g
	cc -c -o ./selfhost/semantics.o -no-pie ./selfhost/semantics.s -lc -MD

	cp ./src/gen_x86_64.o ./selfhost/gen_x86_64.o
	cp ./src/main.o ./selfhost/main.o
	cp ./src/parse.o ./selfhost/parse.o
	cp ./src/tokenizer.o ./selfhost/tokenizer.o
	cp ./src/type.o ./selfhost/type.o
	cp ./src/ident.o ./selfhost/ident.o
	cp ./src/utility.o ./selfhost/utility.o
	cp ./src/preprocess.o ./selfhost/preprocess.o
	cp ./src/pre_macro_map.o ./selfhost/pre_macro_map.o

	#cp ./src/file.o ./selfhost/file.o


	cc -o ./selfhost/mcc2t $(SELF_OBJS) $(LDFLAGS)

self: ./selfhost/mcc2t

./selfhost/test/c/%.o: test/c/%.c
	./selfhost/mcc2t -c $< -o $@.s -i ./test/testinc -i ./src -d PREDEFINED_MACRO -x plvar -g
	cc -c -o $@ $@.s -static

selft: self $(TEST_SELF_OBJS)
	cc -o test.exe $(TEST_SELF_OBJS)
	./test.exe

clean:
	rm -f mcc2 *~ tmp*
	rm -f src/*.o src/*.d 
	rm -f test/c/*.o test.exe test/c/*.s
	rm -f ./selfhost/*.o ./selfhost/*.s ./selfhost/mcc2t
	rm -f ./selfhost/test/c/*.o ./selfhost/test/c/*.s

.PHONY: test clean tmp test2 test3 test4 self selft
