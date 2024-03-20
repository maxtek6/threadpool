/*
** Copyright 2024 Maxtek Consulting
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*/

#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <algorithm>
#include <functional>
#include <future>
#include <memory>
#include <queue>
#include <thread>
#include <typeinfo>
#include <vector>

namespace maxtek
{
    class threadpool
    {
    public:
        threadpool(size_t threads = std::thread::hardware_concurrency());

        ~threadpool();

        template <class F, class... Args>
        std::future<std::result_of_t<F(Args...)>> submit(F &&function, Args &&...args)
        {
            std::shared_ptr<std::packaged_task<std::result_of_t<F(Args...)>()>> packaged_task;

            packaged_task = std::make_shared<std::packaged_task<std::result_of_t<F(Args...)>()>>(std::bind(std::forward<F>(function), std::forward<Args>(args)...));

            push_task(
                std::move(
                    [&]()
                    { 
                        (*packaged_task)(); 
                    }));

            return packaged_task->get_future();
        }

        void shutdown();

    private:
        void push_task(std::function<void()> &&task);
        bool pop_task(std::function<void()> &task);

	size_t num_threads;
        bool _active;
        std::vector<std::thread> _workers;
        std::queue<std::function<void()>> _tasks;
        std::mutex _mutex;
        std::condition_variable _condition;
    };
}
#endif
