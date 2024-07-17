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

assert 10 "1+1+1+7"

# assert 5 "int main () {
#   int a = ( 2 * 2 ) + 1;
#   return a;
# }"
# assert 5 "int main () {
#   int a=1;
#   1+4; 
# }"
# assert 5 "int main () {
#   return 1+1+1+1+1; 
# }
# "
# assert 5 "int main () {
#   int foo=2;
#   foo+3;
# }"
# assert 6 "int main () {
#   int foo=1; 
#   foo = foo +4;
#   return 1 + foo;
# }
# "
# assert 10 "int main () {
#   return 6+4;
# }
# "
# assert 7 "int main() {
#  int  a = 3;
#  int b = 4;
#  return a + b;
# }
# "
# assert 15 "int main () {
#  int foo = 10;
#  int bar = 5;
#  return foo + bar;
# }
# "
# assert 10 "int main () {
#  int foo = 10;
#  int bar = 0;
#  return foo + bar;
# }
# "
# assert 42 "int main(){
#   if (1) {
#     return 42;
#   }
# }
# "
#
# assert 10 "int main () {
#   if (1-0) {
#     return 10;
#   }
# }
# "
# assert 15 "int main() {
#   if (0==0) {
#     return 15;
#   }
# }
# "
# assert 10 "int main () {
#   if (1==0) {
#     return 15;
#   } else {
#     return 10;
#   }
# }"
# assert 5 "int main() {
#   int a = 5;
#   int b = 0;
#   if (a>=b) {
#     return a;
#   } else {
#     return b;
#   }
# }"
# assert 0 "int main() {
#   int a = 5;
#   int b = 0;
#   if (1==0) {
#     return a;
#   } else {
#     return b;
#   }
# }"
# assert 6 "int main () {
#   int a = 5;
#   int b = 0;
#   if (a>=b) {
#     return a + 1;
#   } else {
#     return b;
#   }
# }"
# assert 0 "int main () {
#   int a = 5;
#   int b = 1;
#   if (1==0) {
#     return a;
#   } else if (0==0) {
#     return b-1;
#   } else {
#     return 10;
#   }
# }"
# assert 10 "int main () {
#   if (1==0) {
#     return a;
#   } else if (0==0) {
#     int b = 11; 
#     return b-1;
#   } else {
#     return 10;
#   }
# }"
# assert 10 "int main() {
#   if (0==0) 
#     return 10;
# }
# "
# assert 5 "int main() {
#   if (0==1) 
#     return 10;
#   return 5;
# }
# "
#
# assert 10 "int main() {
#   int a = 1;
#   while (a < 10) {
#     a = a +1;
#   }
#   return a;
# }
# "
# assert 11 "int main (){
#   for (int a=0;a<11;a=a+1) {
#     a = a;
#   }
#   return a;
# }
# "
# assert 11 "int main (){
#   int a = 0;
#   for (;a<11;a=a+1) {
#     a = a;
#   }
#   return a;
# }
# "
# assert 11 "int main (){
#   int a = 0;
#   for (;a < 11;) {
#     a = a + 1;
#   }
#   return a;
# }
# "
# assert 11 "int main (){
#   int a = 0;
#   for (;;) {
#     a = a + 1;
#     if (a==11) {
#       return a;
#     }
#   }
#   return a;
# }
# "
# assert 14 "
# int fn_test() {
#   int b = 13;
#   return b;
# }
#
# int main (){
#   int a = 1;
#   a = a +  fn_test();
#   return a;
# }
# "
#
# assert 11 "
# int fn_test() {
#   int b = 0;
#   while (b < 10) {
#     b = b + 1;
#   }
#   return b;
# }
#
# int main (){
#   int a = 1;
#   a = a +  fn_test();
#   return a;
# }
# "
# assert 10 "
# int echo(int a) {
#   return a;
# }
#
# int main (){
#   return echo(10);
# }
# "
# assert 6 "
# int sum(int a, int b, int c) {
#   return a + b + c;
# }
#
# int main (){
#   return sum(1, 2, 3);
# }
# "
# assert 3 "
# int sum(int a, int b, int c) {
#   return a + b + c;
# }
#
# int main (){
#   return sum(1, 1, 1);
# }
#
# "
#
# assert 45 "
# int sum(int a, int b, int c) {
#   return a + b + c;
# }
#
# int main (){
#   int a = sum(1,2,3);
#   int b = sum(4,5,6);
#   int c = sum(7,8,9);
#   return sum(a,b,c);
# }
# "
#
# assert 45 "
# int sum(int a, int b, int c) {
#   return a + b + c;
# }
#
# int main (){
#   return sum(sum(1,2,3),sum(4,5,6),sum(7,8,9));
# }
# "
#
# assert 27 "
# int mul(int a, int b, int c) {
#   return a * b * c;
# }
#
# int main () {
#   return mul(3, 3, 3);
# }
# "
#
# assert $((2160 % 256)) "
# int sum(int a, int b, int c) {
#   return a + b + c;
# }
#
# int mul(int a, int b, int c) {
#   return a * b * c;
# }
#
# int main (){
#   int a = sum(1,2,3);
#   int b = sum(4,5,6);
#   int c = sum(7,8,9);
#   return mul(a,b,c);
# }
# "
#
# assert 27 "
# int sum(int a, int b, int c) {
#   return a + b + c;
# }
#
# int nine() {
#   return 9;
# }
#
# int main () {
#   return sum(nine(), nine(), nine());
# }
# "
#
# assert 0 "
# int fibonacci(int a) {
#   if (a <= 1) {
#     return a;
#   }
#
#   return fibonacci(a - 1) + fibonacci(a - 2);
# }
#
# int main (){
#   if (fibonacci(2) != 1) {
#     return 1;
#   }
#
#   if (fibonacci(9) != 34) {
#     return 1;
#   }
#
#   return 0;
# }
# "
#
# assert 3 "
# int main() {
#   int x = 3;
#   int y = &x;
#
#   return *y;
# }
# "
#
