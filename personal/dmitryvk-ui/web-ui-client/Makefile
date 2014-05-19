.PHONY: all

all: ui-webserver

index-html-content.cpp: index.html
	python2 bin2c.py index.html index_html_contents > index-html-content.cpp

jquery-js-content.cpp: jquery-1.11.1.min.js
	python2 bin2c.py jquery-1.11.1.min.js jquery_js_contents > jquery-js-content.cpp

app-js-content.cpp: app.js
	python2 bin2c.py app.js app_js_contents > app-js-content.cpp

ui-webserver: webserver.hpp handlers.hpp client-handler.cpp error-value.cpp main.cpp socket.cpp thread-pool.cpp webserver.cpp http.cpp handlers.cpp index-html-content.cpp jquery-js-content.cpp app-js-content.cpp
	g++ -std=c++11 -O2 -g -Wall -pedantic -pthread -o ui-webserver client-handler.cpp error-value.cpp main.cpp socket.cpp thread-pool.cpp webserver.cpp http.cpp handlers.cpp
