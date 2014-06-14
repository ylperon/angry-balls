#include "webserver.hpp"
#include "handlers.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

void print_usage(const std::string& arg0)
{
    std::cout << "Usage: " << arg0 << " [--help]"
                                   << " [--dev]"
                                   << " [--http-port 9010]"
                                   << " [--game-server-host localhost]"
                                   << " [--game-server-port 9011]"
                                   << std::endl;
}

WebServerOptions parse_options(const std::vector<std::string>& args, bool& help_was_requested)
{
  WebServerOptions result;
  result.listen_port = 9010;
  auto it = args.begin();
  help_was_requested = false;
  while (it < args.end()) {
    const std::string& cur_arg = *it++;
    if (cur_arg == "--help") {
      help_was_requested = true;
      return result;
    } if (cur_arg == "--http-port") {
      result.listen_port = std::stoul(*it++);
    } else if (cur_arg == "--game-server-host") {
      result.game_server_host = *it++;
    } else if (cur_arg == "--game-server-port") {
      result.game_server_port = std::stoul(*it++);
    } else if (cur_arg == "--dev") {
      result.developer_mode = true;
    }
  }
  return result;
}

int main (int argc, char * argv[])
{
    bool help_was_requested;
    WebServerOptions options = parse_options(std::vector<std::string>(&argv[1], &argv[argc]),
            help_was_requested
            );
    if (help_was_requested) {
        print_usage(argv[0]);
        return 1;
    }

    ViewerClient client(options);
    WebServer server(options);
    if (!options.developer_mode) {
        server.url_handlers["/"] = index_handler;
        server.url_handlers["/jquery.js"] = jquery_handler;
        server.url_handlers["/app.js"] = app_js_handler;
    } else {
        std::cout << "Running in development mode:" << std::endl
                  << " /jquery.js => jquery-1.11.1.min.js" << std::endl
                  << " /app.js => app.js" << std::endl
                  << " / => index.html" << std::endl;
        server.url_handlers["/"] = [](const HttpRequest& req) {
                                       return file_handler("index.html", "text/html", req);
                                   };
        server.url_handlers["/app.js"] = [](const HttpRequest& req) {
                                             return file_handler("app.js", "text/javascript", req);
                                         };
        server.url_handlers["/jquery.js"] = [](const HttpRequest& req) {
                                                return file_handler("jquery-1.11.1.min.js",
                                                                    "text/javascript",
                                                                    req);
                                            };
    }
    server.url_handlers["/game_state"] = [&client](const HttpRequest& req) {
                                              return game_state_handler(client, req);
                                         };
    int rc_client, rc_webserver;
    std::thread client_thread = std::thread([&]() { rc_client = client.run(); });
    std::thread webserver_thread = std::thread([&]() { rc_webserver = server.run(); });
    client_thread.join();
    webserver_thread.join();
    int return_code = 0;
    if (0 != rc_client)
        return_code = rc_client;
    else if (0 != rc_webserver)
        return_code = rc_webserver;

    return return_code;
}
