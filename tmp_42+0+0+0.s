.intel_syntax noprefix
.global main
main:
  push 42
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  ret
