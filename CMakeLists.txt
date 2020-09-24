cmake_minimum_required(VERSION 3.15)
project(demo C)

set(CMAKE_C_STANDARD 99)

add_executable(demo main.c mysql_person.c mysql_person.h redis_mq.c redis_mq.h cJSON.c cJSON.h message.c message.h)
target_link_libraries(demo libmysqlclient.so)
target_link_libraries(demo libhiredis.so)
target_link_libraries(demo -lpthread -lm)