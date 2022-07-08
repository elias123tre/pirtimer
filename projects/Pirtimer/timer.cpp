#include "timer.h"

void Timer::worker()
{
	std::unique_lock<std::mutex> guard(mx);
	cond.wait_for(guard, timeout);
	if (running)
		func();
	cond.notify_one();
	running = false;
}

Timer::Timer(std::chrono::duration<int> waittime, std::function<void()> donefunc)
{
	timeout = waittime;
	func = std::move(donefunc);
}

bool Timer::isRunning()
{
	return running;
}

void Timer::setTimeout(std::chrono::duration<int> waittime)
{
	timeout = waittime;
}

void Timer::start(std::chrono::duration<int> waittime)
{
	if (waittime != std::chrono::minutes(0))
		timeout = waittime;
	if (running)
		stop();
	running = true;
	t_time = std::thread(&Timer::worker, this);
}

void Timer::stop()
{
	if (running)
	{
		running = false;
		cond.notify_one();
		t_time.join();
	}
}