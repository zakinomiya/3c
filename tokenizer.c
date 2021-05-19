#include <stdlib.h>
#include <string.h>

#include "ccc.h"

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  // allocate memory address to the new token
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->len = len;
  tok->str = calloc(1, sizeof(str));
  strncpy(tok->str, str, len);
  // new token as the next token of the current one
  cur->next = tok;
  return tok;
}

bool startswith(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

int is_alnum(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') || (c == '_');
}

// Split a string into tokens
void tokenize(Token *head, char *p) {
  // set head as the current token
  Token *cur = head;

  while (*p) {
    // skip space
    if (isspace(*p) || *p == '\n') {
      p++;
      continue;
    }

    if (startswith(p, "return") && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }

    if (startswith(p, "for") && !is_alnum(p[3])) {
      cur = new_token(TK_FOR, cur, p, 3);
      p += 3;
      continue;
    }

    if (startswith(p, "while") && !is_alnum(p[5])) {
      cur = new_token(TK_FOR, cur, p, 5);
      p += 5;
      continue;
    }

    if (startswith(p, "if") && !is_alnum(p[2])) {
      cur = new_token(TK_IF, cur, p, 2);
      p += 2;
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      int i = 0;
      while (is_alnum(*(p + i))) {
        i++;
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
        *p == ')' || *p == '<' || *p == '>' || *p == '=' || *p == ';' ||
        *p == '{' || *p == '}') {
      cur = new_token(TK_RESERVED, cur, p, 1);
      p++;
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
}

