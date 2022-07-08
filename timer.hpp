#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

class Timer
{
private:
    std::chrono::duration<int> timeout;

    bool running = false;
    std::mutex mx;
    std::condition_variable cond;
    std::thread t_time;
    std::function<void()> func;

    void worker()
    {
        std::unique_lock<std::mutex> guard(mx);
        cond.wait_for(guard, timeout);
        if (running)
            func();
        cond.notify_one();
        running = false;
    }

public:
    Timer(int minutes, std::function<void()> donefunc)
    {
        timeout = std::chrono::minutes(minutes);
        func = std::move(donefunc);
    }

    bool isRunning() { return running; }

    void setTimeout(int minutes)
    {
        timeout = std::chrono::minutes(minutes);
    }

    void start(uint32_t minutes = 0)
    {
        if (minutes != 0)
            timeout = std::chrono::minutes(minutes);
        if (running)
            stop();
        running = true;
        t_time = std::thread(&Timer::worker, this);
    }

    void stop()
    {
        if (running)
        {
            running = false;
            cond.notify_one();
            t_time.join();
        }
    }
};
