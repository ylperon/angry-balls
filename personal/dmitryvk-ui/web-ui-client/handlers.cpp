#include "handlers.hpp"

using std::string;
using std::vector;

const HttpResponse index_handler(const HttpRequest& request) {
  HttpResponse result;
  result.version = request.version;
  result.status_code = 200;
  result.status_message = "OK";
  result.headers.push_back(HttpHeader("content-type", "text/html; charset=utf-8"));
  
  string body = "<html><body><h1>Визуализатор: TODO</h1></body></html>";
  result.response_body = vector<unsigned char>(body.begin(), body.end());
  return result;
}
