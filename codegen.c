#include "ccc.h"

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

void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
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

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(')');
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

Node *expr() {
  Node *node = equality();
  return node;
}
