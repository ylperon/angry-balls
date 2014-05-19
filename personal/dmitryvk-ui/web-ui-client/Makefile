.PHONY: all

all: ui-webserver

ui-webserver: webserver.hpp handlers.hpp client-handler.cpp error-value.cpp main.cpp socket.cpp thread-pool.cpp webserver.cpp http.cpp handlers.cpp
	g++ -std=c++11 -O2 -g -Wall -pedantic -pthread -o ui-webserver client-handler.cpp error-value.cpp main.cpp socket.cpp thread-pool.cpp webserver.cpp http.cpp handlers.cpp
