# Compact C Compiler (3C)

reference: https://www.sigbus.info/compilerbook

# EBNF Syntax

```
program = compound_stmt*
compound_stmt* = "{" stmt* "}"  
      | "if" "(" expr ")" "{" compound_stmt "}" ("else" "{" compound_stmt "}")?
stmt  = expr* ";"
      | "return" expr ";"
expr = assign*
assign = equality ("=" assign)*
equality = relational("==" relational | "!=" relational)*
relational = add ("<" add | ">" add | "<=" add | ">=" add)*
add = mul ("+" mul | "-" mul)*
mul = unary ("*" unary | "/" unary)*
unary = ("+" | "-")? primary
primary = num | "(" expr ")"
```

# Hello

<div>
      this is div
</div>

<ul>
      <li>a</li>
      <li>b</li>
      <li>c</li>
      <li>d</li>
</ul>