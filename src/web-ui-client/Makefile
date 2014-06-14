.PHONY: all

all: ui-webserver ui-webserver.asan ui-webserver.tsan

PROJECT_ROOT=../..

JSONCPP_SRC_DIR=${PROJECT_ROOT}/contrib/jsoncpp/src/lib_json
JSONCPP_INCLUDE_DIR=${PROJECT_ROOT}/contrib/jsoncpp/include

index-html-content.cpp: index.html
	python2 bin2c.py index.html index_html_contents > index-html-content.cpp

jquery-js-content.cpp: jquery-1.11.1.min.js
	python2 bin2c.py jquery-1.11.1.min.js jquery_js_contents > jquery-js-content.cpp

app-js-content.cpp: app.js
	python2 bin2c.py app.js app_js_contents > app-js-content.cpp

ui-webserver: webserver.hpp handlers.hpp client-handler.cpp error-value.cpp main.cpp socket.cpp \
              thread-pool.cpp webserver.cpp http.cpp handlers.cpp index-html-content.cpp \
              jquery-js-content.cpp app-js-content.cpp viewer-client.cpp \
              ${PROJECT_ROOT}/include/protocol/protocol.h ${PROJECT_ROOT}/include/ab/units.h \
              ${PROJECT_ROOT}/lib/protocol/protocol.cpp ${JSONCPP_SRC_DIR}/json_value.cpp \
              ${JSONCPP_SRC_DIR}/json_reader.cpp ${JSONCPP_SRC_DIR}/json_writer.cpp
	clang++ -std=c++11 -stdlib=libc++ -O2 -g -Wall -Wextra -pedantic -pthread \
	-o ui-webserver client-handler.cpp \
	error-value.cpp main.cpp socket.cpp thread-pool.cpp webserver.cpp http.cpp handlers.cpp viewer-client.cpp \
	${PROJECT_ROOT}/lib/protocol/protocol.cpp ${JSONCPP_SRC_DIR}/json_value.cpp \
	${JSONCPP_SRC_DIR}/json_reader.cpp ${JSONCPP_SRC_DIR}/json_writer.cpp \
	-I${PROJECT_ROOT}/include -I${JSONCPP_INCLUDE_DIR}

ui-webserver.asan: webserver.hpp handlers.hpp client-handler.cpp error-value.cpp main.cpp socket.cpp \
              thread-pool.cpp webserver.cpp http.cpp handlers.cpp index-html-content.cpp \
              jquery-js-content.cpp app-js-content.cpp viewer-client.cpp \
              ${PROJECT_ROOT}/include/protocol/protocol.h ${PROJECT_ROOT}/include/ab/units.h \
              ${PROJECT_ROOT}/lib/protocol/protocol.cpp ${JSONCPP_SRC_DIR}/json_value.cpp \
              ${JSONCPP_SRC_DIR}/json_reader.cpp ${JSONCPP_SRC_DIR}/json_writer.cpp
	clang++ -std=c++11 -stdlib=libc++ -fsanitize=address -O1 -g -Wall -Wextra -pedantic -pthread \
	-o ui-webserver.asan client-handler.cpp \
	error-value.cpp main.cpp socket.cpp thread-pool.cpp webserver.cpp http.cpp handlers.cpp viewer-client.cpp \
	${PROJECT_ROOT}/lib/protocol/protocol.cpp ${JSONCPP_SRC_DIR}/json_value.cpp \
	${JSONCPP_SRC_DIR}/json_reader.cpp ${JSONCPP_SRC_DIR}/json_writer.cpp \
	-I${PROJECT_ROOT}/include -I${JSONCPP_INCLUDE_DIR}


ui-webserver.tsan: webserver.hpp handlers.hpp client-handler.cpp error-value.cpp main.cpp socket.cpp \
              thread-pool.cpp webserver.cpp http.cpp handlers.cpp index-html-content.cpp \
              jquery-js-content.cpp app-js-content.cpp viewer-client.cpp \
              ${PROJECT_ROOT}/include/protocol/protocol.h ${PROJECT_ROOT}/include/ab/units.h \
              ${PROJECT_ROOT}/lib/protocol/protocol.cpp ${JSONCPP_SRC_DIR}/json_value.cpp \
              ${JSONCPP_SRC_DIR}/json_reader.cpp ${JSONCPP_SRC_DIR}/json_writer.cpp
	clang++ -std=c++11 -stdlib=libstdc++ -fsanitize=thread -O1 -g -Wall -Wextra -pedantic -pthread \
	-o ui-webserver.tsan client-handler.cpp \
	error-value.cpp main.cpp socket.cpp thread-pool.cpp webserver.cpp http.cpp handlers.cpp viewer-client.cpp \
	${PROJECT_ROOT}/lib/protocol/protocol.cpp ${JSONCPP_SRC_DIR}/json_value.cpp \
	${JSONCPP_SRC_DIR}/json_reader.cpp ${JSONCPP_SRC_DIR}/json_writer.cpp \
	-I${PROJECT_ROOT}/include -I${JSONCPP_INCLUDE_DIR}
