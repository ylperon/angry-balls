#include "webserver.hpp"
#include "handlers.hpp"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

int main (int argc, char * argv[]) {
  WebServerOptions options;
  options.listen_port = 9010;
  WebServer server(options);
  server.url_handlers["/"] = index_handler;
  int rc = server.run();
  return rc;
}
