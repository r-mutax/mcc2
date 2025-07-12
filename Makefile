###########################################
# Makefile for mcc2 compiler
###########################################
-include $(DEPS)

#############################################
# Compiler and flags for the mcc2
#############################################
# CFLAGS for gcc
CFLAGS=-std=c11 -g -static -I./lib

# Library flags for gcc
LDFLAGS=-L./lib/bin -lmcc2

# file paths
SRCS=$(wildcard ./src/*.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)
LIBS=$(wildcard ./lib/*.c)
LIBSOBJS=$(LIBS:.c=.o)

# Test objects
TESTS=$(wildcard ./test/c/*.c)
TEST_OBJS=$(TESTS:.c=.o)

mcc2: $(OBJS) ./lib/bin/libmcc2.a
	$(CC) -o mcc2 $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -MD -o $@ $<

./lib/bin/libmcc2.a: $(LIBSOBJS)
	echo ${LIBSOBJS}
	mkdir -p ./lib/bin
	ar rcs ./lib/bin/libmcc2.a $(LIBSOBJS)

src/lib/%.o: src/lib/%.c
	$(CC) -c $(CFLAGS) -MD -o $@ $<

# Rules for test objects
test/c/%.o: test/c/%.c
	./mcc2 -c $< -o $@.s -I ./test/testinc -I ./src -I ./lib -d PREDEFINED_MACRO -x plvar -g
	cc -c -o $@ $@.s -static

test : mcc2 $(TEST_OBJS)
	cc -o test.exe $(TEST_OBJS) -L./lib/bin -lmcc2
	./test.exe


#############################################
# Compiler and flags for the mcc2t
#############################################
# CFLAGS for mcc2
CFLAGS_SELF=-g -I./lib -I./src -I./lib -x plvar

# file paths
SELF_OBJS=$(patsubst ./src/%.c, ./selfhost/%.o, $(SRCS))

# Test objects
TEST_SELF_OBJS := $(patsubst ./test/c/%.c, ./selfhost/test/c/%.o, $(TESTS))

./selfhost/%.o: ./src/%.c ./mcc2
	./mcc2 -c $< -d PREDEFINED_MACRO -o $@.s $(CFLAGS_SELF)
	cc -c -o $@ -no-pie $@.s -lc -MD -g

self: $(SELF_OBJS) ./lib/bin/libmcc2.a
	cc -o ./selfhost/mcc2t $(SELF_OBJS) $(LDFLAGS)

./selfhost/test/c/%.o: test/c/%.c
	./selfhost/mcc2t -c $< -o $@.s -I ./test/testinc -I ./src -I ./lib -d PREDEFINED_MACRO -x plvar -g
	cc -c -o $@ $@.s -static

selft: self $(TEST_SELF_OBJS)
	cc -o test.exe $(TEST_SELF_OBJS) -L./lib/bin -lmcc2
	./test.exe

#############################################
# Utilities and tools
#############################################
dwarf : mcc2
	cc ./dev/dwarf_test.c -o ./dev/cc.s -S -g
	cc -c -o ./dev/cc.o ./dev/cc.s -lc -MD
	readelf -w ./dev/cc.o > ./dev/cc.dwarf

	./mcc2 -c ./dev/dwarf_test.c -o ./dev/mcc2.s -I ./test/testinc -I ./src -x plvar -g
	cc -c -o ./dev/mcc2.o ./dev/mcc2.s -lc -MD
	readelf -w ./dev/mcc2.o > ./dev/mcc2.dwarf

	cc -o dwarf_test ./dev/mcc2.o -lc

test2: mcc2
	./mcc2 -c ./dev/test2.c -o ./tmp.s -I ./test/testinc -I ./src -I ./lib -x plvar -g
	cc -o ./dev/tmp -no-pie tmp.s -lc
	./dev/tmp

tmp: mcc2
	cc -o tmp -no-pie tmp.s -lc
	./tmp

clean:
	rm -f mcc2 *~ tmp* libmcc2.a
	rm -f src/*.o src/*.d
	rm -f lib/*.o lib/*.d
	rm -f test/c/*.o test.exe test/c/*.s
	rm -f ./selfhost/*.o ./selfhost/*.s ./selfhost/mcc2t
	rm -f ./selfhost/test/c/*.o ./selfhost/test/c/*.s

.PHONY: test clean tmp test2 test3 test4 self selft dwarf
