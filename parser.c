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
  if (token->kind == TK_RETURN) {
    token = token->next;
    return true;
  }

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

bool consume_return() {
  if (token->kind == TK_RETURN) {
    token = token->next;
    return true;
  }
  return false;
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
    lvar->offset = 8;

    if (locals) {
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
  Node *node;

  if (consume_return()) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else {
    node = expr();
  }

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