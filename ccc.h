// Type definitions
typedef struct Node Node;
typedef struct Token Token;
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
} NodeKind;

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
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
};

// Functions Declarations
void gen(Node *node);
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
bool consume(char *p);
Token *tokenize(char *p);
Node *expr();
