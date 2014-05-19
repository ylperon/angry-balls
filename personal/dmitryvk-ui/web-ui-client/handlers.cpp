#include "handlers.hpp"

#include <fstream>

using std::string;
using std::vector;
using std::ifstream;

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
