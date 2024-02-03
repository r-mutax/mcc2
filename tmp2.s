.intel_syntax noprefix
.global main
  .data
.LSTR26:
  .string "test of assignment..\n"
  .data
.LSTR25:
  .string "test of function call..
  .data
.LSTR24:
  .string "abd"
  .data
.LSTR23:
  .string "abc"
  .data
.LSTR22:
  .string "abc"
  .data
.LSTR21:
  .string "abc"
  .data
.LSTR20:
  .string "abc"
  .data
.LSTR19:
  .string "test of string literal.
  .data
.LSTR18:
  .string "test of int type..\n"
  .data
.LSTR17:
  .string "test of short type..\n"
  .data
.LSTR16:
  .string "test of char type..\n"
  .data
.LSTR15:
  .string "test of assignment..\n"
  .data
.LSTR14:
  .string "test of array..\n"
  .data
.LSTR13:
  .string "test of global variable
  .data
.LSTR12:
  .string "test of local variable.
  .data
.LSTR11:
  .string "test of compound-statem
  .data
.LSTR10:
  .string "test of return-statemen
  .data
.LSTR9:
  .string "test of if-statement...
  .data
.LSTR8:
  .string "test of for-statement..
  .data
.LSTR7:
  .string "test of while-statement
  .data
.LSTR6:
  .string "test of unary...\n"
  .data
.LSTR5:
  .string "test of ternary operato
  .data
.LSTR4:
  .string "test of logical express
  .data
.LSTR3:
  .string "test of expression rela
  .data
.LSTR2:
  .string "test of expression..\n"
  .data
.LSTR1:
  .string "test is complete!!!\n"
  .data
.LSTR0:
  .string "mcc2 test...\n\n"
  .bss
g_b:
  .zero 8
  .bss
g_a:
  .zero 8
  .text
main:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  mov rax, OFFSET FLAT:.LSTR0
  push rax
  pop rdi
  mov eax, 0
  sub rsp, 8
  call printf
  add rsp, 8
  push rax
  pop rax
  sub rsp, 8
  call test_expression
  add rsp, 8
  push rax
  pop rax
  sub rsp, 8
  call test_statement
  add rsp, 8
  push rax
  pop rax
  sub rsp, 8
  call test_variable
  add rsp, 8
  push rax
  pop rax
  sub rsp, 8
  call test_type
  add rsp, 8
  push rax
  pop rax
  sub rsp, 8
  call test_primary
  add rsp, 8
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR1
  push rax
  pop rdi
  mov eax, 0
  sub rsp, 8
  call printf
  add rsp, 8
  push rax
  pop rax
  mov rax, 0
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
  .text
test_assignment:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rax, OFFSET FLAT:.LSTR26
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  lea rax, [rbp - 8]
  push rax
  mov rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 3
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 8]
  push rax
  mov rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 0
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 8]
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 6
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 8]
  push rax
  mov rax, 8
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  cqo
  idiv rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 2
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 8]
  push rax
  mov rax, 13
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 5
  push rax
  pop rdi
  pop rax
  cqo
  idiv rdi
  mov rax, rdx
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 3
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 8]
  push rax
  mov rax, 56
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  mov rcx, rdi
  sar rax, cl
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 7
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 8]
  push rax
  mov rax, 7
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  mov rcx, rdi
  sal rax, cl
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 56
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  .text
test_pointer:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  lea rax, [rbp - 8]
  push rax
  mov rax, 10
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 16]
  push rax
  lea rax, [rbp - 8]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 16]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 10
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 16]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, [rbp - 8]
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 16]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 15
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 15
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 8
  push rax
  mov rax, 8
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  .text
test_primary:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rax, OFFSET FLAT:.LSTR19
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  lea rax, [rbp - 1]
  push rax
  mov rax, OFFSET FLAT:.LSTR20
  push rax
  mov rax, 0
  push rax
  mov rax, 1
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  pop rax
  lea rax, [rbp - 1]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  mov rax, 97
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 1]
  push rax
  mov rax, OFFSET FLAT:.LSTR21
  push rax
  mov rax, 1
  push rax
  mov rax, 1
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  pop rax
  lea rax, [rbp - 1]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  mov rax, 98
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 1]
  push rax
  mov rax, OFFSET FLAT:.LSTR22
  push rax
  mov rax, 2
  push rax
  mov rax, 1
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  pop rax
  lea rax, [rbp - 1]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  mov rax, 99
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 1]
  push rax
  mov rax, OFFSET FLAT:.LSTR23
  push rax
  mov rax, 3
  push rax
  mov rax, 1
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  pop rax
  lea rax, [rbp - 1]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  mov rax, 0
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 4
  push rax
  mov rax, 4
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR25
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 4
  push rax
  pop rsi
  pop rdi
  call add
  push rax
  mov rax, 7
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 5
  push rax
  mov rax, 6
  push rax
  pop rsi
  pop rdi
  call add
  push rax
  mov rax, 11
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  .text
test_type:
  push rbp
  mov rbp, rsp
  sub rsp, 80
  mov rax, OFFSET FLAT:.LSTR16
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  lea rax, [rbp - 1]
  push rax
  mov rax, 5
  push rax
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  pop rax
  lea rax, [rbp - 1]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  mov rax, 5
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 1
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 3
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 8
  push rax
  mov rax, 8
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR17
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  lea rax, [rbp - 14]
  push rax
  mov rax, 7
  push rax
  pop rdi
  pop rax
  mov [rax], di
  push rdi
  pop rax
  lea rax, [rbp - 14]
  push rax
  pop rax
  movsx rax, WORD PTR [rax]
  push rax
  mov rax, 7
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 2
  push rax
  mov rax, 2
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 6
  push rax
  mov rax, 6
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 8
  push rax
  mov rax, 8
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR18
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  lea rax, [rbp - 36]
  push rax
  mov rax, 17
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 36]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 17
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 8
  push rax
  mov rax, 8
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 24
  push rax
  mov rax, 24
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 8
  push rax
  mov rax, 8
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  .text
test_variable:
  push rbp
  mov rbp, rsp
  sub rsp, 112
  mov rax, OFFSET FLAT:.LSTR12
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  lea rax, [rbp - 8]
  push rax
  mov rax, 15
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 15
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 16]
  push rax
  mov rax, 7
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 16]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 7
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 8]
  push rax
  mov rax, 1
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 4
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR13
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  mov rax, OFFSET FLAT:g_a
  push rax
  mov rax, 8
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, OFFSET FLAT:g_a
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 8
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  call test_global_variable
  push rax
  pop rax
  mov rax, OFFSET FLAT:g_b
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 14
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR14
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  mov rax, 88
  push rax
  mov rax, 88
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 104]
  push rax
  mov rax, 5
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 104]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 5
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 104]
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, 7
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 104]
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 7
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 104]
  push rax
  mov rax, 3
  push rax
  mov rax, 8
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 104]
  push rax
  mov rax, 3
  push rax
  mov rax, 8
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 4
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR15
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  .text
test_statement:
  push rbp
  mov rbp, rsp
  sub rsp, 64
  mov rax, OFFSET FLAT:.LSTR7
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  lea rax, [rbp - 8]
  push rax
  mov rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.L0:
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .L1
  lea rax, [rbp - 8]
  push rax
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 1
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .L0
  pop rax
  cmp rax, 0
  je .L0
.L1:
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 3
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR8
  push rax
  pop rdi
  mov eax, 0
  sub rsp, 8
  call printf
  add rsp, 8
  push rax
  pop rax
  lea rax, [rbp - 16]
  push rax
  mov rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 24]
  push rax
  mov rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 16]
  push rax
  mov rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.L2:
  lea rax, [rbp - 16]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 5
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .L3
  lea rax, [rbp - 24]
  push rax
  lea rax, [rbp - 24]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 16]
  push rax
  lea rax, [rbp - 16]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 1
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .L2
  pop rax
  cmp rax, 0
  je .L2
.L3:
  lea rax, [rbp - 16]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 5
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  lea rax, [rbp - 24]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 10
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR9
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  lea rax, [rbp - 32]
  push rax
  mov rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, 1
  push rax
  pop rax
  cmp rax, 0
  je .L4
  lea rax, [rbp - 32]
  push rax
  mov rax, 1
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.L4:
  lea rax, [rbp - 32]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  je .L5
  lea rax, [rbp - 32]
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.L5:
  lea rax, [rbp - 32]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 1
  push rax
  pop rax
  cmp rax, 0
  je .L6
  lea rax, [rbp - 32]
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .L7
  pop rax
  cmp rax, 0
  je .L7
.L6:
  lea rax, [rbp - 32]
  push rax
  mov rax, 20
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.L7:
  lea rax, [rbp - 32]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 2
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  je .L8
  lea rax, [rbp - 32]
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .L9
  pop rax
  cmp rax, 0
  je .L9
.L8:
  lea rax, [rbp - 32]
  push rax
  mov rax, 20
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.L9:
  lea rax, [rbp - 32]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 20
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  je .L10
  lea rax, [rbp - 32]
  push rax
  mov rax, 1
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .L11
  pop rax
  cmp rax, 0
  je .L11
.L10:
  mov rax, 1
  push rax
  pop rax
  cmp rax, 0
  je .L12
  lea rax, [rbp - 32]
  push rax
  mov rax, 12
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  jmp .L13
  pop rax
  cmp rax, 0
  je .L13
.L12:
  lea rax, [rbp - 32]
  push rax
  mov rax, 123
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
.L13:
.L11:
  lea rax, [rbp - 32]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 12
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR10
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  call test_return
  push rax
  mov rax, 5
  push rax
  pop rsi
  pop rdi
  call assert
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR11
  push rax
  pop rdi
  mov eax, 0
  call printf
  push rax
  pop rax
  lea rax, [rbp - 40]
  push rax
  mov rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 48]
  push rax
  mov rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 56]
  push rax
  mov rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 56]
  push rax
  mov rax, 0
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.L14:
  lea rax, [rbp - 56]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 5
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .L15
  lea rax, [rbp - 40]
  push rax
  lea rax, [rbp - 40]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 1
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 48]
  push rax
  lea rax, [rbp - 48]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  lea rax, [rbp - 56]
  push rax
  lea rax, [rbp - 56]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 1
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .L14
  pop rax
  cmp rax, 0
  je .L14
.L15:
  lea rax, [rbp - 40]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 5
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  lea rax, [rbp - 48]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, 10
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  .text
test_expression:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  mov rax, OFFSET FLAT:.LSTR2
  push rax
  pop rdi
  mov eax, 0
  sub rsp, 8
  call printf
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 5
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, 8
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 8
  push rax
  mov rax, 5
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, 3
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 1
  push rax
  mov rax, 3
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, 13
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 4
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  cqo
  idiv rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, 5
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 9
  push rax
  mov rax, 6
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  cqo
  idiv rdi
  push rax
  mov rax, 5
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 5
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  cqo
  idiv rdi
  mov rax, rdx
  push rax
  mov rax, 2
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 56
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  mov rcx, rdi
  sar rax, cl
  push rax
  mov rax, 7
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 7
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  mov rcx, rdi
  sal rax, cl
  push rax
  mov rax, 56
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 5
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  and rax, rdi
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 5
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  xor rax, rdi
  push rax
  mov rax, 6
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  or rax, rdi
  push rax
  mov rax, 7
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 8
  push rax
  mov rax, 3
  push rax
  mov rax, 5
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  and rax, rdi
  push rax
  pop rdi
  pop rax
  xor rax, rdi
  push rax
  pop rdi
  pop rax
  or rax, rdi
  push rax
  mov rax, 15
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR3
  push rax
  pop rdi
  mov eax, 0
  sub rsp, 8
  call printf
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  mov rax, 0
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  mov rax, 0
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 5
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  mov rax, 0
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 4
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 5
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  mov rax, 0
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR4
  push rax
  pop rdi
  mov eax, 0
  sub rsp, 8
  call printf
  add rsp, 8
  push rax
  pop rax
  mov rax, 2
  push rax
  mov rax, 2
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 2
  push rax
  mov rax, 3
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  mov rax, 0
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, 8
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  mov rax, 4
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, 7
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  mov rax, 0
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  pop rax
  cmp rax, 0
  je .L16
  mov rax, 5
  push rax
  pop rax
  cmp rax, 0
  je .L16
  mov rax, 1
  push rax
  jmp .L17
  pop rax
  cmp rax, 0
  je .L17
.L16:
  mov rax, 0
  push rax
.L17:
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  pop rax
  cmp rax, 0
  je .L18
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  je .L18
  mov rax, 1
  push rax
  jmp .L19
  pop rax
  cmp rax, 0
  je .L19
.L18:
  mov rax, 0
  push rax
.L19:
  mov rax, 0
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  je .L20
  mov rax, 5
  push rax
  pop rax
  cmp rax, 0
  je .L20
  mov rax, 1
  push rax
  jmp .L21
  pop rax
  cmp rax, 0
  je .L21
.L20:
  mov rax, 0
  push rax
.L21:
  mov rax, 0
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  je .L22
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  je .L22
  mov rax, 1
  push rax
  jmp .L23
  pop rax
  cmp rax, 0
  je .L23
.L22:
  mov rax, 0
  push rax
.L23:
  mov rax, 0
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  pop rax
  cmp rax, 0
  jne .L24
  mov rax, 5
  push rax
  pop rax
  cmp rax, 0
  jne .L24
  mov rax, 0
  push rax
  jmp .L25
  pop rax
  cmp rax, 0
  je .L25
.L24:
  mov rax, 1
  push rax
.L25:
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 3
  push rax
  pop rax
  cmp rax, 0
  jne .L26
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  jne .L26
  mov rax, 0
  push rax
  jmp .L27
  pop rax
  cmp rax, 0
  je .L27
.L26:
  mov rax, 1
  push rax
.L27:
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  jne .L28
  mov rax, 5
  push rax
  pop rax
  cmp rax, 0
  jne .L28
  mov rax, 0
  push rax
  jmp .L29
  pop rax
  cmp rax, 0
  je .L29
.L28:
  mov rax, 1
  push rax
.L29:
  mov rax, 1
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  jne .L30
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  jne .L30
  mov rax, 0
  push rax
  jmp .L31
  pop rax
  cmp rax, 0
  je .L31
.L30:
  mov rax, 1
  push rax
.L31:
  mov rax, 0
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR5
  push rax
  pop rdi
  mov eax, 0
  sub rsp, 8
  call printf
  add rsp, 8
  push rax
  pop rax
  mov rax, 1
  push rax
  pop rax
  cmp rax, 0
  je .L32
  mov rax, 6
  push rax
  jmp .L33
  pop rax
  cmp rax, 0
  je .L33
.L32:
  mov rax, 3
  push rax
.L33:
  mov rax, 6
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 0
  push rax
  pop rax
  cmp rax, 0
  je .L34
  mov rax, 6
  push rax
  jmp .L35
  pop rax
  cmp rax, 0
  je .L35
.L34:
  mov rax, 3
  push rax
.L35:
  mov rax, 3
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, OFFSET FLAT:.LSTR6
  push rax
  pop rdi
  mov eax, 0
  sub rsp, 8
  call printf
  add rsp, 8
  push rax
  pop rax
  mov rax, 0
  push rax
  mov rax, 0
  push rax
  mov rax, 10
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, 10
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 0
  push rax
  mov rax, 0
  push rax
  mov rax, 10
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, 10
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rax, 0
  push rax
  mov rax, 10
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, 20
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, 10
  push rax
  pop rsi
  pop rdi
  sub rsp, 8
  call assert
  add rsp, 8
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  .text
add:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov [rax], rdi
  lea rax, [rbp - 16]
  push rax
  pop rax
  mov [rax], rsi
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, [rbp - 16]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
  .text
test_global_variable:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  mov rax, OFFSET FLAT:g_b
  push rax
  mov rax, 14
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  mov rax, 0
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
  .text
test_return:
  push rbp
  mov rbp, rsp
  sub rsp, 0
  mov rax, 5
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rax, 123
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  .text
assert:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov [rax], rdi
  lea rax, [rbp - 16]
  push rax
  pop rax
  mov [rax], rsi
  lea rax, [rbp - 8]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  lea rax, [rbp - 16]
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .L36
  mov rax, 1
  push rax
  pop rdi
  call exit
  push rax
  pop rax
.L36:
  mov rax, 0
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
