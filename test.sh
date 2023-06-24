#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./mcc2 "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

# test num
assert 0 "return 0;"
assert 42 "return 42;"

# test + - * /
assert 8 "return 3+5;"
assert 3 "return 8-5;"
assert 13 "return 1 + 3 * 4;"
assert 5 "return 3 + 4 / 2;"
assert 5 "return (9 + 6) / 3;"

# test equality
assert 1 "return 2 == 2;"
assert 0 "return 2 == 3;"
assert 1 "return 3 + 4 != 8;"
assert 0 "return 3 + 4 != 7;"

# test relational
assert 1 "return 3 < 4;"
assert 0 "return 3 < 2;"
assert 1 "return 3 <= 3;"
assert 1 "return 3 <= 4;"
assert 0 "return 3 <= 2;"
assert 1 "return 4 > 3;"
assert 1 "return 4 >= 4;"

# test statement
assert 3 "return 3; 5;"

# test lvar
assert 3 "return a=3; a;"
assert 5 "return a=5; b = 3; a;"
assert 3 "return abc=3; abc;"

# test it
assert 5 "a=0; if(1) a = 5; return a;"
assert 0 "a=0; if(0) a = 5; return a;"

echo OK

