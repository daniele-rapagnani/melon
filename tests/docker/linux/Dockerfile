FROM ubuntu:bionic

VOLUME /opt/melon
RUN apt-get update && apt-get install -y --force-yes cmake && apt-get install -y --force-yes build-essential
RUN apt-get update && apt-get install -y --force-yes libbsd-dev
RUN apt-get update && apt-get install -y --force-yes gcc-multilib g++-multilib
RUN dpkg --add-architecture i386 && apt-get update
RUN apt-get update && apt-get install -y --force-yes libc6-dbg:i386
RUN apt-get update && apt-get install -y --force-yes gdb
COPY melon_test.sh /usr/local/bin
CMD /usr/local/bin/melon_test.sh