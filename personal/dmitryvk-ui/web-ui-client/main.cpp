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
  bool is_development_mode = argc == 2 && std::string(argv[1]) == "--dev";
  if (!is_development_mode) {
    server.url_handlers["/"] = index_handler;
  } else {
    cout << "Running in development mode:" << endl
         << " / => index.html" << endl;
    server.url_handlers["/"] = [](const HttpRequest& req) { return file_handler("index.html", "text/html", req); };
  }
  int rc = server.run();
  return rc;
}
