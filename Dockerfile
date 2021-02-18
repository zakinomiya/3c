FROM alpine

RUN apk update && apk add gcc make binutils libc-dev

WORKDIR ~/

CMD ["/bin/ash"]
