FROM archlinux
ADD . /root/catbin
WORKDIR /root/catbin
RUN pacman -Sy --noconfirm make gcc libmicrohttpd leveldb
RUN make
RUN make install
RUN pacman -Rs --noconfirm make gcc
EXPOSE 5454/tcp
EXPOSE 80/tcp
RUN mkdir /var/lib/catbind
ENTRYPOINT ./bin/catbind -h localhost -w./src/webcontent
