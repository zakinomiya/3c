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
    echo "$input => $actual"
    echo "OK"
    rm ./${test_file_name}
  else 
    echo "$input => $expected expected, but got $actual"
    echo "Test Failed"
    exit 1
  fi
}

assert 5 "1+4 ; "
assert 6 "a=1 ; a = a +4; 1 + 5;"
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
