#pragma once

#include <string>

struct HttpRequest;
struct HttpResponse;
class ViewerClient;

HttpResponse index_handler(const HttpRequest& request);
HttpResponse app_js_handler(const HttpRequest& request);
HttpResponse jquery_handler(const HttpRequest& request);
HttpResponse file_handler(const std::string& filename,
                          const std::string& content_type,
                          const HttpRequest& request);
HttpResponse game_state_handler(ViewerClient& client, const HttpRequest& request);
