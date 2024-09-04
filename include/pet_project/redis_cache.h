#include "boost/asio/io_service.hpp"
#include "bredis.hpp"
#include <QMutex>
#include <chrono>

namespace asio = boost::asio;
namespace sys = boost::system;

using socket_t = asio::ip::tcp::socket;
using Buffer = asio::streambuf;
using Connection = bredis::Connection<socket_t>;
using Iterator = typename bredis::to_iterator<Buffer>::iterator_t;
using Extractor = bredis::extractor<Iterator>;

using namespace std::chrono_literals;

/**
 * @brief Реализация работы с кэшем Redis
 */
class RedisCache final
{
private:
    static QMutex mutex;
    const std::string ttl = "3600";
    const std::string host;
    const std::string port;
    asio::io_context io_context;
    std::unique_ptr<Connection> connection;
    std::unique_ptr<socket_t> socket;

public:
    RedisCache(const std::string &host, const std::string &port);

public:
    bool remove(const std::string &tag, const std::string &key);
    bool set(const std::string &tag, const std::string &key, const std::string &value);
    std::string get(const std::string &tag, const std::string &key);
    void resolve() ;

private:
    void connect();
    std::string taggedKey(const std::string &tag, const std::string &key) const;
    bool exists(const std::string &key);
    template<typename T>
    T sendCommand(const bredis::single_command_t &cmd, const std::string &key, const bool repeat);
};


QMutex RedisCache::mutex;

RedisCache::RedisCache(const std::string &host, const std::string &port)
    : host(host),
      port(port),
      socket(new asio::ip::tcp::socket(io_context))
{
}

void RedisCache::connect()
{
    if (connection)
        return;

    if (host.empty() || port.empty())
    {
        throw std::runtime_error("host or port is null");
    }

    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::resolver::query query(host, port);

    sys::error_code ec;
    asio::ip::tcp::resolver::iterator iter = resolver.resolve(query, ec);
    if (ec.failed())
    {
        //CACHE_LOGGER->info("redisCache NOT CONNECTED: {}:{} - [{}:{}]", host, port, "can't get endpoint", ec.message());
        return;
    }

    socket->connect(iter->endpoint(), ec);
    if (ec.failed())
    {
        //CACHE_LOGGER->info("redisCache NOT CONNECTED: {}:{} - [{}:{}]", host, port, "can't connect", ec.message());
        return;
    }
    connection.reset(new Connection{std::move(*socket)});

    //CACHE_LOGGER->info("redisCache CONNECTED: {}:{}", host, port);
}

void RedisCache::resolve()
{
    constexpr int numberReconnections = 3;
    for (int i = 0; i < numberReconnections; ++i)
    {
        //CACHE_LOGGER->info("connect to Redis...");

        //Закрываем текущее соединение
        connection.reset();
        socket.reset(new asio::ip::tcp::socket(io_context));

        //Подключаемся заново
        connect();

        if (connection)
        {
            //CACHE_LOGGER->info("connect successfully");
            return;
        }

        std::this_thread::sleep_for(1s);
    }

    throw std::runtime_error("redis reconnection error");
}

bool RedisCache::remove(const std::string &tag, const std::string &key)
{
    if (tag.empty() || key.empty())
    {
        //CACHE_LOGGER->error("incorrect data for setting");
        return false;
    }

    QMutexLocker locker(&mutex);

    if (!connection)
    {
        //CACHE_LOGGER->error("there is no connection to cache");
        resolve();
    }

    const auto tagged = taggedKey(tag, key);
    if (!exists(tagged.data()))
    {
        //CACHE_LOGGER->warn("key not exists '{}'", tagged);
        return false;
    }

    bredis::single_command_t cmd("DEL", tagged);
    sendCommand<bool>(cmd, key, true);

    return true;
}

bool RedisCache::set(const std::string &tag, const std::string &key, const std::string &value)
{
    if (tag.empty() || key.empty() || value.empty())
    {
        //CACHE_LOGGER->error("incorrect data for setting");
        return false;
    }

    QMutexLocker locker(&mutex);

    if (!connection)
    {
        //CACHE_LOGGER->error("there is no connection to cache");
        resolve();
    }

    const auto tagged = taggedKey(tag, key);
    bredis::single_command_t cmd{"SET", tagged, value, "EX", ttl};
    sendCommand<std::string>(cmd, key, true);

    return true;
}

std::string RedisCache::get(const std::string &tag, const std::string &key)
{
    std::string value;
    if (tag.empty() || key.empty())
    {
        //CACHE_LOGGER->error("incorrect key for geting");
        return value;
    }

    QMutexLocker locker(&mutex);

    if (!connection)
    {
        //CACHE_LOGGER->error("there is no connection to cache");
        resolve();
    }

    const auto tagged = taggedKey(tag, key);
    if (!exists(tagged.data()))
    {
        //CACHE_LOGGER->warn("key not exists '{}'", tagged);
        return value;
    }

    bredis::single_command_t cmd{"GET", tagged};
    return sendCommand<std::string>(cmd, key, true);
}

std::string RedisCache::taggedKey(const std::string &tag, const std::string &key) const
{
    if (key.empty())
    {
        return std::string();
    }
    if (tag.empty())
    {
        return std::string{key};
    }

    return tag + ":" + key;
}

bool RedisCache::exists(const std::string &key)
{
    if (key.empty())
    {
        //CACHE_LOGGER->error("incorrect key for geting");
        return false;
    }

    if (!connection)
    {
        //CACHE_LOGGER->error("there is no connection to cache");
        return false;
    }

    bredis::single_command_t cmd{"EXISTS", key};
    return sendCommand<bool>(cmd, key, true);
}

template<typename T>
T RedisCache::sendCommand(const bredis::single_command_t &cmd, const std::string &key, const bool repeat)
{
    sys::error_code ec;
    connection->write(cmd, ec);

    if (ec.failed())
    {
        //CACHE_LOGGER->error("can't send key command '{}': {}, code: {}", key, ec.message(), ec.value());
        if ((ec.value() == sys::errc::no_such_file_or_directory || ec.value() == sys::errc::broken_pipe) && repeat)
        {
            resolve();
            return sendCommand<T>(cmd, key, false);
        }

        throw std::runtime_error("sending error");
    }
    else
    {
        sys::error_code ec;
        Buffer buff;
        const auto parse_result = connection->read(buff, ec);

        if (ec.failed())
        {
            //CACHE_LOGGER->error("can't read key '{}': {}, code: {}", key, ec.message(), ec.value());
            if ((ec.value() == sys::errc::no_such_file_or_directory || ec.value() == sys::errc::broken_pipe) && repeat)
            {
                resolve();
                return sendCommand<T>(cmd, key, false);
            }

            throw std::runtime_error("reading error");
        }
        else
        {
            T value;
            const auto extract = boost::apply_visitor(Extractor(), parse_result.result);
            try
            {
                if constexpr (std::is_same<T, bool>::value)
                {
                    value = static_cast<bool>(boost::get<bredis::extracts::int_t>(extract));
                }
                else if constexpr (std::is_same<T, std::string>::value)
                {
                    value = boost::get<bredis::extracts::string_t>(extract).str;
                }
                else
                {
                    static_assert("Unsupported type");
                }
            }
            catch (const std::exception &e)
            {
                //CACHE_LOGGER->warn("can't extract key '{}': {}", key, e.what());
                throw std::runtime_error("can't key extract");
            }

            return value;
        }
    }
}

int main()
{
    try
    {
        RedisCache client("127.0.0.1", "6379");
        std::cout<<client.set("tag", "key", "value")<<std::endl;

        std::cout<<client.get("tag", "key")<<std::endl;
    }
    catch(...)
    {

    }

    return 0;
}
