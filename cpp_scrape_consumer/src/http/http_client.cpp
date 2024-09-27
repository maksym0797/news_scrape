#include "http_client.h"
#include <iostream>
#include <string>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <nlohmann/json.hpp>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

HttpClient::HttpClient(Config &config)
{
    host = config.get("HTTP_HOST");
    port = config.get("HTTP_PORT");
}

HttpClient::~HttpClient()
{
}

nlohmann::json HttpClient::get(const std::string &path)
{
    std::string method = "GET";
    nlohmann::json data;
    return make_request(path, method, data);
}

nlohmann::json HttpClient::post(const std::string &path, const nlohmann::json &data)
{
    std::string method = "POST";
    return make_request(path, method, data);
}

nlohmann::json HttpClient::make_request(const std::string &path, const std::string &method, const nlohmann::json &data)
{
    try
    {
        // Set up an I/O context
        net::io_context ioc;

        // Create a resolver to turn the host into an IP address
        tcp::resolver resolver(ioc);

        std::cout << "http-host: " << host << " port: " << port << " path: " << path << std::endl;
        auto const results = resolver.resolve(host, port);

        // Set up a TCP socket
        beast::tcp_stream stream(ioc);
        stream.connect(results);

        // Set up an HTTP POST request
        http::request<http::string_body> req{method == "GET" ? http::verb::get : http::verb::post, path, 11};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");
        req.body() = data.dump(); // Convert JSON to string
        req.prepare_payload();

        // Send the HTTP request to the remote host
        http::write(stream, req);

        // Receive the HTTP response
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        // Gracefully close the socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);
        if (ec && ec != beast::errc::not_connected)
            throw beast::system_error{ec};

        // Return the body of the response
        return nlohmann::json::parse(res.body());
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return nlohmann::json();
    }
}
