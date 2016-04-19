FROM gcc:4.8
RUN mkdir -p /src/myapp
ENV TIME_ZONE=Asia/Shanghai
RUN ln -snf /usr/share/zoneinfo/$TIME_ZONE /etc/localtime && echo $TIME_ZONE > /etc/timezone
RUN sysctl -w net.ipv4.tcp_syncookies=1
RUN sysctl -w net.ipv4.tcp_tw_reuse=1
RUN sysctl -w net.ipv4.tcp_tw_recycle=1
RUN sysctl -w net.ipv4.tcp_fin_timeout=30
RUN sysctl -w ipv4.tcp_keepalive_time =30
RUN sysctl -w net.ipv4.tcp_keepalive_intvl =10
RUN sysctl -w net.ipv4.tcp_keepalive_probes =3
RUN sysctl -p
COPY . /src/myapp
WORKDIR /src/myapp
RUN make clean && make
EXPOSE 8088
CMD ./start.sh
