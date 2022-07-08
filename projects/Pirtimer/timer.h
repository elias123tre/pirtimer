#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

class Timer
{
private:
	void worker();

	std::chrono::duration<int> timeout;
	bool running = false;
	std::mutex mx;
	std::condition_variable cond;
	std::thread t_time;
	std::function<void()> func;
public:
	Timer(std::chrono::duration<int> waittime, std::function<void()> donefunc);

	bool isRunning();
	void setTimeout(std::chrono::duration<int> waittime);
	void start(std::chrono::duration<int> waittime = std::chrono::minutes(0));
	void stop();
};