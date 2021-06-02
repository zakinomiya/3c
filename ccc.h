#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Type definitions
typedef struct Node Node;
typedef struct Token Token;
typedef struct LVar LVar;
typedef struct Program Program;
typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
  ND_LT,
  ND_GT,
  ND_LTE,
  ND_GTE,
  ND_EQ,
  ND_NEQ,
  ND_ASSIGN,  // assignment(=)
  ND_LVAR,    // local variable
  ND_RETURN,
  ND_IF,
  ND_FOR,
  ND_WHILE,
  ND_BLOCK,
  ND_FNCALL,
} NodeKind;

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_IDENT,
  TK_EOF,
  TK_RETURN,
  TK_IF,
  TK_FOR,
  TK_WHILE,
} TokenKind;

// Struct definitions
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;

  // Block Node
  Node *body;
  int val;
  int offset;
  char *str;

  // if and for
  Node *cond;
  Node *then;
  Node *els;
  Node *init;
  Node *inc;

  // func
  char *name;
  bool is_func;

  Node *next;
};

struct LVar {
  int offset;
  int len;
  char *name;
  LVar *next;
};

typedef struct Segment Segment;
struct Segment {
  Node *contents;
  Segment *next;
};

struct Program {
  char *input;
  int linenum;
  Token *tok;
  Segment *head;
  LVar *locals;
};

// Function Declarations
void codegen(Program *prog);
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
void error(char *fmt, ...);
void parse(Program **prog);
void tokenize(Token *head, char *p);
char *strndk(NodeKind nk);
char *strtk(TokenKind tk);

extern Program *prog;
