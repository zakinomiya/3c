#include <stdlib.h>

#include "ccc.h"

Node *code[100];
LVar *locals;

// if token is the same as op, read the next token; otherwise return error
void expect(char op) {
  if (token->kind != TK_RESERVED || strlen(&op) != token->len ||
      memcmp(token->str, &op, token->len) != 0) {
    error_at(token->str, "'%c' not given", op);
  }
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM) {
    error_at(token->str, "Not a number");
  }

  int val = token->val;

  token = token->next;
  return val;
}

// if token is the same as op, return true; otherwise return false.
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len) != 0) {
    return false;
  }

  token = token->next;
  return true;
}

Token *consume_ident() {
  if (token->kind == TK_IDENT) {
    Token *tok = token;
    token = token->next;
    return tok;
  }

  return NULL;
}

bool at_eof() { return token->kind == TK_EOF; }

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
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

void gen(Node *node) {
  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
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
    case ND_ASSIGN:
      // push the lval address to the stack
      gen_lval(node->lhs);
      // push the num value to the stack. or push another variable to the stack
      gen(node->rhs);

      // load the value
      printf("  pop rdi\n");
      // load the lval address
      printf("  pop rax\n");
      // copy the value in the rdi to the address to which the  value in the rax
      // points
      printf("  mov [rax], rdi\n");
      // push the value to the stack
      printf("  push rdi\n");
      return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

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
    case ND_NUM:
      break;
  }

  printf("  push rax\n");
}

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, tok->len)) {
      return var;
    }
  }

  return NULL;
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(')');
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
      return node;
    }

    lvar = calloc(1, sizeof(LVar));
    lvar->len = tok->len;
    lvar->name = tok->str;

    if (!locals) {
      lvar->offset = 8;
    } else {
      lvar->next = locals;
      lvar->offset = locals->offset + 8;
    }

    node->offset = lvar->offset;
    locals = lvar;
    return node;
  }

  return new_node_num(expect_number());
}

Node *unary() {
  if (consume("+")) {
    return primary();
  } else if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }
  return primary();
}

Node *mul() {
  Node *node = unary();

  if (consume("*")) {
    node = new_node(ND_MUL, node, unary());
  } else if (consume("/")) {
    node = new_node(ND_DIV, node, unary());
  }
  return node;
}

Node *add() {
  Node *node = mul();

  if (consume("+")) {
    node = new_node(ND_ADD, node, mul());
  } else if (consume("-")) {
    node = new_node(ND_SUB, node, mul());
  }
  return node;
}

Node *relational() {
  Node *node = add();

  if (consume("<=")) {
    return new_node(ND_LTE, node, add());
  } else if (consume(">=")) {
    return new_node(ND_GTE, node, add());
  } else if (consume("<")) {
    return new_node(ND_LT, node, add());
  } else if (consume(">")) {
    return new_node(ND_GT, node, add());
  }
  return node;
}

Node *equality() {
  Node *node = relational();

  if (consume("==")) {
    return new_node(ND_EQ, node, relational());
  } else if (consume("!=")) {
    return new_node(ND_NEQ, node, relational());
  } else {
    return node;
  }
}

Node *assign() {
  Node *node = equality();

  if (consume("=")) {
    return new_node(ND_ASSIGN, node, assign());
  }

  return node;
}

Node *expr() { return assign(); }

Node *stmt() {
  Node *node = expr();
  expect(';');
  return node;
}

void program() {
  int i = 0;

  while (!at_eof()) {
    code[i] = stmt();
    i++;
  }

  code[i] = NULL;
}

void parse() { return program(); }
