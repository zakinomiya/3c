#!/bin/ash

assert() {
  expected="$1"
  input="$2"

  ./ccc "$input" > tmp.s

  cc -o tmp tmp.s
  ./tmp 
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else 
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert $@

echo "OK"
