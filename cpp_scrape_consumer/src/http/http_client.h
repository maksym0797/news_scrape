#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <nlohmann/json.hpp>
#include "../config/config.h"
namespace json = nlohmann;
class HttpClient
{
public:
    HttpClient(Config &config);
    ~HttpClient();

    json::json get(const std::string &path);
    json::json post(const std::string &path, const nlohmann::json &data);

private:
    std::string host;
    std::string port;
    json::json make_request(const std::string &path, const std::string &method, const nlohmann::json &data);
};

#endif // HTTP_CLIENT_H