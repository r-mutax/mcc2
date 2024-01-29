#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./mcc2 "$input" > tmp.s
  cc -o tmp -no-pie tmp.s
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
assert 0 "int main(){return 0;}"
assert 42 "int main(){return 42;}"

# test + - * / %
assert 8 "int main(){return 3+5;}"
assert 3 "int main(){return 8-5;}"
assert 13 "int main(){return 1 + 3 * 4;}"
assert 5 "int main(){return 3 + 4 / 2;}"
assert 5 "int main(){return (9 + 6) / 3;}"
assert 2 "int main(){ return 5 % 3; }"
assert 7 "int main(){ return 56 >> 3; }"
assert 56 "int main(){ return 7 << 3; }"
assert 7 "int main(){ int a; a = 56; a >>= 3; return a; }"
assert 56 "int main(){ int a; a = 7; a <<= 3; return a; }"

# test unary
assert 10 "int main(){return - -10;}"
assert 10 "int main(){return - -+10;}"
assert 10 "int main(){return -10+20;}"
assert 5 "int main(){int a; int b; a = 5; b = &a; return *b;}"
assert 10 "int main(){int a; int* b; a = 5; b = &a; *b = 10; return a;}"
assert 8 "int main(){int a; return sizeof a;}"
assert 8 "int main(){int *a; return sizeof(a);}"

# test equality
assert 1 "int main(){return 2 == 2;}"
assert 0 "int main(){return 2 == 3;}"
assert 1 "int main(){return 3 + 4 != 8;}"
assert 0 "int main(){return 3 + 4 != 7;}"

# test relational
assert 1 "int main(){return 3 < 4;}"
assert 0 "int main(){return 3 < 2;}"
assert 1 "int main(){return 3 <= 3;}"
assert 1 "int main(){return 3 <= 4;}"
assert 0 "int main(){return 3 <= 2;}"
assert 1 "int main(){return 4 > 3;}"
assert 1 "int main(){return 4 >= 4;}"

# test expr
assert 1 "int main(){ return 5 & 3; }"
assert 6 "int main(){ return 5 ^ 3; }"
assert 7 "int main(){ return 3 | 4; }"
assert 15 "int main(){ return 8 | 3 ^ 5 & 4;}"

# test logic
assert 1 "int main(){ return 3 && 5; }"
assert 0 "int main(){ return 3 && 0; }"
assert 0 "int main(){ return 0 && 5; }"
assert 0 "int main(){ return 0 && 0; }"
assert 5 "int main(){ int a; a = 5; 0 && (a = 4); return a; }"

assert 1 "int main(){ return 3 || 5; }"
assert 1 "int main(){ return 3 || 0; }"
assert 1 "int main(){ return 0 || 5; }"
assert 0 "int main(){ return 0 || 0; }"
assert 5 "int main(){ int a; a = 5; 1 || (a = 4); return a; }"

assert 6 "int main(){ return 1 ? 6 : 3; }"
assert 3 "int main(){ return 0 ? 6 : 3; }"

# test statement
assert 3 "int main(){return 3; 5;}"

# test lvar
assert 3 "int main(){int a; return a=3; a;}"
assert 5 "int main(){int a; int b; return a=5; b = 3; a;}"
assert 3 "int main(){int abc; return abc=3; abc;}"
assert 4 "int main(){int a; a = 1; a = a + 3; return a;}"
assert 5 "int main(){int a; int b; int c; a = 4; b = c = 5; return b;}"

assert 5 "int main(){int a; a = 2; a += 3; return a; }"
assert 3 "int main(){int a; a = 5; a -= 2; return a; }"
assert 6 "int main(){int a; a = 2; a *= 3; return a; }"
assert 2 "int main(){int a; a = 8; a /= 4; return a; }"
assert 2 "int main(){int a; a = 5; a %= 3; return a; }"

# test if
assert 5 "int main(){int a;a=0; if(1) a = 5; return a;}"
assert 0 "int main(){int a;a=0; if(0) a = 5; return a;}"
assert 3 "int main(){int a;a=0; if(1) a = 3; else a = 2; return a;}"
assert 3 "int main(){int a;a=0; if(0) a = 1; else a = 3; return a;}"
assert 3 "int main(){int a;a = 0; if(0) a = 1; else if(0) a = 2; else a = 3; return a;}"

# test while
assert 3 "int main(){int a;a = 0; while(a < 3) a = a + 1; return a;}"

# test for
assert 10 "int main(){int a; int b; b = 0; for(a = 0; a < 5; a = a + 1) b = b + 2; return b;}"

# test block
assert 5 "int main(){int a; int c; int i; a = 0; c = 0; for(i = 0; i < 5; i = i + 1) { a = a + 1; c = c + 1; } return a;}"

# test function call
assert 3 "int foo(){ return 3; } int main(){ return foo(); }"
assert 3 "int bar(int a, int b) { a = 3; return a;} int main(){ return bar(); }"
assert 4 "int bar(int a, int b) {return b;} int main(){ return bar(3, 4); }"

# test array definition
assert 88 "int main(){int a[11]; return sizeof(a);}"
assert 5 "int main(){int a[5]; *a =5; return *a;}"
assert 5 "int main(){int a[5]; *(a + 2) =5; return *(a + 2);}"
assert 4 "int main(){int a[5]; a[3] = 4; return a[3];}"

# test global variable
assert 5 "int g_a; int main(){ g_a = 5; return g_a;}"
assert 5 "int g_a; int foo() { g_a = 5; }int main(){ foo(); return g_a;}"

echo OK

