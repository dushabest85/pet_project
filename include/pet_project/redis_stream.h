#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>

class RedisSubscriber {
public:
    RedisSubscriber(boost::asio::io_context& io_context, const std::string& host, const std::string& port)
        : resolver_(io_context), socket_(io_context) {
        std::cout << "Initializing Redis subscriber..." << std::endl;

        // Запуск асинхронного разрешения имени хоста
        boost::asio::ip::tcp::resolver::query query(host, port);
        resolver_.async_resolve(query,
                                boost::bind(&RedisSubscriber::handleResolve, this, boost::asio::placeholders::error, boost::asio::placeholders::results));
    }

private:
    void handleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::results_type endpoints) {
        if (!err) {
            std::cout << "Resolved host successfully." << std::endl;

            // Асинхронное подключение к одному из разрешенных эндпоинтов
            boost::asio::async_connect(socket_, endpoints,
                                       boost::bind(&RedisSubscriber::handleConnect, this, boost::asio::placeholders::error));
        } else {
            std::cerr << "Error resolving host: " << err.message() << std::endl;
        }
    }

    void handleConnect(const boost::system::error_code& err) {
        if (!err) {
            std::cout << "Connected to server. Sending XGROUP CREATE command..." << std::endl;

            // Отправка команды XGROUP CREATE для создания группы потребителей
            std::ostream request_stream(&request_);
            request_stream << "XGROUP CREATE calplan_stream my_group 0 MKSTREAM\r\n";

            boost::asio::async_write(socket_, request_,
                                     boost::bind(&RedisSubscriber::handleCreateGroupResponse, this, boost::asio::placeholders::error));
        } else {
            std::cerr << "Error connecting to server: " << err.message() << std::endl;
        }
    }

    void handleCreateGroupResponse(const boost::system::error_code& err) {
        if (!err) {
            std::cout << "XGROUP CREATE command sent. Waiting for the response..." << std::endl;

            // Ожидание ответа на команду XGROUP CREATE
            boost::asio::async_read_until(socket_, response_, "\r\n",
                                          boost::bind(&RedisSubscriber::handleCreateGroupResponseRead, this, boost::asio::placeholders::error));
        } else {
            std::cerr << "Error sending XGROUP CREATE command: " << err.message() << std::endl;
        }
    }

    void handleCreateGroupResponseRead(const boost::system::error_code& err) {
        if (!err) {
            std::istream response_stream(&response_);
            std::string response_line;
            std::getline(response_stream, response_line);

            std::cout << "XGROUP CREATE response: " << response_line << std::endl;

            // Теперь отправляем команду XREADGROUP
            sendXReadGroup();
        } else {
            std::cerr << "Error reading XGROUP CREATE response: " << err.message() << std::endl;
        }
    }

    void sendXReadGroup() {
        std::ostream request_stream(&request_);
        request_stream << "XREADGROUP GROUP my_group my_consumer BLOCK 0 COUNT 10 STREAMS calplan_stream >\r\n";

        boost::asio::async_write(socket_, request_,
                                 boost::bind(&RedisSubscriber::handleRead, this, boost::asio::placeholders::error));
    }

    void handleRead(const boost::system::error_code& err) {
        if (!err) {
            // Обработка ответа и чтение всех данных
            boost::asio::async_read_until(socket_, response_, "\r\n",
                                          boost::bind(&RedisSubscriber::handleReadResponse, this, boost::asio::placeholders::error));
        } else if (err == boost::asio::error::eof) {
            std::cerr << "Connection closed by the server." << std::endl;
        } else if (err == boost::asio::error::operation_aborted) {
            std::cerr << "Operation aborted." << std::endl;
        } else {
            std::cerr << "Error during read: " << err.message() << std::endl;
        }
    }

    void handleReadResponse(const boost::system::error_code& err) {
        if (!err) {
            std::istream response_stream(&response_);
            std::string line;

            std::cout << "Reading server response..." << std::endl;

            // Чтение и вывод всех строк сообщения
            while (std::getline(response_stream, line)) {
                std::cout << "Received: " << line << std::endl;
            }

            // После обработки сообщения снова вызвать XREADGROUP
            std::cout << "Waiting for the next message..." << std::endl;
            sendXReadGroup();
        } else if (err == boost::asio::error::eof) {
            std::cerr << "Connection closed by the server." << std::endl;
        } else if (err == boost::asio::error::operation_aborted) {
            std::cerr << "Operation aborted." << std::endl;
        } else {
            std::cerr << "Error during read: " << err.message() << std::endl;
        }
    }

    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;
};

int main() {
    try {
        std::cout << "Starting Redis subscriber..." << std::endl;

        boost::asio::io_context io_context;
        RedisSubscriber subscriber(io_context, "10.145.1.150", "6385");

        std::cout << "Running IO context..." << std::endl;
        io_context.run();

        std::cout << "IO context finished." << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

