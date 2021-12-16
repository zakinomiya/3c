#include <stdlib.h>
#include <string.h>

#include "ccc.h"

void gen(Node *node);
void print_comment(char *fmt, ...);
static Var *current_func;

// get register name in accordance with the x86-64 calling convention
static char *get_register(int i) {
  switch (i) {
    case 1:
      return "rdi";
    case 2:
      return "rsi";
    case 3:
      return "rdx";
    case 4:
      return "rcx";
    case 5:
      return "r8";
    case 6:
      return "r9";
    default:
      error("Use stack!");
  };
}

static int count() {
  static int cnt = 0;
  cnt++;
  return cnt;
}

void print_prologue(Var *func) {
  print_comment("Prologue\n");
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", (int)(8 * func->argc));
}

void print_epilogue() {
  printf(".L.return.%s:\n", current_func->name);
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}

// gen_lval pushes the address of left side value to the stack
void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("Invalid syntax. Assignment needed");
  }

  // rax now has the base pointer address
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
  int nest_c = count();
  printf("# node kind is %s\n", strndk(node->kind));
  printf("# node str is %s\n", node->str);
  printf("# node val is %d\n", node->val);

  if (node->body) {
    printf("# ---- Body %d\n", nest_c);
    check_ast(node->body);
  }

  if (node->lhs) {
    printf("# ---- LHS %d\n", nest_c);
    check_ast(node->lhs);
  }

  if (node->rhs) {
    printf("# ---- RHS %d\n", nest_c);
    check_ast(node->rhs);
  }

  if (node->next) {
    printf("#      NEXT \n");
    check_ast(node->next);
  }

  if (node->params) {
    printf("#      PARAMS \n");
    check_ast(node->params);
  }
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
  if (node->kind == ND_BLOCK) {
    return gen(node->body);
  }

  switch (node->kind) {
    case ND_FNCALL: {
      print_comment("FNCALL\n");

      int i = 0;
      for (Node *params = node->params; params; params = params->nextp) {
        i++;
        gen(params);
      }

      while (i > 0) {
        printf("  pop %s\n", get_register(i));
        i--;
      }
      printf("  call .L.fn.%s\n", node->str);
      printf("  push rax\n");

      break;
    }

    case ND_ADDR:
      print_comment("ADDR\n");

      gen_lval(node->lhs);
      break;
    case ND_DEREF:
      print_comment("DEREF\n");

      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      break;
    case ND_RETURN: {
      print_comment("RETURN\n");
      if (node->lhs) gen(node->lhs);
      printf("  pop rax\n");
      printf("  jmp .L.return.%s\n", current_func->name);
      break;
    }
    case ND_IF: {
      print_comment("IF\n");
      int c = count();
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 1\n");

      printf("  je .L.end.%d\n", c);
      printf("  jne .L.else.%d\n", c);

      printf(".L.end.%d:\n", c);
      gen(node->then);
      printf("  jmp .L.rest.%d\n", c);

      printf(".L.else.%d:\n", c);
      if (node->els) {
        gen(node->els);
      }

      printf(".L.rest.%d:\n", c);
      if (node->next) {
        gen(node->next);
      }

      break;
    }
    case ND_FOR: {
      print_comment("FOR\n");
      int c = count();
      if (node->init) {
        gen(node->init);
      }
      printf(".L.begin.%d:\n", c);
      if (node->cond) {
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 1\n");
        printf("  jne .L.break.%d\n", c);
      }

      gen(node->then);

      if (node->inc) {
        gen(node->inc);
      }

      printf("  jmp .L.begin.%d\n", c);
      printf(".L.break.%d:\n", c);
      break;
    }
    case ND_NUM:
      print_comment("NUM\n");
      printf("  push %d\n", node->val);
      break;
    case ND_ASSIGN:
      print_comment("ASSIGN\n");
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
      break;
    case ND_LVAR:
      print_comment("LVAR\n");

      // get the value of the local variable and push it to the stack
      // This case is always read after ND_ASSIGN is read beacause of the rule
      // of the tree structure.
      gen_lval(node);
      // pop the value(address to the lval) from the stack and stores it to the
      // rax registre
      printf("  pop rax\n");
      // read the value from the memory to which the value in the rax points and
      // copy it to the rax
      printf("# variable %s\n", node->str);
      printf("  mov rax, [rax]\n");
      // push the value in the rax to the stack
      printf("  push rax\n");
      break;
  }

  if (node->next) {
    return gen(node->next);
  }

  if (node->rhs && node->lhs) {
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
  }

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
      break;
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
    default:
      print_comment("# no operator found\n");
      return;
  }

  printf("  push rax\n");
}

static void gen_func(Segment *seg) {
  for (Segment *cur_seg = seg; cur_seg; cur_seg = cur_seg->next) {
    Var *seg_head = cur_seg->contents;

    if (!seg_head->is_func) continue;

    current_func = seg_head;

    printf("# ---- AST ----\n");
    check_ast(seg_head->body);
    printf("# -------------\n");

    if (memcmp(seg_head->name, "main", 4) == 0) {
      printf("\n%s:\n", seg_head->name);
      print_prologue(seg_head);
    } else {
      printf("\n.L.fn.%s:\n", seg_head->name);
      print_prologue(seg_head);
      printf("  movq  [rbp - %d], rdi\n", 8);
      printf("  movq  [rbp - %d], rsi\n", 16);
      printf("  movq  [rbp - %d], rdx\n", 24);
      printf("  movq  [rbp - %d], rcx\n", 32);
      printf("  movq  [rbp - %d], r8\n", 40);
      printf("  movq  [rbp - %d], r9\n", 48);
    }

    gen(seg_head->body);
    print_epilogue();
  }
}

void codegen(Program *prog) {
  printf("    .intel_syntax noprefix\n\n");
  printf("    .globl main\n");
  printf(".text\n");

  Segment *cur_seg = prog->head;
  gen_func(cur_seg);
}
