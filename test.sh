#!/bin/bash

CASENUM=0

assert() {
  expected="$1"
  input="$2"
  test_file_name="tmp_${CASENUM}.s"
  CASENUM=$((CASENUM+1))

  printf "Code: ${input}\n"
  ./ccc "${input}" > ${test_file_name}
  #./ccc "${input}" 

  cc -o tmp ${test_file_name}
  ./tmp 
  actual="$?"
if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
    echo "OK"
    #rm ./${test_file_name}
  else 
    echo "$expected expected, but got $actual"
    echo "Test Failed"
    exit 1
  fi
}

assert 5 "1+4 ; "
assert 5 "1+1+1+1+1 ; "
assert 5 "foo=1; foo+4 ; "
assert 6 "foo=1 ; foo = foo +4; 1 + foo;"
assert 10 "6+4;"
assert 7 "
 a = 3;
 b = 4;
 a + b;
"
assert 15 "
 foo = 10;
 bar = 5;
 foo + bar;
"
assert 10 "
 foo = 10;
 bar = 0;
 return foo + bar;
"
assert 42 "if (0) {return 42;}"
assert 10 "if (1-1) {return 10;}"
assert 15 "if (0==0) {return 15;}"
assert 10 "if (1==0) {return 15;} else {return 10;}"
assert 5 "
a = 5;
b = 0;
if (a>=b) {
  return a;
} else {
  return b;
}"
assert 0 "
a = 5;
b = 0;
if (1==0) {
  return a;
} else {
  return b;
}"
assert 6 "
a = 5;
b = 0;
if (a>=b) {
  return a + 1;
} else {
  return b;
}"
assert 0 "
a = 5;
b = 1;
if (1==0) {
  return a;
} else if (0==0) {
  return b-1;
} else {
  return 10;
}"
assert 10 "
if (1==0) {
  return a;
} else if (0==0) {
  b = 11; 
  return b-1;
} else {
  return 10;
}"
assert 10 "
if (0==0) 
  return 10;
"
assert 10 "
a = 1;
while (a < 10) {
  a = a +1;
}
return a;
"
assert 11 "
a = 1;
for (;a<11;a=a+1) {
  a = a;
}
return a;
"
