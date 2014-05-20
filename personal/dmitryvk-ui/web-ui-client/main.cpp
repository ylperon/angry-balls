#include "webserver.hpp"
#include "handlers.hpp"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

int main (int argc, char * argv[]) {
  WebServerOptions options;
  options.listen_port = 9010;
  ViewerClient client(options);
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
  server.url_handlers["/game_state"] = [&](const HttpRequest& req) { return game_state_handler(client, req); };
  int rc_client, rc_webserver;
  std::thread client_thread = std::thread([&]() { rc_client = client.run(); });
  std::thread webserver_thread = std::thread([&]() { rc_webserver = server.run(); });
  client_thread.join();
  webserver_thread.join();
  int rc = 0;
  if (rc_client != 0) {
    rc = rc_client;
  } else if (rc_webserver != 0) {
    rc = rc_webserver;
  }
  return rc;
}
