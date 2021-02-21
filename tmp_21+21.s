.intel_syntax noprefix
.global main
main:
  push 21
  push 21
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  ret
