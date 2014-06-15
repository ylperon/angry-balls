#include "webserver.h"

#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

const Url Url::Parse(const std::string& url_string)
{
    auto scheme_separator_pos = url_string.find("://");
    Url result;
    result.port = 0;
    std::string path_and_query;
    if (std::string::npos == scheme_separator_pos) {
        path_and_query = url_string;
    } else {
        result.scheme = url_string.substr(0, scheme_separator_pos);
        auto authority_start = scheme_separator_pos + 3;
        auto authority_separator_pos = url_string.find("/", authority_start);
        if (std::string::npos == authority_separator_pos)
            authority_separator_pos = url_string.length();

        path_and_query = url_string.substr(authority_separator_pos);
        std::string authority = url_string.substr(authority_start, authority_separator_pos);
        auto port_separator_pos = authority.find(":");
        if (std::string::npos == port_separator_pos) {
            result.host = authority;
        } else {
            result.host = authority.substr(0, port_separator_pos);
            result.port = std::stoul(authority.substr(port_separator_pos + 1));
        }
    }
    if (path_and_query.empty())
        path_and_query = "/";

    auto query_separator_pos = path_and_query.find("?");
    if (std::string::npos == query_separator_pos) {
        result.absolute_path = path_and_query;
    } else {
        result.absolute_path = path_and_query.substr(0, query_separator_pos);
        result.query = path_and_query.substr(query_separator_pos + 1);
    }
    return result;
}


std::string HttpHeader::NormalizeName(const std::string& name)
{
    std::string result = name;

    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

HttpRequest HttpRequest::Parse(const std::vector<unsigned char>& body)
{
    HttpRequest result;
    std::string body_str = std::string(body.begin(), body.end());
    auto line_start = std::string::size_type(0);
    auto line_end = body_str.find("\r\n", line_start);

    std::string req_line = body_str.substr(line_start, line_end - line_start);
    std::istringstream req_stream(req_line);
    std::string verb;
    std::string url;
    std::string http_and_version;
    req_stream >> verb >> url >> http_and_version;
    result.verb = verb;
    result.url = Url::Parse(url);
    result.version = http_and_version.substr(http_and_version.find("/") + 1);
    line_start = line_end + 2;

    while (true) {
        line_end = body_str.find("\r\n", line_start);
        if (line_start == line_end) {
            line_start = line_end + 2;
            break;
        } else {
            std::string header_line = body_str.substr(line_start, line_end - line_start);
            auto colon_pos = header_line.find(":");
            std::string header_name = header_line.substr(0, colon_pos);
            auto value_start = colon_pos + 1;
            while (value_start < header_line.length()
                    && (' ' == header_line.at(value_start)
                        || '\t' == header_line.at(value_start)))
            {
                ++value_start;
            }
            auto value_end = header_line.size();
            while (value_end > value_start
                    && (' ' == header_line.at(value_end - 1)
                        || '\t' == header_line.at(value_end - 1)))
            {
                --value_end;
            }
            std::string header_value = header_line.substr(value_start, value_end - value_start);
            result.headers.push_back(HttpHeader(header_name, header_value));
            line_start = line_end + 2;
        }
    }
    return result;
}

std::vector<unsigned char> HttpResponse::Serialize() const
{
    std::ostringstream stream;
    stream << "HTTP/" << version << " " << status_code << " " << status_message << "\r\n";
    for (auto header: headers) {
        if (header.name == "content-length")
            continue;
        // TODO: если header.value содержит CRLF, то его надо разбить на несколько строк,
        // но это не очень важно
        stream << header.name << ": " << header.value << "\r\n";
    }
    stream << "content-length: " << response_body.size() << "\r\n";
    stream << "\r\n";

    stream << std::string(response_body.begin(), response_body.end());
    std::string result_string = stream.str();
    return std::vector<unsigned char>(result_string.begin(), result_string.end());
}
