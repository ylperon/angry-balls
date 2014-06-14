#include "handlers.hpp"

#include <fstream>

#include "index-html-content.h"
#include "jquery-js-content.h"
#include "app-js-content.h"

HttpResponse index_handler(const HttpRequest& request)
{
    HttpResponse result;
    result.version = request.version;
    result.status_code = 200;
    result.status_message = "OK";
    result.headers.push_back(HttpHeader("content-type", "text/html; charset=utf-8"));

    std::string body(reinterpret_cast<const char*>(index_html_contents),
                     sizeof(index_html_contents)
                    );
    result.response_body = std::vector<unsigned char>(body.begin(), body.end());
    return result;
}

HttpResponse jquery_handler(const HttpRequest& request)
{
    HttpResponse result;
    result.version = request.version;
    result.status_code = 200;
    result.status_message = "OK";
    result.headers.push_back(HttpHeader("content-type", "text/javascript; charset=utf-8"));

    std::string body(reinterpret_cast<const char*>(jquery_js_contents),
                     sizeof(jquery_js_contents)
                    );
    result.response_body = std::vector<unsigned char>(body.begin(), body.end());
    return result;
}

HttpResponse app_js_handler(const HttpRequest& request)
{
    HttpResponse result;
    result.version = request.version;
    result.status_code = 200;
    result.status_message = "OK";
    result.headers.push_back(HttpHeader("content-type", "text/javascript; charset=utf-8"));

    std::string body(reinterpret_cast<const char*>(app_js_contents), sizeof(app_js_contents));
    result.response_body = std::vector<unsigned char>(body.begin(), body.end());
    return result;
}

HttpResponse file_handler(const std::string& filename,
                          const std::string& content_type,
                          const HttpRequest& request)
{
    std::vector<unsigned char> buf;
    std::ifstream file(filename, std::ios_base::in | std::ios_base::binary);
    file.seekg(0, std::ios_base::end);
    size_t length = file.tellg();
    file.seekg(0);
    buf.resize(length);
    file.read(reinterpret_cast<char*>(buf.data()), length);
    file.close();

    HttpResponse result;
    result.version = request.version;
    result.status_code = 200;
    result.status_message = "OK";
    result.headers.push_back(HttpHeader("content-type", content_type));

    result.response_body.swap(buf);
    return result;
}

HttpResponse game_state_handler(ViewerClient& client, const HttpRequest& request)
{
    HttpResponse result;
    result.version = request.version;
    result.status_code = 200;
    result.status_message = "OK";
    result.headers.push_back(HttpHeader("content-type", "application/json; charset=utf-8"));

    ab::FieldState field;
    bool have_field;
    client.get_field(field, have_field);
    std::string body;
    if (have_field) {
        ab::FieldStateMessage msg;
        msg.field_state = field;

        body = ab::BuildJsonMessage(&msg);
    } else {
        body = "null";
    }

    result.response_body = std::vector<unsigned char>(body.begin(), body.end());
    return result;
}
