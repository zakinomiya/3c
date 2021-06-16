#!/bin/bash

CASENUM=0

assert() {
  expected="$1"
  input="$2"
  test_file_name="tmp_${CASENUM}.s"
  CASENUM=$((CASENUM+1))

  ./ccc "${input}" > ${test_file_name}
  #./ccc "${input}" 

  cc -o tmp ${test_file_name}
  ./tmp 
  actual="$?"
if [ "$actual" = "$expected" ]; then
    echo "Case: $CASENUM"
    echo " => OK"
    #rm ./${test_file_name}
  else 
    echo "---- Failed ----"
    echo " Case:     ${CASENUM}"
    echo " Expected: ${expected}" 
    echo " Got:      ${actual}" 
    echo "----- Code -----"
    printf "${input}\n"
    echo "----------------"
    exit 1
  fi
}

assert 5 "main () {
  a = ( 2 * 2 ) + 1;
  return a;
}"
assert 5 "main () {
  a=1;
  1+4; 
}"
assert 5 "main () {
  return 1+1+1+1+1; 
}
"
assert 5 "main () {
  foo=2;
  foo+3;
}"
assert 6 "main () {
  foo=1; 
  foo = foo +4;
  return 1 + foo;
}
"
assert 10 "main () {
  return 6+4;
}
"
assert 7 "main() {
 a = 3;
 b = 4;
 return a + b;
}
"
assert 15 "main () {
 foo = 10;
 bar = 5;
 return foo + bar;
}
"
assert 10 "main () {
 foo = 10;
 bar = 0;
 return foo + bar;
}
"
assert 42 "main(){
  if (1) {
    return 42;
  }
}
"
assert 10 "main () {
  if (1-0) {
    return 10;
  }
}
"
assert 15 "main() {
  if (0==0) {
    return 15;
  }
}
"
assert 10 "main () {
  if (1==0) {
    return 15;
  } else {
    return 10;
  }
}"
assert 5 "main() {
  a = 5;
  b = 0;
  if (a>=b) {
    return a;
  } else {
    return b;
  }
}"
assert 0 "main() {
  a = 5;
  b = 0;
  if (1==0) {
    return a;
  } else {
    return b;
  }
}"
assert 6 "main () {
  a = 5;
  b = 0;
  if (a>=b) {
    return a + 1;
  } else {
    return b;
  }
}"
assert 0 "main () {
  a = 5;
  b = 1;
  if (1==0) {
    return a;
  } else if (0==0) {
    return b-1;
  } else {
    return 10;
  }
}"
assert 10 "main () {
  if (1==0) {
    return a;
  } else if (0==0) {
    b = 11; 
    return b-1;
  } else {
    return 10;
  }
}"
assert 10 "main() {
  if (0==0) 
    return 10;
}
"
assert 5 "main() {
  if (0==1) 
    return 10;
  return 5;
}
"
assert 10 "main() {
  a = 1;
  while (a < 10) {
    a = a +1;
  }
  return a;
}
"
assert 11 "main (){
  for (a=0;a<11;a=a+1) {
    a = a;
  }
  return a;
}
"
assert 11 "main (){
  a = 0;
  for (;a<11;a=a+1) {
    a = a;
  }
  return a;
}
"
assert 11 "main (){
  a = 0;
  for (;a < 11;) {
    a = a + 1;
  }
  return a;
}
"
assert 11 "main (){
  a = 0;
  for (;;) {
    a = a + 1;
    if (a==11) {
      return a;
    }
  }
  return a;
}
"
assert 14 "
fn_test() {
  b = 13;
  return b;
}

main (){
  a = 1;
  a = a +  fn_test();
  return a;
}
"

assert 11 "
fn_test() {
  b = 0;
  while (b < 10) {
    b = b + 1;
  }
  return b;
}

main (){
  a = 1;
  a = a +  fn_test();
  return a;
}
"
assert 10 "
echo(a) {
  return a;
}

main (){
  return echo(10);
}
"
assert 6 "
sum(a, b, c) {
  return a + b + c;
}

main (){
  return sum(1, 2, 3);
}
"
assert 3 "
sum(a, b, c) {
  return a + b + c;
}

main (){
  return sum(1, 1, 1);
}

"
assert 45 "
sum(a, b, c) {
  return a + b + c;
}

main (){
  a = sum(1,2,3);
  b = sum(4,5,6);
  c = sum(7,8,9);
  return sum(a,b,c);
}
"

assert 45 "
sum(a, b, c) {
  return a + b + c;
}

main (){
  return sum(sum(1,2,3),sum(4,5,6),sum(7,8,9));
}
"

assert 27 "
mul(a, b, c) {
  return a * b * c;
}

main () {
  return mul(3, 3, 3);
}
"

assert $((2160 % 256)) "
sum(a, b, c) {
  return a + b + c;
}

mul(a, b, c) {
  return a * b * c;
}

main (){
  a = sum(1,2,3);
  b = sum(4,5,6);
  c = sum(7,8,9);
  return mul(a,b,c);
}
"

assert 27 "
sum (a, b, c) {
  return a + b + c;
}

nine() {
  return 9;
}

main () {
  return sum(nine(), nine(), nine());
}
"

assert 0 "
fibonacci(a) {
  if (a <= 1) {
    return a;
  }

  return fibonacci(a - 1) + fibonacci(a - 2);
}

main (){
  if (fibonacci(2) != 1) {
    return 1;
  }

  if (fibonacci(9) != 34) {
    return 1;
  }

  return 0;
}
"

assert 3 "
main() {
  x = 3;
  y = &x;

  return *y;
}
"

