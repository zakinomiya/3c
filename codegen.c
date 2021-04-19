#include <stdlib.h>
#include <string.h>

#include "ccc.h"

Node *code[100];
LVar *locals;

static int count() {
  static int cnt = 0;
  cnt++;
  return cnt;
}

void print_main() {
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
}

void print_prologue(int offset) {
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", offset);
}

void print_epilogue() {
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}

// gen_lval pushes the address of left side value to the stack
void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("Invalid syntax. Assignment needed");
  }

  // rax now has to the base pointer address
  printf("  mov rax, rbp\n");
  // subtract the node offset from the rax(base) registre
  // rax now has the address to the lval
  printf("  sub rax, %d\n", node->offset);
  // push the value(adreess to the lval) in the rax registre to the stack
  printf("  push rax\n");
}

void print_node(Node *node) {
  printf("# node kind is %d\n", node->kind);
  printf("# node str is %s\n", node->str);
  printf("# node val is %d\n", node->val);
}

void check_ast(Node *node) {
  printf("# node kind is %d\n", node->kind);
  printf("# node str is %s\n", node->str);
  printf("# node val is %d\n", node->val);

  if (node->lhs) check_ast(node->lhs);
  if (node->rhs) check_ast(node->rhs);
  return;
}

void print_comment(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  char cfmt[256] = "# ";
  strcat(cfmt, fmt);
  printf(cfmt, ap);
  va_end(ap);
}

void gen(Node *node) {
  // check_ast(node);
  print_node(node);
  switch (node->kind) {
    case ND_RETURN:
      gen(node->lhs);
      printf(" pop rax\n");
      printf(" mov rsp, rbp\n");
      printf(" pop rbp\n");
      printf(" ret\n");
      return;
    case ND_IF:
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");

      int c = count();
      printf("  je .Lend.%d\n", c);
      printf(".Lend.%d:\n", c);

      gen(node->then);
      if (node->els) {
        printf(".Lelse.%d:\n", c);
        gen(node->els);
      }
      return;
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_ASSIGN:
      // push the lval address to the stack
      gen_lval(node->lhs);
      // push the num value to the stack. or push another variable to the stack
      gen(node->rhs);

      print_comment("load the value\n");
      printf("  pop rdi\n");

      print_comment("load the lval address\n");
      printf("  pop rax\n");
      print_comment(
          "copy the value in the rdi to the address to which the value in the "
          "rax points\n");
      printf("  mov [rax], rdi\n");
      // push the value to the stack
      printf("  push rdi\n");
      return;
    case ND_LVAR:
      // get the value of the local variable and push it to the stack
      // This case is always read after ND_ASSIGN is read beacause of the rule
      // of the tree structure.
      gen_lval(node);
      // pop the value(address to the lval) from the stack and stores it to the
      // rax registre
      printf("  pop rax\n");
      // read the value from the memory to which the value in the rax points and
      // copy it to the rax
      printf("  mov rax, [rax]\n");
      // push the value in the rax to the stack
      printf("  push rax\n");
      return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  // operation
  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
    case ND_NEQ:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_GT:
      printf("  cmp rax, rdi\n");
      printf("  setg al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LTE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_GTE:
      printf("  cmp rax, rdi\n");
      printf("  setge al\n");
      printf("  movzb rax, al\n");
      break;
  }

  printf("  push rax\n");
}

