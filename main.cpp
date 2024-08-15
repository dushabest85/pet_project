#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "../../../../estimate/common/cpp-bredis/include/bredis.hpp"
#include <QDebug>

using namespace bredis;

namespace asio = boost::asio;
namespace sys = boost::system;

using socket_t = asio::ip::tcp::socket;
using Buffer = asio::streambuf;
using Iterator = bredis::to_iterator<Buffer>::iterator_t;
using Extractor = bredis::extractor<Iterator>;

class SimpleRedisClient
{
public:
    SimpleRedisClient(const std::string& host, const std::string& port)
        : _host(host),
          _port(port),
          _socket(new asio::ip::tcp::socket(_io_context)),
          _timer(_io_context)
    {
        connect();
    }

    // void print_message(const boost::system::error_code& /*e*/)
    // {
    //     std::cout << "Таймер сработал" << std::endl;
    //     reconnect();
    // }

    // void start_timer()
    // {
    //     // Общее время работы таймера в секундах
    //     const int total_time = 6;
    //     // Интервал между срабатываниями таймера в секундах
    //     const int interval = 2;

    //     // Создаём таймер
    //     boost::asio::steady_timer timer(_io_context, std::chrono::seconds(interval));

    //     // Функция, которая будет вызываться по истечении интервала
    //     std::function<void(const boost::system::error_code&)> handler = [&](const boost::system::error_code& e)
    //     {
    //         if (!e)
    //         {
    //             print_message(e);

    //             // Устанавливаем таймер на следующий интервал
    //             timer.expires_at(timer.expiry() + std::chrono::seconds(interval));
    //             timer.async_wait(handler);
    //         }
    //     };

    //     // Запускаем таймер
    //     timer.async_wait(handler);

    //     // Запускаем основной цикл обработки событий
    //     _io_context.run_for(std::chrono::seconds(total_time));
    // }

public:
    bool set(const std::string& key, const std::string& value)
    {
        if (!_connection)
            return reconnect();

        single_command_t cmd{"SET", key, value};
        sys::error_code ec;

        _connection->write(cmd, ec);
        if (ec)
        {
            return reconnect();
        }

        // Успешная отправка
        Buffer response;
        auto parse_result = _connection->read(response, ec);
        if (ec)
        {
            return reconnect();
        }

        // Успешное чтение
        auto extract = boost::apply_visitor(Extractor(), parse_result.result);
        try
        {
            std::cout<<"data has been set: " << boost::get<bredis::extracts::string_t>(extract).str << std::endl;
        }
        catch (boost::bad_get &e)
        {
            std::cout<<"can't set data: " << boost::get<bredis::extracts::error_t>(extract).str <<std::endl;
            return reconnect();
        }
        catch (std::exception &e)
        {
            std::cout<<"can't set data: " << e.what() <<std::endl;
            return reconnect();
        }

        return true;
    }

    // std::string get(const std::string& key)
    // {
    //     if (!_connection)
    //         return "error";

    //     single_command_t cmd{"GET", key};
    //     sys::error_code ec;

    //     _connection->write(cmd, ec);
    //     if (!ec)
    //     {
    //         // Успешная отправка
    //         Buffer response;
    //         auto parse_result = _connection->read(response, ec);
    //         if (!ec)
    //         {
    //             // Успешное чтение
    //             auto extract = boost::apply_visitor(Extractor(), parse_result.result);
    //             try
    //             {
    //                 return boost::get<extracts::string_t>(extract).str;
    //             }
    //             catch (boost::bad_get& e)
    //             {
    //                 std::cerr << "Failed to parse GET response: " << e.what() << std::endl;
    //             }
    //         }
    //     }

    //     std::cerr << "Error during GET command: " << ec.message() << std::endl;

    //     return "";
    // }

private:
    void connect()
    {
        asio::ip::tcp::resolver::query query(_host, _port);
        asio::ip::tcp::resolver resolver(_io_context);

        sys::error_code ec;
        asio::ip::tcp::resolver::iterator endpoints = resolver.resolve(query, ec);

        if (ec)
        {
            std::cerr << "Failed to resolve host: " << ec.message() << std::endl;
            return;
        }

        _socket->connect(endpoints->endpoint(), ec);

        if (ec)
        {
            std::cerr << "Failed to connect ** : " << ec.message() << ec.value() << std::endl;
            return;
        }

        _connection = std::make_unique<bredis::Connection<socket_t>>(std::move(*_socket));

        std::cout << "Connected to Redis at " << _host << ":" << _port << std::endl;
    }

    bool reconnect()
    {
        for (int i = 0; i < 5; ++i)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            std::cout << "Reconnecting to Redis..." << std::endl;

            // Закрываем текущее соединение
            _connection.reset();
            _socket.reset(new asio::ip::tcp::socket(_io_context));

            // Подключаемся заново
            connect();

            if (_connection)
            {
                std::cout << "Reconnected successfully." << std::endl;
                return true;
            }

            std::cerr << "Failed to reconnect." << std::endl;
        }

        return false;
    }

    std::string _host;
    std::string _port;
    asio::io_context _io_context;
    std::unique_ptr<socket_t> _socket;
    std::unique_ptr<bredis::Connection<socket_t>> _connection;
    boost::asio::steady_timer _timer;
};

int main()
{
    SimpleRedisClient client("127.0.0.1", "6379");

    try
    {
        int age = 0;

        std::cout << "Enter 1"<<std::endl;
        std::cin >> age;

        client.set("mykey1", "myvalue1");
        //std::string value1 = client.get("mykey1");

        //std::cout << "Value from 'mykey1': " << value1 << std::endl;

        std::cout << "Enter 2"<<std::endl;
        std::cin >> age;

        client.set("mykey2", "myvalue2");
        //std::string value2 = client.get("mykey2");

        //std::cout << "Value from 'mykey2': " << value2 << std::endl;

        std::cout << "Enter 3"<<std::endl;
        std::cin >> age;

        client.set("mykey3", "myvalue3");
        //std::string value3 = client.get("mykey3");

        //std::cout << "Value from 'mykey3': " << value3 << std::endl;

        std::cout << "Enter 4"<<std::endl;
        std::cin >> age;

        client.set("mykey4", "myvalue4");
        //std::string value3 = client.get("mykey3");
    }
    catch(...)
    {

    }

    return 0;
}


// #include <boost/asio.hpp>
// #include <iostream>
// namespace asio = boost::asio;
// using boost::system::error_code;

// using namespace std::chrono_literals;
// auto now = std::chrono::steady_clock::now;

// int main() {
//     asio::io_service io;
//     asio::steady_timer t(io, 1s);
//     int count = 0;

//     std::function<void(error_code)> handler;

//     auto const start = now();
//     handler = [&](error_code ec) {
//         std::cout << "at " << (now() - start) / 1.0ms
//                   << "ms handler: " << ec.message() << '\n';
//         if (ec.failed())
//             return;

//         if (count++ < 10) {
//             t.expires_from_now(1s);
//             t.async_wait(handler);
//         }
//     };

//     t.async_wait(handler);

//     io.run();
//     std::cout << "at " << (now() - start) / 1.0ms << " Done\n";
// }
