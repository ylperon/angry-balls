#include "handlers.hpp"

#include <fstream>

using std::string;
using std::vector;
using std::ifstream;

#include "index-html-content.cpp"
#include "jquery-js-content.cpp"
#include "app-js-content.cpp"

const HttpResponse index_handler(const HttpRequest& request) {
  HttpResponse result;
  result.version = request.version;
  result.status_code = 200;
  result.status_message = "OK";
  result.headers.push_back(HttpHeader("content-type", "text/html; charset=utf-8"));
  
  string body(reinterpret_cast<const char*>(index_html_contents), sizeof(index_html_contents));
  result.response_body = vector<unsigned char>(body.begin(), body.end());
  return result;
}

const HttpResponse jquery_handler(const HttpRequest& request) {
  HttpResponse result;
  result.version = request.version;
  result.status_code = 200;
  result.status_message = "OK";
  result.headers.push_back(HttpHeader("content-type", "text/javascript; charset=utf-8"));
  
  string body(reinterpret_cast<const char*>(jquery_js_contents), sizeof(jquery_js_contents));
  result.response_body = vector<unsigned char>(body.begin(), body.end());
  return result;
}

const HttpResponse app_js_handler(const HttpRequest& request) {
  HttpResponse result;
  result.version = request.version;
  result.status_code = 200;
  result.status_message = "OK";
  result.headers.push_back(HttpHeader("content-type", "text/javascript; charset=utf-8"));
  
  string body(reinterpret_cast<const char*>(app_js_contents), sizeof(app_js_contents));
  result.response_body = vector<unsigned char>(body.begin(), body.end());
  return result;
}

const HttpResponse file_handler(const std::string& filename,
                                const std::string& content_type,
                                const HttpRequest& request) {
  vector<unsigned char> buf;
  {
    ifstream file(filename, std::ios_base::in | std::ios_base::binary);
    file.seekg(0, std::ios_base::end);
    size_t length = file.tellg();
    file.seekg(0);
    buf.resize(length);
    file.read(reinterpret_cast<char*>(buf.data()), length);
  }
  
  HttpResponse result;
  result.version = request.version;
  result.status_code = 200;
  result.status_message = "OK";
  result.headers.push_back(HttpHeader("content-type", content_type));
  
  result.response_body.swap(buf);
  return result;
}

const HttpResponse game_state_handler(ViewerClient& client, const HttpRequest& request) {
  HttpResponse result;
  result.version = request.version;
  result.status_code = 200;
  result.status_message = "OK";
  result.headers.push_back(HttpHeader("content-type", "application/json; charset=utf-8"));
  
  ab::FieldState field;
  bool have_field;
  client.get_field(field, have_field);
  string body;
  if (have_field) {
    ab::FieldStateMessage msg;
    msg.field_state = field;

    body = ab::BuildJsonMessage(&msg);
  } else {
    body = "null";
  }

  result.response_body = vector<unsigned char>(body.begin(), body.end());
  return result;
}
