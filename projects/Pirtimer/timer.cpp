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
	Timer(std::chrono::duration<int> waittime, std::function<void()> donefunc)
	{
		timeout = waittime;
		func = std::move(donefunc);
	}

	bool isRunning() { return running; }

	void setTimeout(std::chrono::duration<int> waittime)
	{
		timeout = waittime;
	}

	void start(std::chrono::duration<int> waittime = std::chrono::minutes(0))
	{
		if (waittime != std::chrono::minutes(0))
			timeout = waittime;
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