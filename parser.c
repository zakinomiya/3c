#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ccc.h"

static LVar *locals;
static Node *expr(Token **token);

static void advance(Token **token) {
  fprintf(stderr, "%d", (*token)->kind);
  fprintf(stderr, "%s", (*token)->str);
  fprintf(stderr, "%d\n", (*token)->len);

  *token = (*token)->next;
}

// if token is the same as op, read the next token;
// otherwise return error
static void expect(Token **token, char op) {
  Token *tok = *token;
  if (tok->kind != TK_RESERVED || strlen(&op) == tok->len ||
      memcmp(tok->str, &op, tok->len) != 0) {
    fprintf(stderr, "given token kind is %d\n", tok->kind);
    fprintf(stderr, "given token len is %d\n", tok->len);
    fprintf(stderr, "given token str  is %s\n", tok->str);
    error_at("", "error happened, expected %c but given %s", op, tok->str);
  }

  advance(token);
}

static int expect_number(Token **token) {
  Token *tok = *token;
  if (tok->kind != TK_NUM) {
    error_at(tok->str, "%s is not a number", tok->str);
  }

  int val = tok->val;
  advance(token);
  return val;
}

static bool equal(Token *tok, char *c) {
  if (tok->len == strlen(c) && memcmp(tok->str, c, tok->len) == 0) {
    return true;
  }

  return false;
}

static bool at_eof(Token *tok) { return tok->kind == TK_EOF; }

static Node *new_node(NodeKind kind, char *str, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  node->str = str;
  return node;
}

static Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

static LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, tok->len)) {
      return var;
    }
  }

  return NULL;
}

static Node *primary(Token **token) {
  if (equal(*token, "(")) {
    advance(token);
    Node *node = expr(token);
    advance(token);
    advance(token);
    expect(token, ')');
    return node;
  }

  if ((*token)->kind == TK_IDENT) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->str = (*token)->str;

    LVar *lvar = find_lvar(*token);
    if (lvar) {
      advance(token);
      node->offset = lvar->offset;
      return node;
    }

    lvar = calloc(1, sizeof(LVar));
    lvar->len = (*token)->len;
    lvar->name = (*token)->str;
    lvar->offset = 8;

    if (locals) {
      lvar->next = locals;
      lvar->offset = locals->offset + 8;
    }

    node->offset = lvar->offset;
    locals = lvar;
    advance(token);
    return node;
  }

  return new_node_num(expect_number(token));
}

static Node *unary(Token **token) {
  if (equal(*token, "+")) {
    advance(token);
    return primary(token);
  } else if (equal(*token, "-")) {
    advance(token);
    return new_node(ND_SUB, "-", new_node_num(0), primary(token));
  }
  return primary(token);
}

static Node *mul(Token **token) {
  Node *node = unary(token);

  if (equal(*token, "*")) {
    advance(token);
    node = new_node(ND_MUL, "*", node, unary(token));
  } else if (equal(*token, "/")) {
    advance(token);
    node = new_node(ND_DIV, "/", node, unary(token));
  }
  return node;
}

static Node *add(Token **token) {
  Node *node = mul(token);

  if (equal(*token, "+")) {
    advance(token);
    node = new_node(ND_ADD, "+", node, add(token));
  } else if (equal(*token, "-")) {
    advance(token);
    node = new_node(ND_SUB, "-", node, add(token));
  }

  return node;
}

static Node *relational(Token **token) {
  Node *node = add(token);

  if (equal(*token, "<=")) {
    // node = new_node(ND_SUB, (*token)->str, node, mul(token));
    advance(token);
    return new_node(ND_LTE, "<=", node, add(token));
  } else if (equal(*token, ">=")) {
    // node = new_node(ND_SUB, (*token)->str, node, mul(token));
    advance(token);
    return new_node(ND_GTE, ">=", node, add(token));
  } else if (equal(*token, "<")) {
    advance(token);
    return new_node(ND_LT, "<", node, add(token));
  } else if (equal(*token, ">")) {
    advance(token);
    return new_node(ND_GT, ">", node, add(token));
  }
  return node;
}

static Node *equality(Token **token) {
  Node *node = relational(token);

  if (equal(*token, "==")) {
    advance(token);
    return new_node(ND_EQ, "==", node, relational(token));
  } else if (equal(*token, "!=")) {
    advance(token);
    return new_node(ND_NEQ, "!=", node, relational(token));
  } else {
    return node;
  }
}

static Node *assign(Token **token) {
  Node *node = equality(token);

  if (equal(*token, "=")) {
    advance(token);
    return new_node(ND_ASSIGN, "=", node, assign(token));
  }

  return node;
}

static Node *expr(Token **token) { return assign(token); }

static Node *stmt(Token **token) {
  Node *node;

  if (equal(*token, "return")) {
    advance(token);
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr(token);
    expect(token, ';');
    return node;
  }

  // if (equal(tok, "if")) {
  //  node = calloc(1, sizeof(Node));
  //}
  node = expr(token);
  expect(token, ';');

  return node;
}

void check_node(Node *node) {
  if (!node) {
    printf("no node provided");
    return;
  }

  printf("%d\n", node->kind);
  printf("%s\n", node->str);

  if (node->lhs) check_node(node->lhs);
  if (node->rhs) check_node(node->rhs);
}

static void program(Program *prog) {
  int i = 0;
  Token *cur = prog->tok;

  while (!at_eof(cur)) {
    Node *node = stmt(&cur);
    prog->code[i] = node;
    // check_node(node);
    i++;
  }

  prog->code[i] = NULL;
}

void parse(Program *prog) {
  locals = prog->locals;
  return program(prog);
}
