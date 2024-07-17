#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

char *token_kind_from_int(int kind) {
  switch (kind) {
  case TK_RESERVED:
    return "TK_RESERVED";
  case TK_NUM:
    return "TK_NUM";
  case TK_EOF:
    return "TK_EOF";
  default:
    return "UNKNOWN";
  }
}

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
};

Token *token;

Token *new_token(TokenKind kind, char *str, Token *cur) {
  Token *tok = calloc(1, sizeof(Token));
  tok->str = str;
  tok->kind = kind;
  cur->next = tok;
  return tok;
}

void dump_head(Token *h) {
  printf("Kind: %s\n", token_kind_from_int(h->kind));
  printf("Str: %s\n", h->str);
  printf("Val: %d\n", h->val);
  printf("--------\n");
}

void dump_tokens(Token *head) {
  Token *h = head;
  int depth = 0;
  while (h) {
    printf("Depth: %d\n", depth);
    dump_head(h);
    h = h->next;
    depth++;
  }
}

Token *tokenize(char *input) {
  Token *head = calloc(1, sizeof(Token));
  Token *cur = head;
  char *p = input;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, p++, cur);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, p, cur);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    if (!*p) {
      cur = new_token(TK_RESERVED, "", cur);
      break;
    }
  }

  new_token(TK_EOF, p, cur);
  return head->next;
}

bool at_eof(Token *head) { return head->kind == TK_EOF; }

void errorf(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

int must_read_number(Token *head) {
  if (head->kind != TK_NUM) {
    dump_head(head);
    errorf("expected num token but given %v\n", head);
  }
  int val = head->val;
  return val;
}

void must_read(Token *head, char op) {
  if (head->kind != TK_RESERVED || head->str[0] != op) {
    dump_head(head);
    errorf("expected %s but given %v\n", op, head);
  }
}

void code_gen(Token *head) {
  Token *h = head;

  printf("  mov rax, %d\n", must_read_number(head));
  h = h->next;
  while (!at_eof(h)) {
    if (h->str[0] == '+') {
      h = h->next;
      printf("  add rax, %d\n", must_read_number(h));
      h = h->next;
    } else if (h->str[0] == '-') {
      h = h->next;
      printf("  sub rax, %d\n", must_read_number(h));
      h = h->next;
    }
    /*dump_tokens(h);*/
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "arg count is invalid \n");
    return 1;
  }

  Token *tok = tokenize(argv[1]);
  /*dump_tokens(tok);*/

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  code_gen(tok);
}
