.intel_syntax noprefix
.global main
  .data
.LSTR0:
  .string "mcc2 test...\n\n"
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
  mov rax, 0
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
