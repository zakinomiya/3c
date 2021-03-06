CFLAGS=-std=c11 -g -static

RUN=docker run --rm -v ${PWD}:/ccc -w /ccc ccc 

ccc: 
		${RUN} cc ${CFLAGS} *.c -o ccc

docker_build:
		docker build -t "ccc" .

test: ccc
		${RUN} ./test.sh

clean:
	rm tmp* ccc || true

.PHONY: test clean
