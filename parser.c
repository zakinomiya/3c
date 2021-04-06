#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ccc.h"

static LVar *locals;  // if token is the same as op, read the next token;
                      // otherwise return error
static Token *expect(Token *tok, char op) {
  if (tok->kind != TK_RESERVED || strlen(&op) != tok->len ||
      memcmp(tok->str, &op, tok->len) != 0) {
    error_at("", "error happened, expected %c but given %s", op, tok->str);
  }
  return tok->next;
}

int expect_number(Token **token) {
  Token *tok = *token;
  if (tok->kind != TK_NUM) {
    fprintf(stderr, "given token kind is %d\n", tok->kind);
    fprintf(stderr, "given token str  is %s\n", tok->str);
    error_at(tok->str, "%s is not a number", tok->str);
  }

  int val = tok->val;
  *token = tok->next;
  return val;
}

bool equal(Token *tok, char *c) {
  if (tok->len == strlen(c) && memcmp(tok->str, c, tok->len) == 0) {
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

Node *primary(Token **token) {
  if (equal(*token, "(")) {
    *token = (*token)->next;
    Node *node = expr(token);
    *token = (*token)->next->next;
    expect(*token, ')');
    return node;
  }

  if ((*token)->kind == TK_IDENT) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(*token);
    if (lvar) {
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
    return node;
  }

  return new_node_num(expect_number(token));
}

Node *unary(Token **token) {
  if (equal(*token, "+")) {
    *token = (*token)->next;
    return primary(token);
  } else if (equal(*token, "-")) {
    *token = (*token)->next;
    return new_node(ND_SUB, new_node_num(0), primary(token));
  }
  return primary(token);
}

Node *mul(Token **token) {
  Node *node = unary(token);

  if (equal(*token, "*")) {
    *token = (*token)->next;
    node = new_node(ND_MUL, node, unary(token));
  } else if (equal(*token, "/")) {
    *token = (*token)->next;
    node = new_node(ND_DIV, node, unary(token));
  }
  return node;
}

Node *add(Token **token) {
  Node *node = mul(token);

  if (equal(*token, "+")) {
    *token = (*token)->next;
    node = new_node(ND_ADD, node, mul(token));
  } else if (equal(*token, "-")) {
    *token = (*token)->next;
    node = new_node(ND_SUB, node, mul(token));
  }
  return node;
}

Node *relational(Token **token) {
  Node *node = add(token);

  if (equal(*token, "<=")) {
    node = new_node(ND_SUB, node, mul(token));
    *token = (*token)->next;
    return new_node(ND_LTE, node, add(token));
  } else if (equal(*token, ">=")) {
    node = new_node(ND_SUB, node, mul(token));
    *token = (*token)->next;
    return new_node(ND_GTE, node, add(token));
  } else if (equal(*token, "<")) {
    *token = (*token)->next;
    return new_node(ND_LT, node, add(token));
  } else if (equal(*token, ">")) {
    *token = (*token)->next;
    return new_node(ND_GT, node, add(token));
  }
  return node;
}

Node *equality(Token **token) {
  Node *node = relational(token);

  if (equal(*token, "==")) {
    *token = (*token)->next;
    return new_node(ND_EQ, node, relational(token));
  } else if (equal(*token, "!=")) {
    *token = (*token)->next;
    return new_node(ND_NEQ, node, relational(token));
  } else {
    return node;
  }
}

Node *assign(Token **token) {
  Node *node = equality(token);

  if (equal(*token, "=")) {
    *token = (*token)->next;
    return new_node(ND_ASSIGN, node, assign(token));
  }

  return node;
}

Node *expr(Token **token) { return assign(token); }

Node *stmt(Token **token) {
  Node *node;

  if (equal(*token, "return")) {
    *token = (*token)->next;
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr(token);
    fprintf(stderr, "token now %d\n", (*token)->kind);
    fprintf(stderr, "token now%s\n", (*token)->str);
    expect(*token, ';');
    return node;
  }

  // if (equal(tok, "if")) {
  //  node = calloc(1, sizeof(Node));
  //}
  node = expr(token);
  expect(*token, ';');
  return node;
}

void program(Program *prog) {
  int i = 0;
  Token *cur = prog->tok;

  while (!at_eof(cur)) {
    prog->code[i] = stmt(&cur);
    cur = cur->next;
    i++;
  }

  prog->code[i] = NULL;
}

void parse(Program *prog) {
  locals = prog->locals;
  return program(prog);
}
