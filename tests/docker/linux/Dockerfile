FROM ubuntu:bionic

VOLUME /opt/melon
RUN apt-get update && apt-get install -y --force-yes cmake && apt-get install -y --force-yes build-essential
RUN apt-get update && apt-get install -y --force-yes libbsd-dev
COPY melon_test.sh /usr/local/bin
CMD /usr/local/bin/melon_test.sh