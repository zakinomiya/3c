CFLAGS=-std=c11 -ggdb -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
D_RUN=docker run --ulimit="core=65536" --rm -v ${PWD}:/ccc -w /ccc 

RUN=${D_RUN} ccc 

$(OBJS): ccc.h

ccc: $(OBJS)
		${RUN} cc -o ccc ${SRCS} ${CFLAGS}

docker_build:
		docker build -t "ccc" .

debug:
		${RUN} gdb ccc core

test: ccc
		${RUN} ./test.sh

clean:
	rm -f tmp* ccc *.o core || true

.PHONY: test clean
