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
assert 0 "0;"
assert 42 "42;"

# test + - * /
assert 8 "3+5;"
assert 3 "8-5;"
assert 13 "1 + 3 * 4;"
assert 5 "3 + 4 / 2;"
assert 5 "(9 + 6) / 3;"

# test equality
assert 1 "2 == 2;"
assert 0 "2 == 3;"
assert 1 "3 + 4 != 8;"
assert 0 "3 + 4 != 7;"

# test relational
assert 1 "3 < 4;"
assert 0 "3 < 2;"
assert 1 "3 <= 3;"
assert 1 "3 <= 4;"
assert 0 "3 <= 2;"
assert 1 "4 > 3;"
assert 1 "4 >= 4;"

# test statement
assert 5 "3; 5;"

# test lvar
assert 3 "a=3; a;"
assert 5 "a=5; b = 3; a;"

echo OK

