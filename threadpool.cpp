#include <iostream>
#include "threadpool.hpp"

maxtek::threadpool::threadpool(size_t threads) : num_threads(threads)
{
    const std::function<void()> worker_function = [&]()
    {
        std::function<void()> task;
        while (pop_task(task))
        {
	    if (task != nullptr)
	    {
	        task();
	    }
        }
    };

    _active = true;

    for (int i = 0; i < num_threads; ++i)
    {
        _workers.push_back(std::thread(worker_function));
    }

    for (std::thread& t : _workers)
        t.detach();
}

maxtek::threadpool::~threadpool()
{
    if (!_active)
    {
        shutdown();
    }
}

void maxtek::threadpool::shutdown()
{
    if (_active)
    {
        _active = false;
        _condition.notify_all();
        for(std::thread& worker : _workers)
        {
            worker.join();
        }
    }
}

void maxtek::threadpool::push_task(std::function<void()> &&task)
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (!_active)
    {
        throw std::runtime_error("failed to submit to inactive threadpool");
    }
    _tasks.push(std::move(task));
    lock.unlock();
    _condition.notify_one();
}

bool maxtek::threadpool::pop_task(std::function<void()> &task)
{
    std::unique_lock<std::mutex> lock(_mutex);
    bool result(false);
    _condition.wait(
        lock,
        [&]()
        { 
            return (!_active || !_tasks.empty()); 
        });
    if (_active)
    {
        task = _tasks.front();
        _tasks.pop();
        result = true;
    }
    return result;
}
