#include <iostream>
#include <threadpool.hpp>
#include <random>
#include <condition_variable>

using namespace maxtek;

class consumerProducerExample
{
    public:
        consumerProducerExample(size_t num_consumer_threads = std::thread::hardware_concurrency(),
        size_t num_producer_threads = std::thread::hardware_concurrency(),
        size_t buffer_size = 10, int consumer_thread_sleep = 100,
        int producer_thread_sleep = 100): num_consumer_threads(num_consumer_threads),
        num_producer_threads(num_producer_threads), buffer_size(buffer_size),
        consumer_thread_sleep(consumer_thread_sleep),
        producer_thread_sleep(producer_thread_sleep)
        {
            item_buf.reserve(buffer_size);
            producer_futures.reserve(num_producer_threads);
            myThreadPool = std::make_unique<threadpool>(num_producer_threads + num_consumer_threads);
        }

        void run()
        {
            buffering_active = true;
            for (int i = 0 ; i < num_consumer_threads; ++i)
            {
                auto consumerTask = std::bind(&consumerProducerExample::consumerThread, this);
                myThreadPool->submit(consumerTask);
            }

            for (int i = 0; i < num_producer_threads; ++i)
            {
                auto producerTask = std::bind(&consumerProducerExample::producerThread, this);
                producer_futures[i] = myThreadPool->submit(producerTask);
            }
        }

        void stop()
        {
            std::unique_lock<std::mutex> lock(mtx);
            buffering_active = false;
            lock.unlock();
            cv.notify_all();
            for (int i = 0; i < num_producer_threads; ++i)
            {
                std::cout << "Producer: " << (i + 1) << ", Num bytes: " << producer_futures[i].get() << "\n";
            }
            myThreadPool->shutdown();
        }

        ~consumerProducerExample()
        {
            if (buffering_active)
            {
                stop();
            }
        }
    private:
        size_t num_consumer_threads;
        size_t num_producer_threads;
        size_t buffer_size;
        // in ms
        int consumer_thread_sleep;
        // in ms
        int producer_thread_sleep;
        bool buffering_active;
        std::mutex mtx;
        std::condition_variable cv;
        std::vector<int> item_buf;
	std::unique_ptr<threadpool> myThreadPool;
        std::vector<std::future<int>> producer_futures;

        int get_random_int()
        {
            std::random_device rd;
            std::mt19937 eng(rd());
            std::uniform_int_distribution<> distr(1,100);

            return distr(eng);
        }

        int producerThread()
        {
            int count = 0;
            int item;
            while (buffering_active)
            {
                item = get_random_int();
                std::unique_lock<std::mutex> lock(mtx);
                std::cout << "Bytes written: " << sizeof(int) * count << "  " << "from thread_id: " << std::this_thread::get_id() << "\n";
                if (item_buf.size() == item_buf.capacity())
                {
                    cv.wait(lock, [&] { return (item_buf.size() < item_buf.capacity() || !buffering_active); });
                }
                item_buf.push_back(item);
                ++count;
                lock.unlock();
                cv.notify_one();
                std::this_thread::sleep_for(std::chrono::milliseconds(get_random_int()));
            }
            return (sizeof(int) * count);
         }

         void consumerThread()
         {
             while (buffering_active)
             {
                 int item;
                 std::unique_lock<std::mutex> lock(mtx);
                 if (item_buf.size() == 0)
                 {
                     cv.wait(lock, [&] { return (item_buf.size() > 0 || !buffering_active); });
                 }
                 item = item_buf.back();
                 item_buf.pop_back();
                 std::cout << "thread id: " << std::this_thread::get_id() << " consumed item: " << item << "\n";
                 lock.unlock();
                 cv.notify_one();
                 std::this_thread::sleep_for(std::chrono::milliseconds(consumer_thread_sleep));
             }
         }

};

int main()
{
    consumerProducerExample example;
    example.run();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    example.stop();

    return 0;
}
