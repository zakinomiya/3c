#include "ccc.h"

static LVar *locals;
static Node *stmt(Token **token);
static Node *expr(Token **token);
static Node *compound_stmt(Token **token);

static void next(Token **token) {
  // fprintf(stderr, "%s,", strtk((*token)->kind));
  // fprintf(stderr, "%s,", (*token)->str);
  // fprintf(stderr, "%d,", (*token)->val);
  // fprintf(stderr, "%d\n", (*token)->len);

  *token = (*token)->next;
}

// if token is the same as op, read the next token;
// otherwise return error
static void expect(Token **token, char *op) {
  Token *tok = *token;
  if (tok->kind != TK_RESERVED || strlen(op) != tok->len ||
      memcmp(tok->str, op, tok->len) != 0) {
    fprintf(stderr, "given token kind is %d\n", tok->kind);
    fprintf(stderr, "given token len is %d\n", tok->len);
    fprintf(stderr, "given token str is %s\n", tok->str);
    error_at("", "error happened, expected %s but given %s", op, tok->str);
  }

  next(token);
}

static int expect_number(Token **token) {
  Token *tok = *token;
  if (tok->kind != TK_NUM) {
    fprintf(stderr, "given token kind is %d\n", tok->kind);
    fprintf(stderr, "given token len is %d\n", tok->len);
    fprintf(stderr, "given token str is %s\n", tok->str);
    error_at(tok->str, "%s is not a number", tok->str);
  }

  int val = tok->val;
  next(token);
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

static Node *new_node_block(Node *body) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_BLOCK;
  node->body = body;
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
    next(token);
    Node *node = expr(token);
    next(token);
    next(token);
    expect(token, ")");
    return node;
  }

  if ((*token)->kind == TK_IDENT) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->str = (*token)->str;

    if (equal((*token)->next, "(")) {
      node->kind = ND_FNCALL;
      node->name = (*token)->str;
      next(token);
      next(token);
      expect(token, ")");
      return node;
    }

    LVar *lvar = find_lvar(*token);
    if (lvar) {
      next(token);
      node->offset = lvar->offset;
      return node;
    }

    lvar = calloc(1, sizeof(LVar));
    lvar->len = (*token)->len;
    lvar->name = (*token)->str;
    lvar->offset = 48;

    if (locals) {
      lvar->next = locals;
      lvar->offset = locals->offset + 8;
    }

    node->offset = lvar->offset;
    locals = lvar;
    next(token);
    return node;
  }

  return new_node_num(expect_number(token));
}

static Node *unary(Token **token) {
  if (equal(*token, "+")) {
    next(token);
    return primary(token);
  } else if (equal(*token, "-")) {
    next(token);
    return new_node(ND_SUB, "-", new_node_num(0), primary(token));
  }
  return primary(token);
}

static Node *mul(Token **token) {
  Node *node = unary(token);

  if (equal(*token, "*")) {
    next(token);
    node = new_node(ND_MUL, "*", node, unary(token));
  } else if (equal(*token, "/")) {
    next(token);
    node = new_node(ND_DIV, "/", node, unary(token));
  }
  return node;
}

static Node *add(Token **token) {
  Node *node = mul(token);

  if (equal(*token, "+")) {
    next(token);
    node = new_node(ND_ADD, "+", node, add(token));
  } else if (equal(*token, "-")) {
    next(token);
    node = new_node(ND_SUB, "-", node, add(token));
  }

  return node;
}

static Node *relational(Token **token) {
  Node *node = add(token);

  if (equal(*token, "<=")) {
    // node = new_node(ND_SUB, (*token)->str, node, mul(token));
    next(token);
    return new_node(ND_LTE, "<=", node, add(token));
  } else if (equal(*token, ">=")) {
    // node = new_node(ND_SUB, (*token)->str, node, mul(token));
    next(token);
    return new_node(ND_GTE, ">=", node, add(token));
  } else if (equal(*token, "<")) {
    next(token);
    return new_node(ND_LT, "<", node, add(token));
  } else if (equal(*token, ">")) {
    next(token);
    return new_node(ND_GT, ">", node, add(token));
  }
  return node;
}

static Node *equality(Token **token) {
  Node *node = relational(token);

  if (equal(*token, "==")) {
    next(token);
    return new_node(ND_EQ, "==", node, relational(token));
  } else if (equal(*token, "!=")) {
    next(token);
    return new_node(ND_NEQ, "!=", node, relational(token));
  } else {
    return node;
  }
}

static Node *assign(Token **token) {
  Node *node = equality(token);

  if (equal(*token, "=")) {
    next(token);
    return new_node(ND_ASSIGN, "=", node, assign(token));
  }

  return node;
}

static Node *expr(Token **token) { return assign(token); }

// compound-stmt = stmt* "}"
static Node *compound_stmt(Token **token) {
  Node head = {};
  Node *cur = &head;

  while (!equal(*token, "}")) {
    cur = cur->next = stmt(token);
  }

  Node *node = new_node_block(head.next);
  expect(token, "}");
  return node;
}

// stmt = "return" expr ";"
//      | "{" compound-stmt
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | "while" "(" expr ")" stmt
//      | expr-stmt
static Node *stmt(Token **token) {
  Node *node;

  if (equal(*token, "return")) {
    next(token);
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr(token);
    expect(token, ";");
    return node;
  }

  if (equal(*token, "{")) {
    next(token);
    return compound_stmt(token);
  }

  if (equal(*token, "if")) {
    next(token);
    expect(token, "(");

    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    node->cond = expr(token);

    expect(token, ")");
    node->then = stmt(token);

    if (equal(*token, "else")) {
      next(token);
      node->els = stmt(token);
    }

    return node;
  }

  if (equal(*token, "while")) {
    next(token);
    expect(token, "(");

    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    node->cond = expr(token);

    expect(token, ")");
    node->then = stmt(token);

    return node;
  }

  if (equal(*token, "for")) {
    next(token);
    expect(token, "(");
    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;

    if (!equal(*token, ";")) {
      node->init = expr(token);
    }
    expect(token, ";");

    if (!equal(*token, ";")) {
      node->cond = expr(token);
    }
    expect(token, ";");

    if (!equal(*token, ")")) {
      node->inc = expr(token);
    }

    expect(token, ")");
    node->then = stmt(token);

    return node;
  }

  node = expr(token);
  expect(token, ";");
  return node;
}

// ToBe: function-definition = ident "(" ident? ("," ident)?  ")" "{"
// compound-stmt
//
// AsIs: function-definition = ident "("")" "{" compound-stmt
Node *function_def(Token **token) {
  if ((*token)->kind != TK_IDENT) {
    error("Failed to parse");
  }

  Node *node = calloc(1, sizeof(Node));
  node->name = (*token)->str;
  node->str = (*token)->str;
  node->kind = ND_BLOCK;
  node->is_func = true;

  next(token);

  expect(token, "(");
  expect(token, ")");
  expect(token, "{");

  node->body = compound_stmt(token);
  return node;
}

// program = function-definition
Node *program(Token **token) { return function_def(token); }

void parse(Program **prog) {
  Program *p = *prog;
  locals = p->locals;
  Token *cur = p->tok;

  Segment *cur_seg = calloc(1, sizeof(Segment));
  p->head = cur_seg;

  while (!at_eof(cur)) {
    cur_seg->contents = program(&cur);
    if (!at_eof(cur)) {
      cur_seg->next = calloc(1, sizeof(Segment));
      cur_seg = cur_seg->next;
    }
  }
}

