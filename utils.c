#include "ccc.h"

char* strndk(NodeKind nk) {
  switch (nk) {
    case ND_ADD:
      return "ND_ADD";
    case ND_SUB:
      return "ND_SUB";
    case ND_MUL:
      return "ND_MUL";
    case ND_DIV:
      return "ND_DIV";
    case ND_NUM:
      return "ND_NUM";
    case ND_LT:
      return "ND_LT";
    case ND_GT:
      return "ND_GT";
    case ND_LTE:
      return "ND_LTE";
    case ND_GTE:
      return "ND_GTE";
    case ND_EQ:
      return "ND_EQ";
    case ND_NEQ:
      return "ND_NEQ";
    case ND_ASSIGN:  // assignment(=)
      return "ND_ASSIGN";
    case ND_LVAR:  // local variable
      return "ND_LVAR";
    case ND_RETURN:
      return "ND_RETURN";
    case ND_IF:
      return "ND_IF";
    case ND_FOR:
      return "ND_FOR";
    case ND_WHILE:
      return "ND_WHILE";
    case ND_BLOCK:
      return "ND_BLOCK";
    case ND_FNCALL:
      return "ND_FNCALL";
    case ND_FN:
      return "ND_FN";
    default:
      return "ND_UNKNOWN";
  }
}

char* strtk(TokenKind tk) {
  switch (tk) {
    case TK_RESERVED:
      return "TK_RESERVED";
    case TK_NUM:
      return "TK_NUM";
    case TK_IDENT:
      return "TK_IDENT";
    case TK_EOF:
      return "TK_EOF";
    case TK_RETURN:
      return "TK_RETURN";
    case TK_IF:
      return "TK_IF";
    case TK_FOR:
      return "TK_FOR";
    case TK_WHILE:
      return "TK_WHILE";
    default:
      return "TK_UNKNOWN";
  }
}
