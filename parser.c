#include "ccc.h"

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  // allocate memory address to the new token
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  // new token as the next token of teh current one
  cur->next = tok;
  return tok;
}

bool startswith(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

// Split a string into tokens
Token *tokenize(char *p) {
  // init head token
  Token head;
  head.next = NULL;
  // set head as the current token
  Token *cur = &head;

  while (*p) {
    // skip space
    if (isspace(*p) || *p == '\n') {
      p++;
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      int i = 0;
      char c = *(p + i);
      while ('a' <= c && c <= 'z') {
        i++;
        c = *(p + i);
      }
      cur = new_token(TK_IDENT, cur, p, i);
      p += i;
      continue;
    }

    if (startswith(p, "!=") || startswith(p, "==") || startswith(p, "<=") ||
        startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // if next letter is '+' or '-' create token and set the new one as current
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '<' || *p == '>' || *p == '=' || *p == ';') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // if next letter is numeric, create a new token and set the value
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("Failed to tokenize", *p);
  }
  // create EOF token
  new_token(TK_EOF, cur, p, 1);
  return head.next;
}

