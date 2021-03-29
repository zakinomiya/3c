FROM ubuntu

RUN apt-get update && apt-get install -y gcc make binutils libc-dev gdb

WORKDIR ~/

CMD ["/bin/bash"]
