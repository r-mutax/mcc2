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
assert 0 "main(){return 0;}"
assert 42 "main(){return 42;}"

# test + - * /
assert 8 "main(){return 3+5;}"
assert 3 "main(){return 8-5;}"
assert 13 "main(){return 1 + 3 * 4;}"
assert 5 "main(){return 3 + 4 / 2;}"
assert 5 "main(){return (9 + 6) / 3;}"

# test equality
assert 1 "main(){return 2 == 2;}"
assert 0 "main(){return 2 == 3;}"
assert 1 "main(){return 3 + 4 != 8;}"
assert 0 "main(){return 3 + 4 != 7;}"

# test relational
assert 1 "main(){return 3 < 4;}"
assert 0 "main(){return 3 < 2;}"
assert 1 "main(){return 3 <= 3;}"
assert 1 "main(){return 3 <= 4;}"
assert 0 "main(){return 3 <= 2;}"
assert 1 "main(){return 4 > 3;}"
assert 1 "main(){return 4 >= 4;}"

# test expr
assert 1 "main(){ return 5 & 3; }"
assert 6 "main(){ return 5 ^ 3; }"
assert 7 "main(){ return 3 | 4; }"
assert 15 "main(){ return 8 | 3 ^ 5 & 4;}"

# test logic
assert 1 "main(){ return 3 && 5; }"
assert 0 "main(){ return 3 && 0; }"
assert 0 "main(){ return 0 && 5; }"
assert 0 "main(){ return 0 && 0; }"

# test statement
assert 3 "main(){return 3; 5;}"

# test lvar
assert 3 "main(){return a=3; a;}"
assert 5 "main(){return a=5; b = 3; a;}"
assert 3 "main(){return abc=3; abc;}"
assert 4 "main(){a = 1; a = a + 3; return a;}"
assert 5 "main(){ a = 4; b = c = 5; return b;}"

# test if
assert 5 "main(){a=0; if(1) a = 5; return a;}"
assert 0 "main(){a=0; if(0) a = 5; return a;}"
assert 3 "main(){a=0; if(1) a = 3; else a = 2; return a;}"
assert 3 "main(){a=0; if(0) a = 1; else a = 3; return a;}"
assert 3 "main(){a = 0; if(0) a = 1; else if(0) a = 2; else a = 3; return a;}"

# test while
assert 3 "main(){a = 0; while(a < 3) a = a + 1; return a;}"

# test for
assert 10 "main(){b = 0; for(a = 0; a < 5; a = a + 1) b = b + 2; return b;}"

# test block
assert 5 "main(){a = 0; c = 0; for(i = 0; i < 5; i = i + 1) { a = a + 1; c = c + 1; } return a;}"

# test function call
assert 3 "foo(){ return 3; } main(){ return foo(); }"

echo OK

