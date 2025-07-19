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

#############################################
# Self-hosted compiler
#############################################
# Define a macro to create the self-hosted compiler directory structure
define COMPILE_SELF
$(eval $(1)_BASE_DIR := self/$(2))
$(eval $(1)_LIB_DIR := self/$(2)/lib)
$(eval $(1)_MCC := $(3))
$(eval $(1)_TARGET_COMPILER := self/$(2)/mcc2)
$(eval $(1)_SELF_OBJS := $(patsubst ./src/%.c, $(value $(1)_BASE_DIR)/%.o, $(SRCS)))
$(eval $(1)_SELF_LIB_OBJS := $(patsubst ./lib/%.c, $(value $(1)_BASE_DIR)/lib/%.o, $(LIBS)))
$(eval $(1)_LDFLAGS_SELF := -L$(value $(1)_BASE_DIR)/lib/bin -lmcc2)

$(eval $(1)_SELF_TEST_OBJS := $(patsubst ./test/c/%.c, $(value $(1)_BASE_DIR)/test/c/%.o, $(TESTS)))

# Create the base directory and subdirectories
$(value $(1)_BASE_DIR):
	mkdir -p $(value $(1)_BASE_DIR)
	mkdir -p $(value $(1)_BASE_DIR)/lib/bin
	mkdir -p $(value $(1)_BASE_DIR)/test/c

# Compile library objects for self-hosted compiler
$(value $(1)_LIB_DIR)/%.o: ./lib/%.c $(value $(1)_MCC)
	$(value $(1)_MCC) -c $$< -d PREDEFINED_MACRO -o $$@.s $(CFLAGS_SELF)
	cc -c -o $$@ -no-pie $$@.s -lc -MD -g

# Make library objects
$(value $(1)_BASE_DIR)/lib/bin/libmcc2.a: $(value $(1)_SELF_LIB_OBJS)
	echo make $(value $(1)_BASE_DIR)/lib/bin/libmcc2.a
	mkdir -p $(value $(1)_BASE_DIR)/lib/bin
	ar rcs $(value $(1)_BASE_DIR)/lib/bin/libmcc2.a $(value $(1)_SELF_LIB_OBJS)

# Compile self-hosted compiler objects
$(value $(1)_BASE_DIR)/%.o: ./src/%.c $(value $(1)_MCC)
	$(value $(1)_MCC) -c $$< -d PREDEFINED_MACRO -o $$@.s $(CFLAGS_SELF)
	cc -c -o $$@ -no-pie $$@.s -lc -MD -g

# Make self-hosted compiler
$(value $(1)_TARGET_COMPILER): $(value $(1)_BASE_DIR) $(value $(1)_SELF_OBJS) $(value $(1)_BASE_DIR)/lib/bin/libmcc2.a
	@echo "Compiling self-hosted compiler: $(value $(1)_TARGET_COMPILER)"
	cc -o $(value $(1)_TARGET_COMPILER) $(value $(1)_SELF_OBJS) $(value $(1)_BASE_DIR)/lib/bin/libmcc2.a $(value $(1)_LDFLAGS_SELF)

# Target rule for the self-hosted compiler
$(1): $(value $(1)_BASE_DIR) $(value $(1)_TARGET_COMPILER)

# Compile test objects for self-hosted compiler
$(value $(1)_BASE_DIR)/test/c/%.o: test/c/%.c
	$(value $(1)_TARGET_COMPILER) -c $$< -o $$@.s -I ./test/testinc -I ./src -I ./lib -d PREDEFINED_MACRO -x plvar -g
	cc -c -o $$@ $$@.s -static

# Compile test executable for self-hosted compiler
$(1)t: $(1) $(value $(1)_SELF_TEST_OBJS)
	cc -o $(value $(1)_BASE_DIR)/test/test.exe $(value $(1)_SELF_TEST_OBJS) $(value $(1)_LDFLAGS_SELF)
	$(value $(1)_BASE_DIR)/test/test.exe

endef

# arg1 : task name
# arg2 : base directory name
# arg3 : use compiler path
$(eval $(call COMPILE_SELF,self1,selfhost1,./mcc2))
$(eval $(call COMPILE_SELF,self2,selfhost2,./self/selfhost1/mcc2))
$(eval $(call COMPILE_SELF,self3,selfhost3,./self/selfhost2/mcc2))


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
	rm -rf ./self

.PHONY: test clean tmp test2 self selft dwarf self1
