.PHONY: all

PROJECT_ROOT=../../..

JSONCPP_SRC_DIR=${PROJECT_ROOT}/contrib/jsoncpp/src/lib_json
JSONCPP_INCLUDE_DIR=${PROJECT_ROOT}/contrib/jsoncpp/include

all: random-server

random-server: server.hpp client-handler.cpp error-value.cpp main.cpp socket.cpp thread-pool.cpp server.cpp \
               ${PROJECT_ROOT}/include/protocol/parse_protocol.h ${PROJECT_ROOT}/include/util/basics.h \
               ${PROJECT_ROOT}/lib/protocol/parse_protocol.cpp ${JSONCPP_SRC_DIR}/json_value.cpp \
               ${JSONCPP_SRC_DIR}/json_reader.cpp ${JSONCPP_SRC_DIR}/json_writer.cpp
	g++ -std=c++11 -O2 -g -Wall -pedantic -pthread -o random-server client-handler.cpp error-value.cpp \
	main.cpp socket.cpp thread-pool.cpp server.cpp ${PROJECT_ROOT}/lib/protocol/parse_protocol.cpp \
	${JSONCPP_SRC_DIR}/json_value.cpp ${JSONCPP_SRC_DIR}/json_reader.cpp ${JSONCPP_SRC_DIR}/json_writer.cpp \
	-I${PROJECT_ROOT}/include -I${JSONCPP_INCLUDE_DIR}
