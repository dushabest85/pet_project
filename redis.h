#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <bredis/bredis.hpp>

namespace asio = boost::asio;
namespace sys = boost::system;
using namespace bredis;

// Простейший класс для работы с Redis
class SimpleRedisClient
{
public:
    SimpleRedisClient(const std::string& host, const std::string& port)
        : _io_context(), _resolver(_io_context), _socket(_io_context)
    {
        connect(host, port);
    }

    void set(const std::string& key, const std::string& value)
    {
        single_command_t cmd{"SET", key, value};
        sys::error_code ec;
        _connection.write(cmd, ec);

        if (ec)
        {
            std::cerr << "Failed to send SET command: " << ec.message() << std::endl;
            return;
        }

        Buffer response;
        auto parse_result = _connection.read(response, ec);
        if (ec)
        {
            std::cerr << "Failed to read response: " << ec.message() << std::endl;
            return;
        }

        std::cout << "SET response: " << parse_result.result << std::endl;
    }

    std::string get(const std::string& key)
    {
        single_command_t cmd{"GET", key};
        sys::error_code ec;
        _connection.write(cmd, ec);

        if (ec)
        {
            std::cerr << "Failed to send GET command: " << ec.message() << std::endl;
            return "";
        }

        Buffer response;
        auto parse_result = _connection.read(response, ec);
        if (ec)
        {
            std::cerr << "Failed to read response: " << ec.message() << std::endl;
            return "";
        }

        std::cout << "GET response: " << parse_result.result << std::endl;

        auto extract = boost::apply_visitor(Extractor(), parse_result.result);
        try
        {
            return boost::get<extracts::string_t>(extract).str;
        }
        catch (boost::bad_get& e)
        {
            std::cerr << "Failed to parse GET response: " << e.what() << std::endl;
            return "";
        }
    }

private:
    void connect(const std::string& host, const std::string& port)
    {
        asio::ip::tcp::resolver::query query(host, port);
        sys::error_code ec;
        auto endpoints = _resolver.resolve(query, ec);

        if (ec)
        {
            std::cerr << "Failed to resolve host: " << ec.message() << std::endl;
            return;
        }

        asio::connect(_socket, endpoints, ec);

        if (ec)
        {
            std::cerr << "Failed to connect: " << ec.message() << std::endl;
            return;
        }

        _connection = std::make_unique<Connection>(_socket);
        std::cout << "Connected to Redis at " << host << ":" << port << std::endl;
    }

    asio::io_context _io_context;
    asio::ip::tcp::resolver _resolver;
    asio::ip::tcp::socket _socket;
    std::unique_ptr<Connection> _connection;
};

int main()
{
    SimpleRedisClient client("127.0.0.1", "6379");

    client.set("mykey", "myvalue");
    std::string value = client.get("mykey");

    std::cout << "Value for 'mykey': " << value << std::endl;

    return 0;
}
