#include <stdlib.h>
#include <string.h>

#include "ccc.h"

static LVar *locals;

// if token is the same as op, read the next token; otherwise return error
static Token *expect(Token *tok, char op) {
  if (tok->kind != TK_RESERVED || strlen(&op) != tok->len ||
      memcmp(tok->str, &op, tok->len) != 0) {
    error_at("", tok->str, 1, "'%c' not given", op);
  }
  return tok->next;
}

int expect_number(Token *tok) {
  if (tok->kind != TK_NUM) {
    error_at(tok->str, "Not a number");
  }

  int val = tok->val;
  tok = tok->next;
  return val;
}

bool equal(Token *tok, char *c) {
  if (tok->len == strlen(c) && memcmp(tok->str, c, tok->len)) {
    return true;
  }

  return false;
}

bool at_eof(Token *tok) { return tok->kind == TK_EOF; }

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

Node *primary(Token *tok) {
  if (equal(tok, "(")) {
    Node *node = expr(tok);
    expect(tok, ')');
    return node;
  }

  if (tok->kind == TK_IDENT) {
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

  return new_node_num(expect_number(tok));
}

Node *unary(Token *tok) {
  if (equal(tok, "+")) {
    return primary(tok);
  } else if (equal(tok, "-")) {
    return new_node(ND_SUB, new_node_num(0), primary(tok));
  }
  return primary(tok);
}

Node *mul(Token *tok) {
  Node *node = unary(tok);

  if (equal(tok, "*")) {
    node = new_node(ND_MUL, node, unary(tok));
  } else if (equal(tok, "/")) {
    node = new_node(ND_DIV, node, unary(tok));
  }
  return node;
}

Node *add(Token *tok) {
  Node *node = mul(tok);

  if (equal(tok, "+")) {
    node = new_node(ND_ADD, node, mul(tok));
  } else if (equal(tok, "-")) {
    node = new_node(ND_SUB, node, mul(tok));
  }
  return node;
}

Node *relational(Token *tok) {
  Node *node = add(tok);

  if (equal(tok, "<=")) {
    return new_node(ND_LTE, node, add(tok));
  } else if (equal(tok, ">=")) {
    return new_node(ND_GTE, node, add(tok));
  } else if (equal(tok, "<")) {
    return new_node(ND_LT, node, add(tok));
  } else if (equal(tok, ">")) {
    return new_node(ND_GT, node, add(tok));
  }
  return node;
}

Node *equality(Token *tok) {
  Node *node = relational(tok);

  if (equal(tok, "==")) {
    return new_node(ND_EQ, node, relational(tok));
  } else if (equal(tok, "!=")) {
    return new_node(ND_NEQ, node, relational(tok));
  } else {
    return node;
  }
}

Node *assign(Token *tok) {
  Node *node = equality(tok);

  if (equal(tok, "=")) {
    return new_node(ND_ASSIGN, node, assign(tok->next));
  }

  return node;
}

Node *expr(Token *tok) { return assign(tok); }

Node *stmt(Token *tok) {
  Node *node;

  if (equal(tok, "return")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr(tok);
    expect(tok, ';');
    return node;
  }

  if (equal(tok, "if")) {
    node = calloc(1, sizeof(Node));
  }
  return expr(tok);
}

void program(Program *prog) {
  int i = 0;

  while (!at_eof(prog->tok)) {
    prog->code[i] = stmt(prog->tok);
    i++;
  }

  prog->code[i] = NULL;
}

void parse(Program *prog) {
  locals = prog->locals;
  return program(prog);
}
