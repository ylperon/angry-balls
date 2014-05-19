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
    server.url_handlers["/jquery.js"] = jquery_handler;
    server.url_handlers["/app.js"] = app_js_handler;
  } else {
    cout << "Running in development mode:" << endl
         << " /jquery.js => jquery-1.11.1.min.js" << endl
         << " /app.js => app.js" << endl
         << " / => index.html" << endl;
    server.url_handlers["/"] = [](const HttpRequest& req) { return file_handler("index.html", "text/html", req); };
    server.url_handlers["/app.js"] = [](const HttpRequest& req) { return file_handler("app.js", "text/javascript", req); };
    server.url_handlers["/jquery.js"] = [](const HttpRequest& req) { return file_handler("jquery-1.11.1.min.js", "text/javascript", req); };
  }
  server.url_handlers["/game_state"] = game_state_handler;
  int rc = server.run();
  return rc;
}
