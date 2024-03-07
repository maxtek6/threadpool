#include "threadpool.hpp"

maxtek::threadpool::threadpool(size_t threads) : _workers(threads)
{
    const std::function<void()> worker_function = [&]()
    {
        std::function<void()> task;
        while(pop_task(task))
        {
            task();
        }
    };

    _active.store(true);

    while(_workers.size() < _workers.capacity())
    {
        _workers.push_back(std::thread(worker_function));
    }
}

maxtek::threadpool::~threadpool()
{
    if(!_active)
    {
        shutdown();
    }
}

void maxtek::threadpool::shutdown()
{
    if(_active.load())
    {
        _active.store(false);
    }
}


void maxtek::threadpool::push_task(std::function<void()> &&task)
{
    if(!_active.load())
    {
        throw std::runtime_error("failed to submit to inactive threadpool");
    }

}

bool maxtek::threadpool::pop_task(std::function<void()> &task)
{
    return false;
}

