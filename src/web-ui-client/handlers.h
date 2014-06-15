#pragma once

#include <string>

struct HttpRequest;
struct HttpResponse;
class ViewerClient;

HttpResponse IndexHandler(const HttpRequest& request);
HttpResponse AppJsHandler(const HttpRequest& request);
HttpResponse JqueryHandler(const HttpRequest& request);
HttpResponse FileHandler(const std::string& filename,
                         const std::string& content_type,
                         const HttpRequest& request);
HttpResponse GameStateHandler(ViewerClient& client, const HttpRequest& request);
