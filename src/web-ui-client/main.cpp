#include "webserver.h"
#include "handlers.h"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

void PrintUsage(const std::string& arg0)
{
    std::cout << "Usage: " << arg0 << " [--help]"
                                   << " [--dev]"
                                   << " [--http-port 9010]"
                                   << " [--game-server-host localhost]"
                                   << " [--game-server-port 9011]"
                                   << std::endl;
}

WebServerOptions ParseOptions(const std::vector<std::string>& args, bool& help_requested)
{
    WebServerOptions result;
    result.listen_port = 9010;
    auto it = args.begin();
    help_requested = false;
    while (it < args.end()) {
        const std::string& cur_arg = *it++;
        if (cur_arg == "--help") {
            help_requested = true;
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
    bool help_requested = false;
    const WebServerOptions options = ParseOptions(std::vector<std::string>(&argv[1], &argv[argc]),
                                                  help_requested
                                                 );
    if (help_requested) {
        PrintUsage(argv[0]);
        return 1;
    }

    ViewerClient client{options};
    WebServer server{options};
    if (!options.developer_mode) {
        server.url_handlers["/"] = IndexHandler;
        server.url_handlers["/jquery.js"] = JqueryHandler;
        server.url_handlers["/app.js"] = AppJsHandler;
    } else {
        std::cout << "Running in development mode:" << std::endl
                  << " /jquery.js => jquery-1.11.1.min.js" << std::endl
                  << " /app.js => app.js" << std::endl
                  << " / => index.html" << std::endl;
        server.url_handlers["/"] = [](const HttpRequest& req) {
                                       return FileHandler("index.html", "text/html", req);
                                   };
        server.url_handlers["/app.js"] = [](const HttpRequest& req) {
                                             return FileHandler("app.js", "text/javascript", req);
                                         };
        server.url_handlers["/jquery.js"] = [](const HttpRequest& req) {
                                                return FileHandler("jquery-1.11.1.min.js",
                                                                   "text/javascript",
                                                                   req);
                                            };
    }
    server.url_handlers["/game_state"] = [&client](const HttpRequest& req) {
                                              return GameStateHandler(client, req);
                                         };
    int rc_client = 0;
    int rc_webserver = 0;
    std::thread client_thread = std::thread([&client, &rc_client]() {
                                                rc_client = client.Run();
                                            }
                                           );
    std::thread webserver_thread = std::thread([&server, &rc_webserver]() {
                                                   rc_webserver = server.Run();
                                               }
                                              );
    client_thread.join();
    webserver_thread.join();
    int return_code = 0;
    if (0 != rc_client)
        return_code = rc_client;
    else if (0 != rc_webserver)
        return_code = rc_webserver;

    return return_code;
}
