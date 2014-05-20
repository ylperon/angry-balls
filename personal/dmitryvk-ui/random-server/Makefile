.PHONY: all

PROJECT_ROOT=../../..

# These are hardcoded values, which is bad
# Also this requires jsoncpp to be already built

JSONCPP_LIB_DIR=${PROJECT_ROOT}/contrib/jsoncpp/libs/linux-gcc-4.7.3
JSONCPP_INCLUDE_DIR=${PROJECT_ROOT}/contrib/jsoncpp/include/
JSONCPP_LIB_NAME=libjson_linux-gcc-4.7.3_libmt.a

all: random-server

random-server: server.hpp client-handler.cpp error-value.cpp main.cpp socket.cpp thread-pool.cpp server.cpp \
               ${PROJECT_ROOT}/include/protocol/parse_protocol.h ${PROJECT_ROOT}/include/util/basics.h \
               ${PROJECT_ROOT}/lib/protocol/parse_protocol.cpp
	g++ -std=c++11 -O2 -g -Wall -pedantic -pthread -o random-server client-handler.cpp error-value.cpp \
	main.cpp socket.cpp thread-pool.cpp server.cpp ${PROJECT_ROOT}/lib/protocol/parse_protocol.cpp \
	-I${PROJECT_ROOT}/include -I${JSONCPP_INCLUDE_DIR} \
	-L${PROJECT_ROOT}/contrib/jsoncpp/libs/linux-gcc-4.7.3 \
	-l:${JSONCPP_LIB_NAME}
