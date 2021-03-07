CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

RUN=docker run --rm -v ${PWD}:/ccc -w /ccc ccc 

$(OBJS): ccc.h

ccc: $(OBJS)
		${RUN} cc -o ccc ${SRCS} ${CFLAGS}

docker_build:
		docker build -t "ccc" .

test: ccc
		${RUN} ./test.sh

clean:
	rm tmp* ccc *.o || true

.PHONY: test clean
