# docker run -d -p 6666:6666 lilqcn/imsystem:v1
# 运行
# 构建成功后没有进行删除 镜像过大

FROM ubuntu:latest

MAINTAINER li liangquan

RUN apt-get update
RUN apt-get install -y gcc
RUN apt-get install -y make
RUN apt-get install -y cmake
RUN apt-get install -y git
# 安装libmysqlclient
RUN apt-get install -y libmysqlclient-dev

RUN mkdir ./home/temp
RUN mkdir ./home/run

add ./ /home/run

WORKDIR /home/temp

# 安装hiredis
RUN git clone https://github.com/redis/hiredis.git
WORKDIR /home/temp/hiredis
RUN make
RUN make install
RUN ldconfig
# 构建项目
WORKDIR /home/run
RUN cmake .
RUN make

CMD ["./demo"]