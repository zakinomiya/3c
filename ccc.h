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
} NodeKind;

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_IDENT,
  TK_EOF,
  TK_RETURN,
  TK_IF,
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
  int val;
  int offset;
};

struct LVar {
  int offset;
  int len;
  char *name;
  LVar *next;
};

struct Program {
  char *input;
  int linenum;
  Token *tok;
  Node *code[100];
  LVar *locals;
};

// Function Declarations
void gen(Node *node);
void program();
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
void print_main();
void print_prologue(int offset);
void print_epilogue();
void parse(Program *prog);
bool consume(char *p);
void tokenize(Token *head, char *p);
Node *expr();

extern Program *prog;
