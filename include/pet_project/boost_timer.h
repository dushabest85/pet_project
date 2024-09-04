#include <iostream>
#include <boost/asio.hpp>

class Timer
{
public:
    Timer() : _timer(_io_context)
    {
        start_timer();
    }

    void print_message(const boost::system::error_code& /*e*/)
    {
        std::cout << "Таймер сработал" << std::endl;
    }

    void start_timer()
    {
        // Общее время работы таймера в секундах
        const int total_time = 6;
        // Интервал между срабатываниями таймера в секундах
        const int interval = 2;

        // Создаём таймер
        boost::asio::steady_timer timer(_io_context, std::chrono::seconds(interval));

        // Функция, которая будет вызываться по истечении интервала
        std::function<void(const boost::system::error_code&)> handler = [&](const boost::system::error_code& e)
        {
            if (!e)
            {
                print_message(e);

                // Устанавливаем таймер на следующий интервал
                timer.expires_at(timer.expiry() + std::chrono::seconds(interval));
                timer.async_wait(handler);
            }
        };

        // Запускаем таймер
        timer.async_wait(handler);

        // Запускаем основной цикл обработки событий
        _io_context.run_for(std::chrono::seconds(total_time));
    }

    boost::asio::io_context _io_context;
    boost::asio::steady_timer _timer;
};

int main()
{
    Timer timer;

    return 0;
}
