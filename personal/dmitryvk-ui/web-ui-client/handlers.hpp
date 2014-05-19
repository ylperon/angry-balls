#pragma once
#include "webserver.hpp"

const HttpResponse index_handler(const HttpRequest& request);
const HttpResponse file_handler(const std::string& filename,
                                const std::string& content_type,
                                const HttpRequest& request);
