#pragma once
#include "webserver.hpp"

const HttpResponse index_handler(const HttpRequest& request);
const HttpResponse app_js_handler(const HttpRequest& request);
const HttpResponse jquery_handler(const HttpRequest& request);
const HttpResponse file_handler(const std::string& filename,
                                const std::string& content_type,
                                const HttpRequest& request);
const HttpResponse game_state_handler(ViewerClient& client, const HttpRequest& request);
